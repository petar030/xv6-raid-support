#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#define CHILD_PROCESSES 4  // broj paralelnih procesa

// Makroi za test ishod
#define ASSERT(msg, cond) \
    do { \
        if (cond) printf("[PASS] %s\n", msg); \
        else printf("[FAIL] %s\n", msg); \
    } while (0)

// Test funkcija za dati RAID tip
void test_raid(enum RAID_TYPE type) {
    printf("\n=== Testiranje RAID tipa %d ===\n", type);

    int result = init_raid(type);
    ASSERT("Inicijalizacija RAID-a", result == 0);

    unsigned int blkn = 0, blks = 0, diskn = 0;
    result = info_raid(&blkn, &blks, &diskn);
    ASSERT("Dobavljanje informacija o RAID-u", result == 0);
    printf("  Broj blokova: %d, Veličina bloka: %d, Broj diskova: %d\n", blkn, blks, diskn);

    // Alokacija memorije za podatke
    uchar* data_write = malloc(blks);
    uchar* data_read = malloc(blks);
    if (!data_write || !data_read) {
        printf("[ERROR] Alokacija memorije neuspešna\n");
        return;
    }

    // Popunjavanje test podataka
    for (unsigned int i = 0; i < blks; ++i)
        data_write[i] = (uchar)(i % 256);

    // Pisanje u blok 0
    result = write_raid(0, data_write);
    ASSERT("Pisanje u blok 0", result == 0);

    // Čitanje iz bloka 0
    memset(data_read, 0, blks);
    result = read_raid(0, data_read);
    ASSERT("Čitanje iz bloka 0", result == 0);

    // Poređenje napisanih i pročitanih podataka
    ASSERT("Verifikacija pročitanih podataka", memcmp(data_write, data_read, blks) == 0);

    // Simulacija otkaza diska 1 (numeracija od 1)
    result = disk_fail_raid(1);
    ASSERT("Simulacija otkaza diska 1", result == 0);

    // Ponovno čitanje bloka 0 nakon otkaza
    memset(data_read, 0, blks);
    result = read_raid(0, data_read);
    ASSERT("Čitanje nakon otkaza diska", result == 0);
  

    // Popravka diska 1
    result = disk_repaired_raid(1);
    ASSERT("Popravka diska 1", result == 0);

    // Čitanje nakon popravke
    memset(data_read, 0, blks);
    result = read_raid(0, data_read);
    ASSERT("Čitanje nakon popravke diska", result == 0);
    ASSERT("Verifikacija podataka nakon popravke", memcmp(data_write, data_read, blks) == 0);

    // Uništavanje RAID-a
    result = destroy_raid();
    ASSERT("Uništavanje RAID sistema", result == 0);

    free(data_write);
    free(data_read);
}



void concurrent_test(enum RAID_TYPE raid_type) {
    if (init_raid(raid_type) != 0) {
        printf("init_raid nije uspeo\n");
        return;
    }

    unsigned int blkn = 0, blksz = 0, diskn = 0;
    if (info_raid(&blkn, &blksz, &diskn) != 0) {
        printf("info_raid nije uspeo\n");
        destroy_raid();
        return;
    }

    if (blkn < CHILD_PROCESSES) {
        printf("Premalo blokova (%u) za %d procesa\n", blkn, CHILD_PROCESSES);
        destroy_raid();
        return;
    }

    for (int i = 0; i < CHILD_PROCESSES; i++) {
        int pid = fork();
        if (pid == 0) {
            int block_id = i;
            uchar *wbuf = malloc(blksz);
            uchar *rbuf = malloc(blksz);

            if (wbuf == 0 || rbuf == 0) {
                printf("Child %d: greska pri alokaciji\n", i);
                exit(1);
            }

            for (unsigned int j = 0; j < blksz; j++)
                wbuf[j] = (uchar)(i + 1);

            if (write_raid(block_id, wbuf) != 0) {
                printf("Child %d: pisanje neuspesno\n", i);
                exit(1);
            }

            for (unsigned int j = 0; j < blksz; j++)
                rbuf[j] = 0;

            if (read_raid(block_id, rbuf) != 0) {
                printf("Child %d: citanje neuspesno\n", i);
                exit(1);
            }

            int ok = 1;
            for (unsigned int j = 0; j < blksz; j++) {
                if (rbuf[j] != (uchar)(i + 1)) {
                    ok = 0;
                    break;
                }
            }

            if (!ok) {
                printf("Child %d: podaci ne odgovaraju\n", i);
                exit(1);
            }

            printf("Child %d: OK\n", i);
            free(wbuf);
            free(rbuf);
            exit(0);
        } else if (pid < 0) {
            printf("fork neuspesan\n");
            destroy_raid();
            return;
        }
    }

    for (int i = 0; i < CHILD_PROCESSES; i++) {
        int status;
        wait(&status);
        if (status != 0)
            printf("Proces %d zavrsio sa greskom (status=%d)\n", i, status);
    }

    destroy_raid();
}

int main(int argc, char *argv[]) {
    concurrent_test(RAID0);


    exit(0);
}

