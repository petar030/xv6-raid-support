#include "raid.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "proc.h"
#include "defs.h"
#include "fs.h"
// SPINLOCK 
struct sleeplock raid_lock;
static int raid_lock_inited = 0;

static void raid_lock_init(void) {
  if (!raid_lock_inited) {
    initsleeplock(&raid_lock, "raid_lock");
    raid_lock_inited = 1;
  }
}


// MAIN RAID FUNCTIONS
struct raid_meta{
    enum RAID_TYPE type;
    int exists; //(1)==exists ; (-1)==doesn't exist ; (0)==not clear
};
static struct raid_meta raid_info = {0, 0};
int check_raid() {


    if (raid_info.exists == 1) return 0;
    if (raid_info.exists == -1) return -1;
    
    //If exists == 0 it is not clear if raid exists so we need to check for all raids
    if (check_raid1() == 0) {
        set_raid(RAID1);
        printf("RAID1 FOUND\n");
        return 0;
    }
    if (check_raid0() == 0) {
        set_raid(RAID0);
        printf("RAID0 FOUND\n");
        return 0;
    }
    if (check_raid0_1() == 0) {
        set_raid(RAID0_1);
        printf("RAID0_1 FOUND\n");
        return 0;
    }
    if (check_raid4() == 0) {
        set_raid(RAID4);
        printf("RAID4 FOUND\n");
        return 0;
    }
    if (check_raid5() == 0) {
        set_raid(RAID5);
        printf("RAID5 FOUND\n");
        return 0;
    }
    raid_info.exists = -1;
    printf("RAID NOT FOUND\n");
    return -1;
}
void set_raid(enum RAID_TYPE type) {
    raid_info.type = type;
    raid_info.exists = 1;
}
void reset_raid() {
    raid_info.exists = -1;
}
int init_raid(enum RAID_TYPE raid) {
    raid_lock_init();
    acquiresleep(&raid_lock);
    //printf("raid_lock acquired in init_raid\n");

    int ret = -1;
    if (raid < 0 || raid > 5) {
        ret = -1;
    } else {
        switch (raid) {
        case RAID1:
            ret = init_raid1();
            break;
        case RAID0:
            ret = init_raid0();
            break;
        case RAID0_1:
            ret = init_raid0_1();
            break;
        case RAID4:
            ret = init_raid4();
            break;
        case RAID5:
            ret = init_raid5();
            break;
        default:
            ret = -1;
            break;
        }
    }

    //printf("raid_lock releasing in init_raid\n");
    releasesleep(&raid_lock);
    return ret;
}

enum RAID_TYPE raid_type() {
    // This function can be called only if raid exists
    return raid_info.type;
}



int read_raid(int blkn, uchar *data) {
    raid_lock_init();
    acquiresleep(&raid_lock);
    //printf("raid_lock acquired in read_raid\n");

    if (check_raid() != 0) {
        //printf("raid_lock releasing in read_raid (check_raid failed)\n");
        releasesleep(&raid_lock);
        return -1;
    }

    enum RAID_TYPE raid = raid_type();
    int ret = -1;
    switch (raid) {
    case RAID0:
        ret = read_raid0(blkn, data);
        break;
    case RAID1:
        ret = read_raid1(blkn, data);
        break;
    case RAID0_1:
        ret = read_raid0_1(blkn, data);
        break;
    case RAID4:
        ret = read_raid4(blkn, data);
        break;
    case RAID5:
        ret = read_raid5(blkn, data);
        break;
    default:
        ret = -1;
        break;
    }

    //printf("raid_lock releasing in read_raid\n");
    releasesleep(&raid_lock);
    return ret;
}

int write_raid(int blkn, uchar *data) {   
     raid_lock_init();
    acquiresleep(&raid_lock);
    //printf("raid_lock acquired in write_raid\n");

    if (check_raid() != 0) {
        //printf("raid_lock releasing in write_raid (check_raid failed)\n");
        releasesleep(&raid_lock);
        return -1;
    }

    enum RAID_TYPE raid = raid_type();
    int ret = -1;
    switch (raid) {
    case RAID0:
        ret = write_raid0(blkn, data);
        break;
    case RAID1:
        ret = write_raid1(blkn, data);
        break;
    case RAID0_1:
        ret = write_raid0_1(blkn, data);
        break;
    case RAID4:
        ret = write_raid4(blkn, data);
        break;
    case RAID5:
        ret = write_raid5(blkn, data);
        break;
    default:
        ret = -1;
        break;
    }

    //printf("raid_lock releasing in write_raid\n");
    releasesleep(&raid_lock);
    return ret;
}

