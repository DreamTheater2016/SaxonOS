#ifndef __FS_ATA_H
#define __FS_ATA_H

#define      ATA_MASTER      0x00
#define      ATA_SLAVE       0x01

// ATA-Commands:
#define      ATA_CMD_READ_PIO          0x20
#define      ATA_CMD_READ_PIO_EXT      0x24
#define      ATA_CMD_READ_DMA          0xC8
#define      ATA_CMD_READ_DMA_EXT      0x25
#define      ATA_CMD_WRITE_PIO         0x30
#define      ATA_CMD_WRITE_PIO_EXT     0x34
#define      ATA_CMD_WRITE_DMA         0xCA
#define      ATA_CMD_WRITE_DMA_EXT     0x35
#define      ATA_CMD_CACHE_FLUSH       0xE7
#define      ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define      ATA_CMD_PACKET            0xA0
#define      ATA_CMD_IDENTIFY_PACKET   0xA1
#define      ATA_CMD_IDENTIFY          0xEC

#define      ATAPI_CMD_READ       0xA8
#define      ATAPI_CMD_EJECT      0x1B

#endif
