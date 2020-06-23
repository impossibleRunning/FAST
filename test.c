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
void ftl_write(int lsn);

void test() {

	for (int i = 0; i < PAGES_PER_BLOCK; i++) {
		ftl_write(60+i);
	}
	for (int i = 0; i < PAGES_PER_BLOCK; i++) {
		ftl_write(60 + i);
	}
	
//	ftl_write(60);
//	ftl_write(62);
	ftl_write(60);
}

void test_1() {
	printf("test condition 1\n");
	for (int i = 0; i < PAGES_PER_BLOCK; i++) {
		ftl_write(60 + i);
	}
	ftl_write(60);
}

void test_2() {
	printf("test condition 2\n");
	for (int i = 0; i < PAGES_PER_BLOCK; i++) {
		ftl_write(60 + i);
	}
	ftl_write(60);
	ftl_write(61);
}
void test_3() {
	printf("test condition 3\n");
	for (int i = 0; i < PAGES_PER_BLOCK; i++) {
		ftl_write(60 + i);
	}
	ftl_write(60);
	ftl_write(62);
}
void test_4() {
	printf("test condition 4\n");
	for (int i = 0; i < PAGES_PER_BLOCK; i++) {
		ftl_write(60 + i);
	}
	ftl_write(60);
	ftl_write(61);
	ftl_write(62);
	ftl_write(61);
}

void test_5() {
	printf("test condition 5\n");
	for (int i = 0; i < PAGES_PER_BLOCK*DATABLKS_PER_DEVICE; i++) {
		ftl_write(i);
	}
	for (int i = 0; i < PAGES_PER_BLOCK; i++) {
		ftl_write(60+i);
	}

	ftl_write(9);
	ftl_write(10);
	ftl_write(11);
	ftl_write(21);
	
	ftl_write(1);	
	ftl_write(8);	
	ftl_write(3);
	ftl_write(10);

	ftl_write(3);
	ftl_write(1);
	ftl_write(8);
	ftl_write(9);

	ftl_write(21);

	ftl_write(23);
	ftl_write(1);
}

void test_6() {
	for (int i = 0; i < PAGES_PER_BLOCK; i++) {
		ftl_write(60 + i);
	}
}
