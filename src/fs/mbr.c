#include <fs/mbr.h>

void parse_mbr(bdev_t *dev)
{
	/*mbr_t *buff = (mbr_t*)kmalloc(sizeof(mbr_t));

	ata_read_sects_28_pio((void*)buff, 1, *dev, 0);

	if (buff->signature != 0x55AA)
		kpanic("Invalid bootsector");
	terminal_writestring("parsing mbr\n");

	uint32_t *part;
	int i = 0;
	for (part = /*&buff->partition_table_entry0*//*(uint8_t*)buff + 446; part < &buff->signature; part += 16)
	{
		terminal_writestring("part: ");
		terminal_printhex32(part);
		terminal_writestring("\n");
		if (!(part[4] == 0 || (part[12] == 0 && part[13] == 0 && part[14] == 0 && part[15] == 0)
		    || (part[2] & 0x3F == 0) || part[4] != 0x83))
			break;
	}

	const uint16_t start_lba_hi = *((uint16_t*)buff + 454);
	const uint16_t start_lba_lo = part[10] | (uint16_t)part[11] << 8;
	
	dev->start_lba = ((uint32_t)start_lba_hi << 16) | start_lba_lo;/*(uint16_t)part[8] << 8 | (uint8_t)part[9] | (uint16_t)part[10] << 24 | (uint16_t)part[11] << 16*//*(uint32_t)*(&part[8]);
	dev->part_size = part[12] | part[13] >> 8 | part[14] >> 16 | part[15] >> 24;

	const uint16_t tmp = *(((uint16_t*)buff) + 227);
	
	dev->start_lba = tmp >> 8 | tmp << 8;*/

	const uint32_t real_lba = dev->start_lba;
	const uint16_t base = dev->base;
	outb(base + REG_DEV_SEL_PORT, ((dev->slave_bit) ? 0xF0 : 0xE0) | ((real_lba >> 24) & 0x0F));
	outb(base + REG_INFO_PORT, 0x00);
	outb(base + REG_SECT_CNT_PORT, 1);
	outb(base + REG_LBA_LOW_PORT, (uint8_t)real_lba);
	outb(base + REG_LBA_MID_PORT, (uint8_t)(real_lba >> 8));
	outb(base + REG_LBA_HIGH_PORT, (uint8_t)(real_lba >> 16));

	outb(base + REG_CMD_PORT, READ_SECTS_CMD);

	uint8_t status = inb(dev->base + REG_CMD_PORT);
	while(((status & 0x80) == 0x80)
	      && ((status & 0x01) != 0x01))
	{
	        status = inb(dev->base + REG_CMD_PORT);
		terminal_writestring("status:\n");
		terminal_printhex(status);
	}

	uint16_t tmp;
	for (uint32_t i = 0; i < 256; i++)
	{
		tmp = inw(dev->base);
		if (i == 227)
			dev->start_lba = tmp;
		terminal_printhex16(tmp);
	}
	
	terminal_printhex32(dev->start_lba);
	//kfree(buff);
	terminal_writestring("Finished parsing mbr\n");
}
