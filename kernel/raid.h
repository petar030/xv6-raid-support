#include "types.h"
//MAIN RAID FUNCTIONS
enum RAID_TYPE {RAID0, RAID1, RAID0_1, RAID4, RAID5};
int check_raid();
void set_raid(enum RAID_TYPE type);
void reset_raid();
int init_raid(enum RAID_TYPE raid);
int read_raid(int blkn, uchar* data);
int write_raid(int blkn, uchar* data);
int disk_fail_raid(int diskn);
int disk_repaired_raid(int diskn);
int info_raid(uint *blkn, uint *blks, uint *diskn);
int destroy_raid();
void clone_disk(int disk_src, int disk_dst);
void load_cache();
uchar rstatus(int diksn);
void wstatus(int diskn, uchar status);
//RAID1 FUNCTIONS
int init_raid1();
int read_raid1(int blkn, unsigned char *data);
int write_raid1(int blkn, unsigned char *data);
int disk_fail_raid1(int diskn);
int disk_repaired_raid1(int diskn);
int info_raid1(uint *blkn, uint *blks, uint *diskn);
int destroy_raid1();
int check_raid1();
//RAID0 FUNCTIONS  
void load_cache0();
int init_raid0();
int read_raid0(int blkn, uchar *data);
int write_raid0(int blkn, uchar *data);
int disk_fail_raid0(int diskn);
int disk_repaired_raid0(int diskn);
int info_raid0(uint *blkn, uint *blks, uint *diskn);
int destroy_raid0();
int check_raid0();
//RAID0_1 FUNCTIONS
uint return_mirror_disk(uint diskn);
int init_raid0_1();
int read_raid0_1(int blkn, uchar *data);
int write_raid0_1(int blkn, uchar *data);
int disk_fail_raid0_1(int diskn);
int disk_repaired_raid0_1(int diskn);
int info_raid0_1(uint *blkn, uint *blks, uint *diskn);
int destroy_raid0_1();
int check_raid0_1();
//RAID4 FUNCTIONS 
void xor_blocks(uchar* first, uchar* second);
int recover_disk_raid4(int diskn);
int init_raid4();
int read_raid4(int blkn, uchar *data);
int write_raid4(int blkn, uchar *data);
int disk_fail_raid4(int diskn);
int disk_repaired_raid4(int diskn);
int info_raid4(uint *blkn, uint *blks, uint *diskn);
int destroy_raid4();
int check_raid4();
//RAID 5 FUNCTIONS
int init_raid5(void);
int read_raid5(int blkn, uchar *data);
int write_raid5(int blkn, uchar *data);
int disk_fail_raid5(int diskn);
int disk_repaired_raid5(int diskn);
int info_raid5(uint *blkn, uint *blks, uint *diskn);
int destroy_raid5(void);
int check_raid5(void);
