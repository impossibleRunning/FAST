#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "fastblk.h"

extern int lbn;
extern int used_blk;
extern int erase;
extern int write;
extern int read;
extern int repeat;
extern int copy;
extern int freeblock;
extern int merge_target_blk;

extern FREEBLK freeblk[FREEBLOCKS];
extern SpareData spare[BLOCKS_PER_DEVICE*PAGES_PER_BLOCK];
extern BlockMapTbl datatable;
extern RWMapTbl RWtbl;
extern SWMapTbl SWtbl;



void ftl_open() {
	erase = 0;
	write = 0;
	read = 0;
	copy = 0;
	used_blk = 0;

	int i = 0;
	// spare space
	for (i; i < PAGES_PER_BLOCK*BLOCKS_PER_DEVICE; i++) {
		spare[i].lsn = -1;
		spare[i].valid = 0;
	}
	// data blk mapping
	for (i = 0; i < DATABLKS_PER_DEVICE; i++) {
		datatable.entry[i].pbn = -1;
	}
	//free blk
	for (i = 0; i < FREEBLOCKS; i++) {
		freeblk[i] = DATABLKS_PER_DEVICE + i;
	}

	for (i = 0; i < FREEBLOCKS; i++) {
		for (int j = 0; j < PAGES_PER_BLOCK; j++) {
			spare[freeblk[i]*PAGES_PER_BLOCK + j].lsn = -2;
		}
		

	}
	//rw blk mapping
	RWtbl.rw_tbl_count = 0;
	RWtbl.front = 0;
	RWtbl.rear = 0;
	for (i = 0; i < RW_LOG_BLOCK*PAGES_PER_BLOCK; i++) {
		RWtbl.entry[i].lsn = -1;
		RWtbl.entry[i].psn = (BLOCKS_PER_DEVICE-RW_LOG_BLOCK)*PAGES_PER_BLOCK+i;
		RWtbl.entry[i].valid = 0;
//		RWtbl.entry[i].lsns[PAGES_PER_BLOCK] = -1;
	}
	//sw blk mapping
	SWtbl.lbn = -1;
	SWtbl.pbn = BLOCKS_PER_DEVICE-LOG_BLOCK;
	SWtbl.sw_sec_num = 0;
}

int ftl_read(int lsn) {
	/* 
	   read from sw mapping table
	   read from rw mapping table
	   read from blk mapping table
	   retrurn < PSN >
	*/
	int LBN = lsn / PAGES_PER_BLOCK;
	int offset = lsn % PAGES_PER_BLOCK;
	int ppn = read_from_sw_tbl(LBN, offset);
	return ppn;

}

void ftl_write(int lsn) {
	int LBN = lsn / PAGES_PER_BLOCK;
	int offset = lsn % PAGES_PER_BLOCK;

	int PBNinData = get_PBN_From_DataTbl(LBN);

	//---------write to data block----------
	if (PBNinData == -1) {
		writeToDatablk1(LBN, offset);
	}

	else {
		int LSNinSpare = get_lsn_From_Spare(LBN, offset);
		if (LSNinSpare == -1) {
			writeToDatablk2(LBN, offset);
		}
		else {
			spare[datatable.entry[LBN].pbn*PAGES_PER_BLOCK + offset].valid = 0;
			int LBN_in_swtbl, swsecnum;
			get_lbn_from_swtbl(&LBN_in_swtbl, &swsecnum);
			//over write
			int condition = writeCondition(LBN, offset);
//			printf("lsn: %d condition: %d\n", lsn,condition);
			if (condition == 1) {
				//condition 1: wirte to sequencial write block and update sw table  
				
				
				if (LBN_in_swtbl == -1) {
					
					// write to sw log block derictly
					write_to_swtbl_derictly(LBN, lsn);
				}
				else if (LBN_in_swtbl != -1 && swsecnum == PAGES_PER_BLOCK) {
	
					// sw log block switch
					sw_log_blk_swicth(SWtbl.pbn, PBNinData);
					write_to_swtbl_derictly(LBN, lsn);
				}
				else if (LBN_in_swtbl != -1 && swsecnum < PAGES_PER_BLOCK) {

					// sw log block merge
//					printf("%d, %d, %d\n", SWtbl.pbn, PBNinData, swsecnum);
					sw_log_blk_merge(SWtbl.pbn, datatable.entry[SWtbl.lbn].pbn,swsecnum);
					write_to_swtbl_derictly(LBN, lsn);
				}
			}

			else if (condition == 2) {
				//
				write_to_swtbl(LBN, offset);
			}

			else if (condition == 3) {
				//sw log block merge
				write_to_swtbl(LBN, offset);
				sw_log_blk_merge(SWtbl.pbn, PBNinData, swsecnum);
			}

			else if (condition == 4) {
				//over write on sw log block
				
				if (SWtbl.sw_sec_num = PAGES_PER_BLOCK) {
					sw_log_blk_swicth(SWtbl.pbn,PBNinData);
					write_to_swtbl(LBN, offset);
					SWtbl.lbn = LBN;
				}
				else {
					sw_log_blk_merge(SWtbl.pbn, PBNinData, swsecnum);
					write_to_swtbl(LBN, offset);
					SWtbl.lbn = LBN;
				}
			}
			else if (condition == 5) {
				//write on rw block
				//rw block merge if necessary
//				printf("front:%d, rear: %d", RWtbl.front, RWtbl.rear);
				
				if (rwtbl_isFull() != 1) {
//					printf("not full\n");
					//write on rw block directly
					write_to_rwtbl_derictly(lsn);
				}
				else if (rwtbl_isFull()) {
//					printf("full\n");
					//rw block merge
					rw_log_blk_merge(lsn);

				}

			}

		}
	
	}	
}


void printTable() {
	/*
	printf("---- datablk ------\n");

	printf("lbn\tpbn\n");
	for (lbn = 0; lbn < DATABLKS_PER_DEVICE; lbn++) {
		printf("%d\t%d\n", lbn, datatable.entry[lbn].pbn);
	}

	printf("---- spare -----\n");
	printf("spare\tlsn\tvalid\n");
	for (lbn = 0; lbn < BLOCKS_PER_DEVICE * PAGES_PER_BLOCK; lbn++) {
		
		printf("%d\t%d\t%d\n", lbn, spare[lbn].lsn,spare[lbn].valid);

	}
	*/

	printf("------rw mapping tbl------\n");
	printf("lsn\tpsn\n");
	for (lbn = 0; lbn < RW_LOG_BLOCK*PAGES_PER_BLOCK; lbn++) {
		printf("%d\t%d\t%d", RWtbl.entry[lbn].lsn, RWtbl.entry[lbn].psn,RWtbl.entry[lbn].valid);
		printf("\n");
	}


	printf("------sw mapping tbl ------\n");
	printf("lbn\tpbn\tsw_swc_num\n");
	printf("%d\t%d\t", SWtbl.lbn, SWtbl.pbn);
	printf("%d\n", SWtbl.sw_sec_num);
	

}