int disk_fail_raid(int diskn) {
        raid_lock_init();
    acquiresleep(&raid_lock);
    //printf("raid_lock acquired in disk_fail_raid\n");

    if (check_raid() != 0) {
        //printf("raid_lock releasing in disk_fail_raid (check_raid failed)\n");
        releasesleep(&raid_lock);
        return -1;
    }

    enum RAID_TYPE raid = raid_type();
    int ret = -1;
    switch (raid) {
    case RAID0:
        ret = disk_fail_raid0(diskn);
        break;
    case RAID1:
        ret = disk_fail_raid1(diskn);
        break;
    case RAID0_1:
        ret = disk_fail_raid0_1(diskn);
        break;
    case RAID4:
        ret = disk_fail_raid4(diskn);
        break;
    case RAID5:
        ret = disk_fail_raid5(diskn);
        break;
    default:
        ret = -1;
        break;
    }

    //printf("raid_lock releasing in disk_fail_raid\n");
    releasesleep(&raid_lock);
    return ret;
}

int disk_repaired_raid(int diskn) {
    raid_lock_init();
    acquiresleep(&raid_lock);
    //printf("raid_lock acquired in disk_repaired_raid\n");

    if (check_raid() != 0) {
        //printf("raid_lock releasing in disk_repaired_raid (check_raid failed)\n");
        releasesleep(&raid_lock);
        return -1;
    }

    enum RAID_TYPE raid = raid_type();
    int ret = -1;
    switch (raid) {
    case RAID0:
        ret = disk_repaired_raid0(diskn);
        break;
    case RAID1:
        ret = disk_repaired_raid1(diskn);
        break;
    case RAID0_1:
        ret = disk_repaired_raid0_1(diskn);
        break;
    case RAID4:
        ret = disk_repaired_raid4(diskn);
        break;
    case RAID5:
        ret = disk_repaired_raid5(diskn);
        break;
    default:
        ret = -1;
        break;
    }

    //printf("raid_lock releasing in disk_repaired_raid\n");
    releasesleep(&raid_lock);
    return ret;
}

int info_raid(uint *blkn, uint *blks, uint *diskn) {
    raid_lock_init();
    acquiresleep(&raid_lock);
    //printf("raid_lock acquired in info_raid\n");

    if (check_raid() != 0) {
        //printf("raid_lock releasing in info_raid (check_raid failed)\n");
        releasesleep(&raid_lock);
        return -1;
    }

    enum RAID_TYPE raid = raid_type();
    int ret = -1;
    switch (raid) {
    case RAID0:
        ret = info_raid0(blkn, blks, diskn);
        break;
    case RAID1:
        ret = info_raid1(blkn, blks, diskn);
        break;
    case RAID0_1:
        ret = info_raid0_1(blkn, blks, diskn);
        break;
    case RAID4:
        ret = info_raid4(blkn, blks, diskn);
        break;
    case RAID5:
        ret = info_raid5(blkn, blks, diskn);
        break;
    default:
        ret = -1;
        break;
    }

    //printf("raid_lock releasing in info_raid\n");
    releasesleep(&raid_lock);
    return ret;
}

int destroy_raid() {
    raid_lock_init();
    acquiresleep(&raid_lock);
    //printf("raid_lock acquired in destroy_raid\n");

    if (check_raid() != 0) {
        //printf("raid_lock releasing in destroy_raid (check_raid failed)\n");
        releasesleep(&raid_lock);
        return -1;
    }

    enum RAID_TYPE raid = raid_type();
    int ret = -1;
    switch (raid) {
    case RAID0:
        ret = destroy_raid0();
        break;
    case RAID1:
        ret = destroy_raid1();
        break;
    case RAID0_1:
        ret = destroy_raid0_1();
        break;
    case RAID4:
        ret = destroy_raid4();
        break;
    case RAID5:
        ret = destroy_raid5();
        break;
    default:
        ret = -1;
        break;
    }

    //printf("raid_lock releasing in destroy_raid\n");
    releasesleep(&raid_lock);
    return ret;
}

