#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "fastblk.h"

extern int read;
extern int write;
extern int used_blk;
extern int erase;
extern RWMapTbl RWtbl;
extern SWMapTbl SWtbl;
extern BlockMapTbl datatable;
extern SpareData spare[BLOCKS_PER_DEVICE*PAGES_PER_BLOCK];
extern FREEBLK freeblk[FREEBLOCKS];


//----------return free block from freeblk list------------
int freeBlockChange(int ppn) {
	int toDataBlock = freeblk[0];
	for (int i = 0; i < PAGES_PER_BLOCK; i++) {
//		spare[toDataBlock*PAGES_PER_BLOCK + i].lsn = -1;
		spare[ppn*PAGES_PER_BLOCK + i].lsn = -2;
	}
	
	int i = 0;
	for (i; i < FREEBLOCKS - 1; i++) {
		freeblk[i] = freeblk[i + 1];
	}
	freeblk[i] = ppn;
	return toDataBlock;
}

//-----------erase operation-----------------

void erasePBN(int pbn) {
	for (int i = 0; i < PAGES_PER_BLOCK; i++) {
		spare[pbn*PAGES_PER_BLOCK + i].lsn = -1;
		spare[pbn*PAGES_PER_BLOCK + i].valid = 0;
	}
	erase++;
}

//-----------sw log block swicth-------------

void sw_log_blk_swicth(int PBN_in_SW, int blk_to_erase) {
	// erase blk_to_erase
	erasePBN(blk_to_erase);
	for (int i = 0; i < PAGES_PER_BLOCK; i++) {
		spare[freeblk[0]*PAGES_PER_BLOCK + i].lsn = -1;
	}
	int new_sw_pbn = freeBlockChange(blk_to_erase);
	//update data table
	datatable.entry[SWtbl.lbn].pbn = PBN_in_SW;

	//reset sw table
	SWtbl.pbn = new_sw_pbn;
	SWtbl.lbn = -1;
	SWtbl.sw_sec_num = 0;
}

//-----------sw_log_block merge--------------
void sw_log_blk_merge(int PBN_in_SW, int blk_to_erase, int sw_sec_num) {

	// copy data block value to sw log blk
	for (int i=0; i < PAGES_PER_BLOCK; i++) {
		read++;
		if (spare[blk_to_erase*PAGES_PER_BLOCK + i].lsn != -1 &&
			spare[blk_to_erase*PAGES_PER_BLOCK + i].valid==1) {
			spare[PBN_in_SW*PAGES_PER_BLOCK + i].lsn =
				spare[blk_to_erase*PAGES_PER_BLOCK + i].lsn;
			spare[PBN_in_SW*PAGES_PER_BLOCK + i].valid = 1;
			write++;
		}
		
	}

	//update data table
	datatable.entry[SWtbl.lbn].pbn = PBN_in_SW;

	erasePBN(blk_to_erase);

	for (int i = 0; i < PAGES_PER_BLOCK; i++) {
		spare[freeblk[0] * PAGES_PER_BLOCK + i].lsn = -1;
	}
	int new_sw_pbn= freeBlockChange(blk_to_erase);

	//reset sw table
	SWtbl.pbn = new_sw_pbn;
	SWtbl.lbn = -1;
	SWtbl.sw_sec_num = 0;
	
	
}

//-------------write to sw blk---------------
void write_to_swtbl_derictly(int LBN,int lsn) {
	// write to sw log block derictly
	//----update sw table-------
	SWtbl.lbn = LBN;
	SWtbl.sw_sec_num++;
	//----write data to sw block---
	spare[SWtbl.pbn*PAGES_PER_BLOCK].lsn = lsn;
	spare[SWtbl.pbn*PAGES_PER_BLOCK].valid = 1;
	spare[datatable.entry[LBN].pbn*PAGES_PER_BLOCK].valid = 0;
	write++;
}

void write_to_swtbl(int LBN, int offset) {
	int lsn = LBN * PAGES_PER_BLOCK + offset;
	spare[SWtbl.pbn*PAGES_PER_BLOCK + offset].lsn = lsn;
	spare[SWtbl.pbn*PAGES_PER_BLOCK + offset].valid = 1;
	spare[datatable.entry[LBN].pbn*PAGES_PER_BLOCK + offset].valid = 0;
	write++;
	SWtbl.sw_sec_num++;
}


