#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "fastblk.h"

int lbn;
int used_blk = 0;
int erase = 0;
int write = 0;
int read = 0;
int repeat = 0;
int copy = 0;
int merge_target_blk=0;
FREEBLK freeblk[FREEBLOCKS];
SpareData spare[BLOCKS_PER_DEVICE*PAGES_PER_BLOCK];
BlockMapTbl datatable;
RWMapTbl RWtbl;
SWMapTbl SWtbl;


//-------function-------

void ftl_open();
int ftl_read(int lsn); 
void ftl_write(int lsn);
void printTable();

//----------------------

void test();

//-----------------------

double calculateTime(int read, int write, int erase);

void random() {
	printf("FAST with %d log block\n", LOG_BLOCK);
	ftl_open();
	FILE *fpWrite = fopen("E:\\FAST\\random.txt", "r");
	int i,a;
	if (fpWrite == NULL)
	{
		printf("error\n");
		//		return -1;
	}
	for (i = 0; i < 1000; i++) //69576
	{
		fscanf(fpWrite, "%d", &a);
		ftl_write(a);
	}

	fclose(fpWrite);
	printf("-----linux_blk-----\n");
	printf("erase: %d\n", erase);
	printf("read: %d\n", read);
	printf("write: %d\n", write);
	printf("time: %f\n", calculateTime(read, write, erase));
	
}

double calculateTime(int read, int write, int erase) {
	double total = read * 0.02 + write * 0.2 + erase * 2 ;
	return total;
}

void linux() {
	printf("FAST with %d log block\n", LOG_BLOCK);
	ftl_open();
	FILE *fpWrite = fopen("E:\\FAST\\Datasets\\linux.trace", "r");
	int i, a;
	if (fpWrite == NULL)
	{
		printf("error\n");
		//		return -1;
	}
	for (i = 0; i < 18900; i++) //69576
	{
		fscanf(fpWrite, "%d", &a);
		ftl_write(a);
	}

	fclose(fpWrite);
	printf("-----linux_fast-----\n");
	printf("erase: %d\n", erase);
	printf("read: %d\n", read);
	printf("write: %d\n", write);
	printf("time: %f\n", calculateTime(read, write, erase));
}

void digicam_new() {
	printf("FAST with %d log block\n", LOG_BLOCK);
	ftl_open();
	FILE *fpWrite = fopen("E:\\FAST\\Datasets\\digicam_new.trace", "r");
	int i, a;
	if (fpWrite == NULL)
	{
		printf("error\n");
		//		return -1;
	}
	for (i = 0; i < 69576; i++) //69576
	{
		fscanf(fpWrite, "%d", &a);
		ftl_write(a);
	}

	fclose(fpWrite);
	printf("-----linux_DIGICAM-----\n");
	printf("erase: %d\n", erase);
	printf("read: %d\n", read);
	printf("write: %d\n", write);
	printf("time: %f\n", calculateTime(read, write, erase));
}

void nikon() {
	printf("FAST with %d log block\n", LOG_BLOCK);
	ftl_open();
	FILE *fpWrite = fopen("E:\\FAST\\Datasets\\NIKON-SS32.txt", "r");
	int i, a;
	if (fpWrite == NULL)
	{
		printf("error\n");
		//		return -1;
	}
	for (i = 0; i < 4618; i++) //69576
	{
		fscanf(fpWrite, "%d", &a);
		ftl_write(a);
	}

	fclose(fpWrite);
	printf("-----NIKON-SS32-----\n");
	printf("erase: %d\n", erase);
	printf("read: %d\n", read);
	printf("write: %d\n", write);
	printf("time: %f\n", calculateTime(read, write, erase));
}

void wpcb() {
	printf("FAST with %d log block\n", LOG_BLOCK);
	ftl_open();
	FILE* fpWrite = fopen("C:\\Users\\DB-LAB\\source\\repos\\dataset\\wpcb_out\\wpcb_out_sequ.txt", "r");
	int i, a;
	if (fpWrite == NULL)
	{
		printf("error\n");
		//		return -1;
	}
	fscanf(fpWrite, "%d", &a);
	while (a != -1) {
		ftl_write(a);
		fscanf(fpWrite, "%d", &a);
	}

	fclose(fpWrite);
	printf("----wpcb_sequ-----\n");
	printf("erase: %d\n", erase);
	printf("read: %d\n", read);
	printf("write: %d\n", write);
	printf("time: %f\n", calculateTime(read, write, erase));
}

void mpcb() {
	printf("FAST with %d log block\n", LOG_BLOCK);
	ftl_open();
	FILE* fpWrite = fopen("C:\\Users\\DB-LAB\\source\\repos\\dataset\\mpcb_out\\mpcb_out_sequ.txt", "r");
	int i, a;
	if (fpWrite == NULL)
	{
		printf("error\n");
		//		return -1;
	}
	fscanf(fpWrite, "%d", &a);
	while (a != -1) {
		ftl_write(a);
		fscanf(fpWrite, "%d", &a);
	}
	

	fclose(fpWrite);
	printf("----mpcb_sequ-----\n");
	printf("erase: %d\n", erase);
	printf("read: %d\n", read);
	printf("write: %d\n", write);
	printf("time: %f\n", calculateTime(read, write, erase));
}


int main() {
//	random();
	wpcb();
	mpcb();
//	printTable();
	system("pause");
}