void serialize(void *data, uchar *buffer, int size)
{
    if (size > BSIZE)
        size = BSIZE;
    uchar *ptr = (uchar *)data;
    for (int i = 0; i < size; i++)
    {
        buffer[i] = ptr[i];
    }
}
void deserialize(uchar *buffer, void *data, int size)
{
    if (size > BSIZE)
        size = BSIZE;
    uchar *ptr = (uchar *)data;
    for (int i = 0; i < size; i++)
    {
        ptr[i] = buffer[i];
    }
}
void clone_disk(int disk_src, int disk_dst) {
    uchar buffer[BSIZE];


    for (int i = 0; i < NUM_OF_BLOCKS; i++) {
        read_block(disk_src, i, buffer);
        write_block(disk_dst, i, buffer);
        printf("KLONIRANJE BLOKA %d\n", i);
    }

}




struct uni_raid_meta
{
    enum RAID_TYPE type;
    uchar status; //1==working ; 0==broken
    uint num_of_blocks;
    uchar raid_set;
};
static struct uni_raid_meta metadata_cache[VIRTIO_RAID_DISK_END];
static uchar cache_loaded = 0;
void load_cache(){
    if(cache_loaded) return;

    uchar buffer[BSIZE];
    int size_of_metadata = sizeof(struct uni_raid_meta);
    for (int i = VIRTIO_RAID_DISK_START; i <= VIRTIO_RAID_DISK_END; i++)
    {

        read_block(i, 0, buffer);
        //deserialize and write in cache
        deserialize(buffer, &metadata_cache[i-1], size_of_metadata);

        
    }


    cache_loaded = 1;
}
uchar rstatus(int diskn) {
    load_cache();
    uchar ret = metadata_cache[diskn - 1].status;
    return ret;
}

void wstatus(int diskn, uchar status) {
    load_cache();
    metadata_cache[diskn - 1].status = status;
    uchar buffer[BSIZE];
    int size_of_metadata = sizeof(struct uni_raid_meta);
    serialize(&metadata_cache[diskn - 1], buffer, size_of_metadata);
    write_block(diskn, 0, buffer);
}

