#include <common.h>
#include <command.h>
#include <environment.h>
#include <malloc.h>
#include <asm/byteorder.h>
#include <config.h>
#include <asm/arch/io.h>
#include <partition_table.h>
#include <libavb.h>
#include <version.h>
#include <spotify/avb.h>

#ifdef CONFIG_BOOTLOADER_CONTROL_BLOCK
extern int store_read_ops(
    unsigned char *partition_name,
    unsigned char * buf, uint64_t off, uint64_t size);
extern int store_write_ops(
    unsigned char *partition_name,
    unsigned char * buf, uint64_t off, uint64_t size);

bool boot_info_validate(AvbABData* info)
{
    if (memcmp(info->magic, AVB_AB_MAGIC, AVB_AB_MAGIC_LEN) != 0) {
        printf("Magic %s is incorrect.\n", info->magic);
        return false;
    }
    if (info->version_major > AVB_AB_MAJOR_VERSION) {
        printf("No support for given major version.\n");
        return false;
    }
    return true;
}

void boot_info_reset(AvbABData* info)
{
    memset(info, '\0', sizeof(AvbABData));
    memcpy(info->magic, AVB_AB_MAGIC, AVB_AB_MAGIC_LEN);
    info->version_major = AVB_AB_MAJOR_VERSION;
    info->version_minor = AVB_AB_MINOR_VERSION;
    info->slots[0].priority = AVB_AB_MAX_PRIORITY;
    info->slots[0].tries_remaining = AVB_AB_MAX_TRIES_REMAINING;
    info->slots[0].successful_boot = 0;
    info->slots[1].priority = AVB_AB_MAX_PRIORITY - 1;
    info->slots[1].tries_remaining = AVB_AB_MAX_TRIES_REMAINING;
    info->slots[1].successful_boot = 0;
}

void dump_boot_info(AvbABData* info)
{
    printf("info->magic = %s\n", info->magic);
    printf("info->version_major = %d\n", info->version_major);
    printf("info->version_minor = %d\n", info->version_minor);
    printf("info->slots[0].priority = %d\n", info->slots[0].priority);
    printf("info->slots[0].tries_remaining = %d\n", info->slots[0].tries_remaining);
    printf("info->slots[0].successful_boot = %d\n", info->slots[0].successful_boot);
    printf("info->slots[1].priority = %d\n", info->slots[1].priority);
    printf("info->slots[1].tries_remaining = %d\n", info->slots[1].tries_remaining);
    printf("info->slots[1].successful_boot = %d\n", info->slots[1].successful_boot);

    printf("info->crc32 = %d\n", info->crc32);
}

int get_active_slot(AvbABData* info) {
    if (info->slots[0].priority > info->slots[1].priority)
        return 0;
    else
        return 1;
}

int boot_info_set_active_slot(AvbABData* info, int slot)
{
    unsigned int other_slot_number;

    /* Make requested slot top priority, unsuccessful, and with max tries. */
    info->slots[slot].priority = AVB_AB_MAX_PRIORITY;
    info->slots[slot].tries_remaining = AVB_AB_MAX_TRIES_REMAINING;
    info->slots[slot].successful_boot = 0;

    /* Ensure other slot doesn't have as high a priority. */
    other_slot_number = 1 - slot;
    if (info->slots[other_slot_number].priority == AVB_AB_MAX_PRIORITY) {
        info->slots[other_slot_number].priority = AVB_AB_MAX_PRIORITY - 1;
    }

    dump_boot_info(info);

    return 0;
}

int boot_info_failover(AvbABData* info) {
    int new_slot;

    new_slot = 1 - get_active_slot(info);

    printf("Failing over to slot %d...\n", new_slot);

    boot_info_set_active_slot(info, new_slot);

    return 0;
}

int boot_info_open_partition(char *miscbuf)
{
    char *partition = "misc";
    //int i;
    printf("Start read %s partition datas!\n", partition);
    if (store_read_ops((unsigned char *)partition,
        (unsigned char *)miscbuf, 0, MISCBUF_SIZE) < 0) {
        printf("failed to store read %s.\n", partition);
        return -1;
    }

    /*for (i = AB_METADATA_MISC_PARTITION_OFFSET;i < (AB_METADATA_MISC_PARTITION_OFFSET+AVB_AB_DATA_SIZE);i++)
        printf("buf: %c \n", miscbuf[i]);*/
    return 0;
}

bool boot_info_load(AvbABData *out_info, char *miscbuf)
{
    memcpy(out_info, miscbuf+AB_METADATA_MISC_PARTITION_OFFSET, AVB_AB_DATA_SIZE);
    dump_boot_info(out_info);
    return true;
}

bool boot_info_save(AvbABData *info, char *miscbuf)
{
    char *partition = "misc";
    printf("save boot-info \n");
    info->crc32 = avb_htobe32(
      avb_crc32((const uint8_t*)info, sizeof(AvbABData) - sizeof(uint32_t)));

    memcpy(miscbuf+AB_METADATA_MISC_PARTITION_OFFSET, info, AVB_AB_DATA_SIZE);
    dump_boot_info(info);
    store_write_ops((unsigned char *)partition, (unsigned char *)miscbuf, 0, MISCBUF_SIZE);
    return true;
}

#endif /* CONFIG_BOOTLOADER_CONTROL_BLOCK */