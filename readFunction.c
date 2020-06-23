#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "fastblk.h"

extern int read;
extern BlockMapTbl datatable;
extern RWMapTbl RWtbl;
extern SWMapTbl SWtbl;
extern SpareData spare[BLOCKS_PER_DEVICE*PAGES_PER_BLOCK];


int read_from_sw_tbl(int LBN, int offset) {
	read++;
	if (LBN != SWtbl.lbn) return read_from_rw_tbl(LBN, offset);
	else {
		if (offset > SWtbl.sw_sec_num) return read_from_data_tbl(LBN, offset);
		else if (offset <= SWtbl.sw_sec_num) {
			return (SWtbl.pbn*PAGES_PER_BLOCK + offset);
		}
	}
}

int read_from_rw_tbl(int LBN, int offset) {
	int lsn = LBN * PAGES_PER_BLOCK + offset;
	for (int i = 0; i < RW_LOG_BLOCK*PAGES_PER_BLOCK; i++) {
		read++;
		if (RWtbl.entry[i].lsn == lsn&&RWtbl.entry[i].valid==1) {
			return RWtbl.entry[i].psn;
		}
	}
	return read_from_data_tbl(LBN, offset);
}

int read_from_data_tbl(int LBN, int offset) {
	read++;
	if (datatable.entry[LBN].pbn == -1) {
		return -1;
	}
	else if (spare[datatable.entry[LBN].pbn*PAGES_PER_BLOCK + offset].lsn == -1) {
		return -1;
	}
	else {
		return (datatable.entry[LBN].pbn*PAGES_PER_BLOCK + offset);
	}
	
}
