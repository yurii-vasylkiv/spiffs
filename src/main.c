#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <spiffs.h>
#include <time.h>

#ifndef NO_TEST
#include "testrunner.h"
#endif

#define PHY_SIZE        0x10000
#define PHY_ADDR              0
#define PHYS_ERASE_BLOCK   4096

#define LOG_BLOCK_SIZE     4096
#define LOG_PAGE_SIZE       256

#define FILE_SIZE_MAX       256
#define CHUNK_SIZE_MAX      128

static uint8_t fs_storage[PHY_SIZE];
static uint8_t fdworkspace[256];
static uint8_t cache[256];

static struct spiffs_t fs;
static spiffs_config config;
static uint8_t workspace[2 * LOG_PAGE_SIZE];


static int32_t hal_read(struct spiffs_t *fs_p, uint32_t addr, uint32_t size, uint8_t *dst_p)
{
    assert(addr + size < sizeof(fs_storage));
    memcpy(dst_p, &fs_storage[addr], size);
    return (0);
}
static int32_t hal_write(struct spiffs_t *fs_p, uint32_t addr, uint32_t size, uint8_t *src_p)
{
    assert(addr + size < sizeof(fs_storage));
    memcpy(&fs_storage[addr], src_p, size);
    return (0);
}
static int32_t hal_erase(struct spiffs_t *fs_p, uint32_t addr, uint32_t size)
{
    assert(addr + size <= sizeof(fs_storage));
    memset(&fs_storage[addr], -1, size);
    return (0);
}

static int init(void)
{
    /* initialize memory */
    memset(&fs_storage[0], -1, sizeof(fs_storage));
    
    /* Initiate the config struct. */
    config.hal_read_f = hal_read;
    config.hal_write_f = hal_write;
    config.hal_erase_f = hal_erase;
    config.phys_size = PHY_SIZE;
    config.phys_addr = PHY_ADDR;
    config.phys_erase_block = PHYS_ERASE_BLOCK;
    config.log_block_size = LOG_BLOCK_SIZE;
    config.log_page_size = LOG_PAGE_SIZE;
    
    /* Mount the file system to initialize the runtime variables. */
    int res = SPIFFS_mount(&fs, &config, workspace, fdworkspace, sizeof(fdworkspace), cache, sizeof(cache), NULL);
    return res;
}
static int format(void)
{
    /* Format and mount the file system again. */
    SPIFFS_format(&fs);
    int res = SPIFFS_mount(&fs, &config, workspace, fdworkspace, sizeof(fdworkspace), cache, sizeof(cache), NULL);
    
    return (0);
}

void test_read(size_t size)
{
    char buf[size];
    spiffs_file fd;
    
    /* Create a file, delete previous if it already exists, and open
       it for reading and writing. */
    fd = SPIFFS_open(&fs, "file.txt", SPIFFS_RDWR, 0);
    assert(fd >= 0);
    
    /* Read it. */
    SPIFFS_read(&fs, fd, buf, size);
    printf("%s\n", buf);
}

void test_write(void *data, size_t data_size)
{
    spiffs_file fd;
    /* Create a file, delete previous if it already exists, and open
       it for reading and writing. */
    fd = SPIFFS_open(&fs, "file.txt", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
    assert(fd >= 0);
    
    /* Write to it. */
    assert(SPIFFS_write(&fs, fd, data, data_size) == data_size);
    
    /* Close it. */
    assert(SPIFFS_close(&fs, fd) == 0);
}


int main(int argc, char **args)
{
    init();
    format();
    test_write("Hello World!", sizeof("Hello World!"));
    test_read(sizeof("Hello World!"));
    
  exit(EXIT_SUCCESS);
}
