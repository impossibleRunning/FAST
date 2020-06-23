#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "fastblk.h"

extern int read;
extern int write;
extern int used_blk;
extern int erase;
extern int merge_target_blk;
extern RWMapTbl RWtbl;
extern SWMapTbl SWtbl;
extern BlockMapTbl datatable;
extern SpareData spare[BLOCKS_PER_DEVICE*PAGES_PER_BLOCK];
extern FREEBLK freeblk[FREEBLOCKS];

int freeBlockChange(int ppn);
void erasePBN(int pbn);
void merge(int LBN, int LSN);
void writeToDatablk2(int LBN, int offset);

int rwtbl_isFull() {
	return ((RWtbl.rear + 1) % (RW_LOG_BLOCK*PAGES_PER_BLOCK+1) == RWtbl.front);
}

void write_to_rwtbl_derictly(int lsn) {
	int i = RWtbl.front;
	int LBN = lsn / PAGES_PER_BLOCK;
	int offset = lsn % PAGES_PER_BLOCK;
	int count = 0;
	for (i; count < RW_LOG_BLOCK*PAGES_PER_BLOCK; i=(i+1)%(RW_LOG_BLOCK*PAGES_PER_BLOCK),count++) {
//		read++;
		if (RWtbl.entry[i].lsn == lsn) {
			RWtbl.entry[i].valid = 0;
		}
		if (RWtbl.entry[i].lsn == -1) {
			RWtbl.entry[i].lsn = lsn;
			RWtbl.entry[i].valid = 1;
			spare[RWtbl.entry[i].psn].lsn = lsn;
			spare[datatable.entry[LBN].pbn*PAGES_PER_BLOCK + offset].valid = 0;
			write++;
			break;
		}
	}
	RWtbl.rw_tbl_count++;
	RWtbl.rear=(RWtbl.rear+1)%(RW_LOG_BLOCK*PAGES_PER_BLOCK+1);
}

void rw_log_blk_merge(int lsn) {
	int h = RWtbl.front;
	int r = RWtbl.rear;
	int count = 0;
	for (h; count < PAGES_PER_BLOCK; h++,count++) {
		if (RWtbl.entry[h].valid == 1) {
			int LBN = RWtbl.entry[h].lsn / PAGES_PER_BLOCK;
			merge(LBN,lsn);

		}
		RWtbl.entry[h].lsn = -1;
		RWtbl.entry[h].valid = 0;

	}
//	printf("front: %d, psn: %d", RWtbl.front, RWtbl.entry[h].psn);
	erasePBN(RWtbl.entry[RWtbl.front].psn / PAGES_PER_BLOCK);	

	RWtbl.rw_tbl_count = RWtbl.rw_tbl_count - PAGES_PER_BLOCK;
	RWtbl.front = (RWtbl.front + PAGES_PER_BLOCK) % (RW_LOG_BLOCK*PAGES_PER_BLOCK);
	int LBN = lsn / PAGES_PER_BLOCK;
	int offset = lsn % PAGES_PER_BLOCK;

	//write lsn 
	writeToDatablk2(LBN, offset);
	RWtbl.rear = (RWtbl.rear + 1) % (RW_LOG_BLOCK*PAGES_PER_BLOCK + 1);
}

void merge(int LBN, int LSN) {
	int getfreeblk = freeblk[0];
	int offset;
	int PBN = datatable.entry[LBN].pbn;
//	int psn;
	int miss = 1;

	// traverse rw log block and find the lsn in lbn_to_merge
	int h = RWtbl.front;
	int r = RWtbl.rear;
	int count = 0;
	for (r; count < PAGES_PER_BLOCK*RW_LOG_BLOCK; r--, count++) {//backward search in rw table

//		read++;
		if (r < 0) {
			r = PAGES_PER_BLOCK * RW_LOG_BLOCK - 1;
		}
		
		int lsn_in_rwtbl = RWtbl.entry[r].lsn;
		int lbn_in_rwtbl = lsn_in_rwtbl / PAGES_PER_BLOCK;

		if (lbn_in_rwtbl == LBN && RWtbl.entry[r].valid == 1&&RWtbl.entry[r].lsn!=LSN) {//copy from rw log block to free block
			write++;
			offset = lsn_in_rwtbl % PAGES_PER_BLOCK;
			spare[getfreeblk*PAGES_PER_BLOCK + offset].lsn = RWtbl.entry[r].lsn;
			spare[getfreeblk*PAGES_PER_BLOCK + offset].valid = 1;
			RWtbl.entry[r].valid = 0;

		}
	}
	int i = 0;
	for (i; i < PAGES_PER_BLOCK; i++){//copy from data block to free block
		read++;
		if (spare[PBN*PAGES_PER_BLOCK + i].valid == 1) {
			write++;
			spare[getfreeblk*PAGES_PER_BLOCK + i].lsn = spare[PBN*PAGES_PER_BLOCK + i].lsn;
			spare[getfreeblk*PAGES_PER_BLOCK + i].valid = 1;
			spare[PBN*PAGES_PER_BLOCK + i].valid = 0;
		}
		if (spare[getfreeblk*PAGES_PER_BLOCK + i].lsn == -2) {
			spare[getfreeblk*PAGES_PER_BLOCK + i].lsn = -1;
		}
	}			
	erasePBN(PBN);
	freeBlockChange(PBN);
	//update datablk mapping table
	datatable.entry[LBN].pbn = getfreeblk;


}