// RAID1 FUNCTIONS 
int init_raid1()
{      
    //If one or less disks exception
    if(VIRTIO_RAID_DISK_END <= 1) return -1;

    struct uni_raid_meta metadata;
    metadata.type = RAID1;
    metadata.status = 1;
    metadata.num_of_blocks = NUM_OF_BLOCKS - 1;
    metadata.raid_set = 1;

    uchar buffer[BSIZE];
    int size_of_metadata = sizeof(struct uni_raid_meta);

    serialize(&metadata, buffer, size_of_metadata);

    for (int i = VIRTIO_RAID_DISK_START; i <= VIRTIO_RAID_DISK_END; i++)
    {

        write_block(i, 0, buffer);
        
    }
    load_cache();
    set_raid(RAID1);    
    return 0;
}
int read_raid1(int blkn, uchar *data)
{
    int raid_blkn = blkn + 1; //Metadata is in block 0
    //blkn validity check
    if(raid_blkn < 1 || raid_blkn > (NUM_OF_BLOCKS-1)) return -1;
    uchar successful = 0; //if there is at least one available disk, successful = 1
    for (int i = VIRTIO_RAID_DISK_START; i <= VIRTIO_RAID_DISK_END; i++)
    {
        
        if(rstatus(i)){
            if(!successful) successful = 1;
            read_block(i, raid_blkn, data);

        }
        
    }
    if(!successful) return -1;
    
    return 0;
    
}
int write_raid1(int blkn, uchar *data)
{
    int raid_blkn = blkn + 1; //Metadata is in block 0
    //blkn validity check
    if(raid_blkn < 1 || raid_blkn > (NUM_OF_BLOCKS-1)) return -1;
    uchar successful = 0; //if there is at least one available disk, successful = 1
    for (int i = VIRTIO_RAID_DISK_START; i <= VIRTIO_RAID_DISK_END; i++)
    {
        
        if(rstatus(i)){
            if(!successful) successful = 1;
            write_block(i, raid_blkn, data);

        }
        
    }
    if(!successful) return -1;
    
    return 0;
    
}
int disk_fail_raid1(int diskn)
{   if(diskn < VIRTIO_RAID_DISK_START || diskn > VIRTIO_RAID_DISK_END) return -1;
    wstatus(diskn, 0);
    for(int i = VIRTIO_RAID_DISK_START;i <= VIRTIO_RAID_DISK_END;i++){
        if(rstatus(i)){
            return 0;
        }
    }
    destroy_raid1();
    return 0;
}
int disk_repaired_raid1(int diskn)
{   
    if(diskn < VIRTIO_RAID_DISK_START || diskn > VIRTIO_RAID_DISK_END) return -1;
    if(rstatus(diskn)) return -1;
    //Go through all disks, if there is one healthy recover from it
    for(int i = VIRTIO_RAID_DISK_START;i <= VIRTIO_RAID_DISK_END;i++){
        if(rstatus(i)){
            clone_disk(i, diskn);
            wstatus(diskn, 1);
            return 0;
        }
    }
    
    return -1;
}
int info_raid1(uint *blkn, uint *blks, uint *diskn) 
{   
    *blkn = NUM_OF_BLOCKS-1;
    *blks = BSIZE;
    *diskn = VIRTIO_RAID_DISK_END;
    return 0;
}
int destroy_raid1()
{   
    reset_raid();
    if(VIRTIO_RAID_DISK_END < 1) return -1;

    struct uni_raid_meta metadata;
    metadata.type = 0;
    metadata.status = 0;
    metadata.num_of_blocks = 0;
    metadata.raid_set = 0;

    uchar buffer[BSIZE];
    int size_of_metadata = sizeof(struct uni_raid_meta);

    serialize(&metadata, buffer, size_of_metadata);

    for (int i = VIRTIO_RAID_DISK_START; i <= VIRTIO_RAID_DISK_END; i++)
    {

        write_block(i, 0, buffer);
        
    }


    

    return 0;
}
int check_raid1(){
    uchar buffer[BSIZE];
    int size_of_metadata = sizeof(struct uni_raid_meta);
    struct uni_raid_meta meta;
    read_block(1, 0, buffer);
    deserialize(buffer, &meta, size_of_metadata);
    if(meta.type == RAID1 && meta.raid_set == 1) return 0;
    return -1;
}
// RAID0 FUNCTIONS
struct raid0_meta
{
    enum RAID_TYPE type;
    uchar status;
    uint num_of_blocks;
    uchar raid_set;
};
static struct raid0_meta metadata_cache0;
static uchar cache_loaded0 = 0;
void load_cache0(){
    if(cache_loaded0) return;

    uchar buffer[BSIZE];
    int size_of_metadata = sizeof(struct raid0_meta);
    read_block(1, 0, buffer);
    //deserialize and write in cache
    deserialize(buffer, &metadata_cache0, size_of_metadata);
    cache_loaded0 = 1;
}
int init_raid0(){
    if(VIRTIO_RAID_DISK_END < 1) return -1;

    struct raid0_meta metadata;
    metadata.type = RAID0;
    metadata.status = 1;
    metadata.num_of_blocks = (NUM_OF_BLOCKS*VIRTIO_RAID_DISK_END) - 1;
    metadata.raid_set = 1;
    uchar buffer[BSIZE];
    int size_of_metadata = sizeof(struct raid0_meta);
    serialize(&metadata, buffer, size_of_metadata);
    write_block(1, 0, buffer);
    load_cache0();
    set_raid(RAID0); 
    return 0;
}
int read_raid0(int blkn, uchar *data){
    load_cache0();
    uint raid_blkn = blkn + 1;
    if(raid_blkn < 1 || raid_blkn > metadata_cache0.num_of_blocks) return -1;
    uint disknum = (raid_blkn % VIRTIO_RAID_DISK_END)+1;
    uint blocknum = raid_blkn / VIRTIO_RAID_DISK_END;
    read_block(disknum, blocknum, data);
    return 0;


}
int write_raid0(int blkn, uchar *data){
    load_cache0();
    uint raid_blkn = blkn + 1;
    if(raid_blkn < 1 || raid_blkn > metadata_cache0.num_of_blocks) return -1;
    uint disknum = (raid_blkn % VIRTIO_RAID_DISK_END)+1;
    uint blocknum = raid_blkn / VIRTIO_RAID_DISK_END;
    write_block(disknum, blocknum, data);
    return 0;
}
int disk_fail_raid0(int diskn){
    if(diskn < VIRTIO_RAID_DISK_START || diskn > VIRTIO_RAID_DISK_END) return -1;
    destroy_raid0(); return 0;
}
int disk_repaired_raid0(int diskn){return -1;}
int info_raid0(uint *blkn, uint *blks, uint *diskn){
    *blkn = (NUM_OF_BLOCKS*VIRTIO_RAID_DISK_END)-1;
    *blks = BSIZE;
    *diskn = VIRTIO_RAID_DISK_END;
    return 0;
}
int destroy_raid0(){
    reset_raid();
    struct raid0_meta metadata;
    metadata.type = 0;
    metadata.status = 0;
    metadata.num_of_blocks = 0;
    metadata.raid_set = 0;
    uchar buffer[BSIZE];
    int size_of_metadata = sizeof(struct raid0_meta);
    serialize(&metadata, buffer, size_of_metadata);
    write_block(1, 0, buffer);
    return 0;
}
int check_raid0(){
    uchar buffer[BSIZE];
    int size_of_metadata = sizeof(struct raid0_meta);
    struct raid0_meta meta;
    read_block(1, 0, buffer);
    deserialize(buffer, &meta, size_of_metadata);
    if(meta.type == RAID0 && meta.raid_set == 1) return 0;
    return -1;
}
// RAID 0_1 FUNCTIONS
uint return_mirror_disk(uint diskn){
    uint split = VIRTIO_RAID_DISK_END>>1;
    if(diskn <= split) return (diskn+split);
    return (diskn-split);
}
int init_raid0_1()
{
    // If one or less disks exception
    if (VIRTIO_RAID_DISK_END < 4 || (VIRTIO_RAID_DISK_END % 2)) return -1;

    struct uni_raid_meta metadata;
    metadata.type = RAID0_1;
    metadata.status = 1;
    metadata.num_of_blocks = ((VIRTIO_RAID_DISK_END>>1) * (NUM_OF_BLOCKS-1));
    metadata.raid_set = 1;

    uchar buffer[BSIZE];
    int size_of_metadata = sizeof(struct uni_raid_meta);

    serialize(&metadata, buffer, size_of_metadata);

    for (int i = VIRTIO_RAID_DISK_START; i <= VIRTIO_RAID_DISK_END; i++)
    {
        write_block(i, 0, buffer);
    }
    load_cache();
    set_raid(RAID0_1);
    return 0;
}
int read_raid0_1(int blkn, uchar *data)
{ 
    load_cache();
    uint usable_disk_num = (VIRTIO_RAID_DISK_END>>1);
    uint num_of_blocks = metadata_cache[0].num_of_blocks;
    if(blkn < 0 || blkn >= num_of_blocks) return -1;
    //FIND FIRST DISK
    uint raid_blkn = blkn + usable_disk_num;
    uint disknum1 = (raid_blkn % usable_disk_num)+1;
    //FIND BLOCK
    uint blocknum = raid_blkn / usable_disk_num;
    if(rstatus(disknum1)){
        read_block(disknum1, blocknum, data);
        return 0;
    }
    //FIND SECOND DISK
    uint disknum2 = return_mirror_disk(disknum1);
    if(rstatus(disknum2)){
        read_block(disknum2, blocknum, data);
        return 0;
    }
    

    return -1;
}
int write_raid0_1(int blkn, uchar *data)
{
    load_cache();
    uint usable_disk_num = (VIRTIO_RAID_DISK_END>>1);
    uint num_of_blocks = metadata_cache[0].num_of_blocks;
    if(blkn < 0 || blkn >= num_of_blocks) return -1;
    //FIND FIRST DISK
    uint raid_blkn = blkn + usable_disk_num;
    uint disknum1 = (raid_blkn % usable_disk_num)+1;
    //FIND SECOND DISK
    uint disknum2 = return_mirror_disk(disknum1);
    //FIND BLOCK
    uint blocknum = raid_blkn / usable_disk_num;
    if(rstatus(disknum1)) write_block(disknum1, blocknum, data);
    if(rstatus(disknum2)) write_block(disknum2, blocknum, data);
    return 0;
}
int disk_fail_raid0_1(int diskn)
{
    if(diskn < VIRTIO_RAID_DISK_START || diskn > VIRTIO_RAID_DISK_END) return -1;
    wstatus(diskn, 0);
    int mirror_disk = return_mirror_disk(diskn);
    if(rstatus(mirror_disk) == 0) destroy_raid0_1();
    return 0;
}
int disk_repaired_raid0_1(int diskn)
{
    if(diskn < VIRTIO_RAID_DISK_START || diskn > VIRTIO_RAID_DISK_END) return -1;
    int mirror_disk = return_mirror_disk(diskn);
    if(rstatus(mirror_disk)){
        clone_disk(mirror_disk, diskn);
        wstatus(diskn, 1);
        return 0;
    }


    return -1;
}
int info_raid0_1(uint *blkn, uint *blks, uint *diskn)
{
    *blkn = ((VIRTIO_RAID_DISK_END>>1) * (NUM_OF_BLOCKS-1));
    *blks = BSIZE;
    *diskn = VIRTIO_RAID_DISK_END;
    return 0;
}
int destroy_raid0_1()
{
    reset_raid();
    if (VIRTIO_RAID_DISK_END < 1) return -1;

    struct uni_raid_meta metadata;
    metadata.type = 0;
    metadata.status = 0;
    metadata.num_of_blocks = 0;
    metadata.raid_set = 0;

    uchar buffer[BSIZE];
    int size_of_metadata = sizeof(struct uni_raid_meta);

    serialize(&metadata, buffer, size_of_metadata);

    for (int i = VIRTIO_RAID_DISK_START; i <= VIRTIO_RAID_DISK_END; i++)
    {
        write_block(i, 0, buffer);
    }

    return 0;
}
int check_raid0_1()
{
    uchar buffer[BSIZE];
    int size_of_metadata = sizeof(struct uni_raid_meta);
    struct uni_raid_meta meta;
    read_block(1, 0, buffer);
    deserialize(buffer, &meta, size_of_metadata);
    if (meta.type == RAID0_1 && meta.raid_set == 1) return 0;
    return -1;
}
//RAID 4 FUNCTIONS
void xor_blocks(uchar* first, uchar* second) {
    for (int i = 0; i < BSIZE; i++) {
        first[i] ^= second[i];
    }
}
int recover_disk_raid4(int diskn){
    if(diskn < VIRTIO_RAID_DISK_START || diskn > VIRTIO_RAID_DISK_END) return -1;
    struct run *page = kalloc();
    uchar *parity = (uchar *)page;
    uchar *tmp = (uchar *)((char *)page + BSIZE);
    if(!parity || !tmp) return -1;
    
    for(int i = 1; i < NUM_OF_BLOCKS; i++){
        for(int x = 0; x < BSIZE; x++) parity[x] = 0;
        for(int j = VIRTIO_RAID_DISK_START; j <= VIRTIO_RAID_DISK_END; j++){
            if(j != diskn){
                read_block(j, i, tmp);
                xor_blocks(parity, tmp);
            }
        }
        write_block(diskn, i, parity);
    }

    kfree(page);
    return 0;
}
int init_raid4()
{
     //If one or less disks exception
    if(VIRTIO_RAID_DISK_END < 3) return -1;

    struct uni_raid_meta metadata;
    metadata.type = RAID4;
    metadata.status = 1;
    metadata.num_of_blocks = (VIRTIO_RAID_DISK_END-1)*(NUM_OF_BLOCKS-1);
    metadata.raid_set = 1;

    uchar buffer[BSIZE];
    int size_of_metadata = sizeof(struct uni_raid_meta);

    serialize(&metadata, buffer, size_of_metadata);

    for (int i = VIRTIO_RAID_DISK_START; i <= VIRTIO_RAID_DISK_END; i++)
    {

        write_block(i, 0, buffer);
        
    }
    load_cache();
    set_raid(RAID4);  

    //init parity disk
    if(recover_disk_raid4(VIRTIO_RAID_DISK_END) != 0) return -1;   
    return 0;
   
}
int read_raid4(int blkn, uchar *data)
{
    load_cache();
    uint usable_disk_num = (VIRTIO_RAID_DISK_END-1);
    uint num_of_blocks = metadata_cache[0].num_of_blocks;
    if(blkn < 0 || blkn >= num_of_blocks) return -1;
    //FIND DISK AND BLOCK
    uint raid_blkn = blkn + usable_disk_num;
    uint disknum = (raid_blkn % usable_disk_num)+1;
    if(!rstatus(disknum)) return -1;
    uint blocknum = raid_blkn / usable_disk_num;
    read_block(disknum, blocknum, data);

    

    return 0;
}
int write_raid4(int blkn, uchar *data)
{
    load_cache();
    uint usable_disk_num = (VIRTIO_RAID_DISK_END-1);
    uint num_of_blocks = metadata_cache[0].num_of_blocks;
    if(blkn < 0 || blkn >= num_of_blocks) return -1;
    //FIND DISK AND BLOCK
    uint raid_blkn = blkn + usable_disk_num;
    uint disknum = (raid_blkn % usable_disk_num)+1;
    if(!rstatus(disknum) || !rstatus(VIRTIO_RAID_DISK_END)) return -1;
    uint blocknum = raid_blkn / usable_disk_num;
    //UPDATING PARITY
    struct run *page = kalloc();
    uchar *old_val = (uchar *)page;
    if(!old_val) return -1;
    uchar *new_par = (uchar *)((char *)page + BSIZE);
    read_block(disknum, blocknum, old_val);
    read_block(VIRTIO_RAID_DISK_END, blocknum, new_par); //old_parity
    xor_blocks(new_par, old_val);//old_parity xor old_data
    xor_blocks(new_par, data); //old_parity xor new_data
    write_block(VIRTIO_RAID_DISK_END, blocknum, new_par); //new_parity = old_parity xor old_data xor new_data
    //WRITING NEW VAL IN BLOCK
    write_block(disknum, blocknum, data);

    kfree(page);
    return 0;
}
int disk_fail_raid4(int diskn)
{
    if(diskn < VIRTIO_RAID_DISK_START || diskn > VIRTIO_RAID_DISK_END) return -1;
    //if any other disk has failed already destroy raid
    for(int i = VIRTIO_RAID_DISK_START;i <= VIRTIO_RAID_DISK_END;i++){
        if(i != diskn && !rstatus(i)){
            destroy_raid4();
            return 0;
        }
    }
    wstatus(diskn, 0);
    return 0;
}
int disk_repaired_raid4(int diskn)
{
    if(diskn < VIRTIO_RAID_DISK_START || diskn > VIRTIO_RAID_DISK_END || rstatus(diskn)) return -1;
    wstatus(diskn, 1);
    recover_disk_raid4(diskn);
    return 0;
}
int info_raid4(uint *blkn, uint *blks, uint *diskn)
{
    *blkn = (VIRTIO_RAID_DISK_END-1)*(NUM_OF_BLOCKS-1);
    *blks = BSIZE;
    *diskn = VIRTIO_RAID_DISK_END;
    return 0;
}
int destroy_raid4()
{
    reset_raid();
    if (VIRTIO_RAID_DISK_END < 1) return -1;

    struct uni_raid_meta metadata;
    metadata.type = 0;
    metadata.status = 0;
    metadata.num_of_blocks = 0;
    metadata.raid_set = 0;

    uchar buffer[BSIZE];
    int size_of_metadata = sizeof(struct uni_raid_meta);

    serialize(&metadata, buffer, size_of_metadata);

    for (int i = VIRTIO_RAID_DISK_START; i <= VIRTIO_RAID_DISK_END; i++)
    {
        write_block(i, 0, buffer);
    }

    return 0;
}
int check_raid4()
{
    uchar buffer[BSIZE];
    int size_of_metadata = sizeof(struct uni_raid_meta);
    struct uni_raid_meta meta;
    read_block(1, 0, buffer);
    deserialize(buffer, &meta, size_of_metadata);
    if (meta.type == RAID4 && meta.raid_set == 1) return 0;
    return -1;
}
//RAID 5 FUNCTIONS
int init_raid5() {
    //If one or less disks exception
    if (VIRTIO_RAID_DISK_END < 3) return -1;

    struct uni_raid_meta metadata;
    metadata.type = RAID5;
    metadata.status = 1;
    metadata.num_of_blocks = (VIRTIO_RAID_DISK_END - 1) * (NUM_OF_BLOCKS - 1);
    metadata.raid_set = 1;

    uchar buffer[BSIZE];
    int size_of_metadata = sizeof(struct uni_raid_meta);

    serialize(&metadata, buffer, size_of_metadata);

    for (int i = VIRTIO_RAID_DISK_START; i <= VIRTIO_RAID_DISK_END; i++) {
        write_block(i, 0, buffer);
    }
    load_cache();
    set_raid(RAID5);  

    //init parity blocks
    int parity_disk_num = VIRTIO_RAID_DISK_START;
    struct run *page = kalloc();
    uchar *parity = (uchar *)page;
    uchar *tmp = (uchar *)((char *)page + BSIZE);
    if(!parity || !tmp) return -1;
    for(int i = 1;i < NUM_OF_BLOCKS;i++){
        for(int x = 0; x < BSIZE; x++) parity[x] = 0;
        for(int j = VIRTIO_RAID_DISK_START; j <= VIRTIO_RAID_DISK_END; j++){
            if(j != parity_disk_num){
                read_block(j, i, tmp);
                xor_blocks(parity, tmp);
            }
        }
        write_block(parity_disk_num, i, parity);

        parity_disk_num++;
        if(parity_disk_num > VIRTIO_RAID_DISK_END) parity_disk_num = VIRTIO_RAID_DISK_START;
    }

    return 0;
}
int read_raid5(int blkn, uchar *data) {
    load_cache();
    uint usable_disk_num = (VIRTIO_RAID_DISK_END-1);
    uint num_of_blocks = metadata_cache[0].num_of_blocks;
    if(blkn < 0 || blkn >= num_of_blocks) return -1;
    uint raid_blkn = blkn + usable_disk_num;
    uint disknum = (raid_blkn % usable_disk_num)+1;
    uint blocknum = raid_blkn / usable_disk_num;
    int parity_disknum = ((blocknum-1) % VIRTIO_RAID_DISK_END) + 1;
    if(parity_disknum <= disknum) disknum++;
    if(!rstatus(disknum)) return -1;
    read_block(disknum, blocknum, data);

    return 0;
}
int write_raid5(int blkn, uchar *data) {
    load_cache();
    uint usable_disk_num = (VIRTIO_RAID_DISK_END-1);
    uint num_of_blocks = metadata_cache[0].num_of_blocks;
    if(blkn < 0 || blkn >= num_of_blocks) return -1;
     
    uint raid_blkn = blkn + usable_disk_num;
    uint disknum = (raid_blkn % usable_disk_num)+1;
    uint blocknum = raid_blkn / usable_disk_num;
    int parity_disknum = ((blocknum-1) % VIRTIO_RAID_DISK_END) + 1;
    if(parity_disknum <= disknum) disknum++;
    if(!rstatus(disknum) || !rstatus(parity_disknum)) return -1;

    //UPDATING PARITY
    struct run *page = kalloc();
    uchar *old_val = (uchar *)page;
    if(!old_val) return -1;
    uchar *new_par = (uchar *)((char *)page + BSIZE);
    read_block(disknum, blocknum, old_val);
    read_block(parity_disknum, blocknum, new_par); //old_parity
    xor_blocks(new_par, old_val);//old_parity xor old_data
    xor_blocks(new_par, data); //old_parity xor new_data
    write_block(parity_disknum, blocknum, new_par); //new_parity = old_parity xor old_data xor new_data
    //WRITING NEW VAL IN BLOCK
    write_block(disknum, blocknum, data);
    //printf("blocknum: %d, parity_disk: %d, data_disk: %d\n", blocknum, parity_disknum, disknum);
    return 0;
}
int disk_fail_raid5(int diskn) {
    if (diskn < VIRTIO_RAID_DISK_START || diskn > VIRTIO_RAID_DISK_END) return -1;
    //if any other disk has failed already destroy raid
    for (int i = VIRTIO_RAID_DISK_START; i <= VIRTIO_RAID_DISK_END; i++) {
        if (i != diskn && !rstatus(i)) {
            destroy_raid5();
            return 0;
        }
    }
    wstatus(diskn, 0);
    return 0;
}
int disk_repaired_raid5(int diskn) {
    if (diskn < VIRTIO_RAID_DISK_START || diskn > VIRTIO_RAID_DISK_END || rstatus(diskn)) return -1;
    wstatus(diskn, 1);
    //The disk recovers the same way raid4 disk recovers
    recover_disk_raid4(diskn);
    return 0;
}
int info_raid5(uint *blkn, uint *blks, uint *diskn) {
    *blkn = (VIRTIO_RAID_DISK_END - 1) * (NUM_OF_BLOCKS - 1);
    *blks = BSIZE;
    *diskn = VIRTIO_RAID_DISK_END;
    return 0;
}
int destroy_raid5() {
    reset_raid();
    if (VIRTIO_RAID_DISK_END < 1) return -1;

    struct uni_raid_meta metadata;
    metadata.type = 0;
    metadata.status = 0;
    metadata.num_of_blocks = 0;
    metadata.raid_set = 0;

    uchar buffer[BSIZE];
    int size_of_metadata = sizeof(struct uni_raid_meta);

    serialize(&metadata, buffer, size_of_metadata);

    for (int i = VIRTIO_RAID_DISK_START; i <= VIRTIO_RAID_DISK_END; i++) {
        write_block(i, 0, buffer);
    }
    return 0;
}
int check_raid5() {
    uchar buffer[BSIZE];
    int size_of_metadata = sizeof(struct uni_raid_meta);
    struct uni_raid_meta meta;
    read_block(1, 0, buffer);
    deserialize(buffer, &meta, size_of_metadata);
    if (meta.type == RAID5 && meta.raid_set == 1) return 0;
    return -1;
}
