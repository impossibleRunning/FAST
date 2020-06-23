#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "fastblk.h"

extern int read;
extern int write;
extern int used_blk;
extern RWMapTbl RWtbl;
extern SWMapTbl SWtbl;
extern BlockMapTbl datatable;
extern SpareData spare[BLOCKS_PER_DEVICE*PAGES_PER_BLOCK];
int checkInRWtbl(int LBN);

int writeCondition(int LBN, int offset) {
	if ((offset == 0) && (checkInRWtbl(LBN) == NOTINRWTBL)) {
		return 1;
	}
	if ((LBN == SWtbl.lbn) && (offset == SWtbl.sw_sec_num)) {
		read++;
		return 2;
	}
	if ((LBN == SWtbl.lbn) && (offset > SWtbl.sw_sec_num)) {
		read++;
		return 3;
	}
	if ((LBN == SWtbl.lbn) && (offset < SWtbl.sw_sec_num)) {
		read++;
		return 4;
	}
	return 5;
}

//---------check whether LBN is in RW write table--------
int checkInRWtbl(int LBN) {
	int i = 0;
	for (i; i < PAGES_PER_BLOCK*RW_LOG_BLOCK; i++) {
//		read++;
		if (RWtbl.entry[i].lsn != -1) {
			int rwlbn = RWtbl.entry[i].lsn / PAGES_PER_BLOCK;
			if (LBN == rwlbn) {
				return INRWTBL;
			}
		}
		
	}
	return NOTINRWTBL;
}

void writeToDatablk1(int LBN, int offset) {
	datatable.entry[LBN].pbn = used_blk;
	used_blk++;
	int lsn = LBN * PAGES_PER_BLOCK + offset;
	spare[datatable.entry[LBN].pbn*PAGES_PER_BLOCK + offset].lsn = lsn;
	spare[datatable.entry[LBN].pbn*PAGES_PER_BLOCK + offset].valid = 1;	//maybe not necessary
	write++;
}

void writeToDatablk2(int LBN, int offset) {
	int lsn = LBN * PAGES_PER_BLOCK + offset;
	spare[datatable.entry[LBN].pbn*PAGES_PER_BLOCK + offset].lsn = lsn;
	spare[datatable.entry[LBN].pbn*PAGES_PER_BLOCK + offset].valid = 1;	//maybe not necessary
	write++;
}

int get_PBN_From_DataTbl(int LBN) {
//	read++;
	return datatable.entry[LBN].pbn;
}

int get_lsn_From_Spare(int LBN,int offset) {
	read++;
	return spare[datatable.entry[LBN].pbn*PAGES_PER_BLOCK + offset].lsn;
}

void get_lbn_from_swtbl(int *lbn,int *secnum) {
//	read++;
	*lbn = SWtbl.lbn;
	*secnum = SWtbl.sw_sec_num;
}
