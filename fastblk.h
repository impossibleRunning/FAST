#pragma once
#ifndef	_FAST_MAPPING_H_
#define	_FAST_MAPPING_H_

#define TRUE						1
#define	FALSE						0

#define	SECTOR_SIZE					512			
#define	SPARE_SIZE					16			
#define	PAGE_SIZE					(SECTOR_SIZE+SPARE_SIZE)
#define SECTORS_PER_PAGE			1
#define	PAGES_PER_BLOCK				4  // updatable
#define	BLOCK_SIZE					(PAGE_SIZE*PAGES_PER_BLOCK)
#define	BLOCKS_PER_DEVICE			20 // updatable
#define FREEBLOCKS					1	// 한 개의 free block을 유지
#define SW_LOG_BLOCK				1
#define LOG_BLOCK					3	// 4/8/16/32
#define RW_LOG_BLOCK				(LOG_BLOCK - SW_LOG_BLOCK)
#define DATABLKS_PER_DEVICE			(BLOCKS_PER_DEVICE - FREEBLOCKS - LOG_BLOCK)

#define INRWTBL						1
#define NOTINRWTBL					0

typedef int FREEBLK;


//
// If necessary, constants can be added
// 

/*
     ____________________________________
	 | FREE BLOCK | SW BLOCK | RW BLOCK |
	 ------------------------------------

*/



typedef struct
{
	int lsn;									// lsn means what logical page occupies this physical page
	int valid;
	char dummy[SPARE_SIZE - 4];
} SpareData;

typedef struct
{
	//	int lbn;									// not necessary
	int pbn;
	//	int first_free_page_offset;					// not necessary
} BlkMapTblEntry;

typedef struct									// block mapping table
{
	BlkMapTblEntry entry[DATABLKS_PER_DEVICE];
} BlockMapTbl;

typedef struct {
	int lsn;
	int psn;
//	int lsns[PAGES_PER_BLOCK];
	int valid;
}RWMapTblEntry;

typedef	struct {
	RWMapTblEntry entry[RW_LOG_BLOCK*PAGES_PER_BLOCK];
	int rw_tbl_count;
	int rear;	
	int front;
} RWMapTbl;

typedef struct {
	int lbn;
	int pbn;
	int sw_sec_num;
} SWMapTbl;

//
// If necessary, you can add new data types or modify the upper type definitions.
// This header file is used just for reference. 
//

int read_from_sw_tbl(int LBN, int offset);
int read_from_rw_tbl(int LBN, int offset);
int read_from_data_tbl(int LBN, int offset);

void writeToDatablk1(int LBN, int offset);
void writeToDatablk2(int LBN, int offset);
int writeCondition(int LBN, int offset);

int get_PBN_From_DataTbl(int LBN);
int get_lsn_From_Spare(int LBN, int offset);
void get_lbn_from_swtbl(int* lbn, int* secnum);

void sw_log_blk_swicth(int PBN_in_SW, int blk_to_erase);
void write_to_swtbl_derictly(int LBN, int lsn);
void write_to_swtbl(int LBN, int offset);
void sw_log_blk_merge(int PBN_in_SW, int blk_to_erase, int sw_sec_num);

void write_to_rwtbl_derictly(int lsn);
int rwtbl_isFull();
void rw_log_blk_merge(int lsn);


#endif
