#include <drivers/ata.h>
//#include <fs/ext2.h>

void ata_soft_reset(uint16_t cntrl)
{
	uint16_t old_value = inw(REG_DEV_CTL_PORT);
	outw(REG_DEV_CTL_PORT, old_value | SOFT_RESET);
	outw(REG_DEV_CTL_PORT, old_value);
}

uint8_t detect_device_type(uint8_t slave_bit, bdev_t *dev)
{
	ata_soft_reset(dev->dev_ctl);
	outb(dev->base + REG_DEV_SEL_PORT, MASTER_DRIVE | (slave_bit << 4));
	inb(dev->dev_ctl);
	inb(dev->dev_ctl);
	inb(dev->dev_ctl);
	inb(dev->dev_ctl);
        uint8_t cl = inb(dev->base + REG_LBA_MID_PORT);
	uint8_t ch = inb(dev->base + REG_LBA_HIGH_PORT);

	if (cl==0x14 && ch==0xEB)
		return ATADEV_PATAPI;
	if (cl==0x69 && ch==0x96)
		return ATADEV_SATAPI;
	if (cl==0 && ch == 0)
		return ATADEV_PATA;
	if (cl==0x3c && ch==0xc3)
		return ATADEV_SATA;

	return ATADEV_UNKNOWN;
}

void ata_write_28_pio(void *src, uint8_t sect_count, bdev_t dev, uint32_t rel_lba)
{
	terminal_writestring("Writing to drive");
	const uint32_t real_lba = rel_lba + dev.start_lba;
	const uint16_t base = dev.base;
	outb(base + REG_DEV_SEL_PORT, ((dev.slave_bit) ? 0xF0 : 0xE0) | ((real_lba >> 24) & 0x0F));
	outb(base + REG_INFO_PORT, 0x00);
	outb(base + REG_SECT_CNT_PORT, sect_count);
	outb(base + REG_LBA_LOW_PORT, (uint8_t)real_lba);
	outb(base + REG_LBA_MID_PORT, (uint8_t)(real_lba >> 8));
	outb(base + REG_LBA_HIGH_PORT, (uint8_t)(real_lba >> 16));
	outb(base + REG_CMD_PORT, WRITE_SECTS_CMD);
	uint8_t status;

	uint16_t *ptr = (uint16_t*)src;
	for (uint16_t i = 0; i < ((sect_count == 0) ? 256 : sect_count) * 256; i++)
	{
		/*if (i % 256 == 0)
		{
			inb(REG_DEV_CTL_PORT);
			inb(REG_DEV_CTL_PORT);
			inb(REG_DEV_CTL_PORT);
			terminal_printhex(inb(REG_DEV_CTL_PORT));
			while (1)
			{	
				status = inb(base + REG_CMD_PORT);
				if ((!(status & STATUS_BSY) && status & STATUS_DRQ) || status & STATUS_ERR || status & STATUS_DF)
					break;
			}
        
			if (status & 0x01)
			{
				kpanic("ERROR");
				return;
			}	
		}*/
		outw(base, ptr[i]);
	}
	ata_flush_cache(&dev);
}

void ata_read_sect_lba28_pio(uint16_t *dest, bdev_t dev, uint32_t lba)
{
	outb(0x1F0 + REG_INFO_PORT, 0x00);
	outb(0x1F0 + REG_SECT_CNT_PORT, 0x01);
	outb(0x1F0 + REG_LBA_LOW_PORT, (uint8_t)lba);
	outb(0x1F0 + REG_LBA_MID_PORT, (uint8_t)(lba >> 8));
	outb(0x1F0 + REG_LBA_HIGH_PORT, (uint8_t)(lba >> 16));
        outb(0x1F0 + REG_DEV_SEL_PORT, ((dev.slave_bit) ? 0xF0 : 0xE0) | ((lba >> 24) & 0x0F));
	outb(0x1F0 + REG_CMD_PORT, /*READ_SECTS_CMD*/0xEC/*identify command*/);

	while (!(inb(0x1F0 + REG_CMD_PORT) & 0x08)) {}

	for (uint16_t i = 0; i < 256; i++)
	{
		dest[i] = inw(0x1F0 + REG_DATA_PORT);
		char *text = "  \0";
		text[0] = (dest[i] >> 8) & 0xFF;
		text[1] = dest[i] & 0xFF;
		terminal_writestring(text);
	}
	
}

void ata_write_sect_lba28_pio(uint16_t *src, bdev_t dev, uint32_t lba)
{
	outb(0x1F0 + REG_INFO_PORT, 0x00);
	outb(0x1F0 + REG_SECT_CNT_PORT, 0x01);
	outb(0x1F0 + REG_LBA_LOW_PORT, (uint8_t)lba);
	outb(0x1F0 + REG_LBA_MID_PORT, (uint8_t)(lba >> 8));
	outb(0x1F0 + REG_LBA_HIGH_PORT, (uint8_t)(lba >> 16));
        outb(0x1F0 + REG_DEV_SEL_PORT, ((dev.slave_bit) ? 0xF0 : 0xE0) | ((lba >> 24) & 0x0F));
	outb(0x1F0 + REG_CMD_PORT, WRITE_SECTS_CMD);

	while (!(inb(0x1F0 + REG_CMD_PORT) & 0x08)) {}

	for (int i = 0; i < 256; ++i)
	{
		outw(0x1F0, src[i]);
	}
	//ata_flush_cache(dev);
}

void prepare_disk_read(bdev_t dev, uint32_t lba, uint32_t sect_count)
{
	const uint16_t base = dev.base;
	outb(base + REG_DEV_SEL_PORT, ((dev.slave_bit) ? 0xF0 : 0xE0) | ((lba >> 24) & 0x0F));
	outb(base + REG_INFO_PORT, 0x00);
	outb(base + REG_SECT_CNT_PORT, sect_count);
	outb(base + REG_LBA_LOW_PORT, (uint8_t)lba);
	outb(base + REG_LBA_MID_PORT, (uint8_t)(lba >> 8));
	outb(base + REG_LBA_HIGH_PORT, (uint8_t)(lba >> 16));

	outb(base + REG_CMD_PORT, READ_SECTS_CMD);

	uint8_t status = inb(dev.base + REG_CMD_PORT);
	while(((status & 0x80) == 0x80)
	      && ((status & 0x01) != 0x01))
	{
	        status = inb(dev.base + REG_CMD_PORT);
		terminal_writestring("status:\n");
		terminal_printhex(status);
	}
}

/*void ata_read_28_pio(bdev_t *dev, void *dest, uint32_t count, size_t *pos)
{
	const uint32_t rel_lba = *pos / BLOCK_SIZE;
	const uint32_t off = *pos % BLOCK_SIZE;
	const uint32_t real_lba = rel_lba + dev.start_lba;
	const uint16_t base = dev.base;
	const uint8_t sect_count = (1 + CEILING_DIV(count, 512) == 256) ? 0 : 1 + CEILING_DIV(count, 512);
	outb(base + REG_DEV_SEL_PORT, ((dev.slave_bit) ? 0xF0 : 0xE0) | ((real_lba >> 24) & 0x0F));
	outb(base + REG_INFO_PORT, 0x00);
	outb(base + REG_SECT_CNT_PORT, sect_count);
	outb(base + REG_LBA_LOW_PORT, (uint8_t)real_lba);
	outb(base + REG_LBA_MID_PORT, (uint8_t)(real_lba >> 8));
	outb(base + REG_LBA_HIGH_PORT, (uint8_t)(real_lba >> 16));

	outb(base + REG_CMD_PORT, READ_SECTS_CMD);

	uint8_t status = inb(dev.base + REG_CMD_PORT);
	while(((status & 0x80) == 0x80)
	      && ((status & 0x01) != 0x01))
	{
	        status = inb(dev.base + REG_CMD_PORT);
	}
	
	uint16_t *ptr = (uint16_t*)dest;
	for (uint16_t i = 0; i < ((sect_count == 0) ? 256 : sect_count) * 256; i++)
	{
		ptr[i] = inw(base);
	}
}

void ata_read_28_pio_offw(void *dest, uint32_t sect_off, uint32_t count, bdev_t dev, uint32_t rel_lba)
{
	const uint32_t real_lba = rel_lba + dev.start_lba;
	const uint16_t base = dev.base;
	const uint8_t sect_count = (CEILING_DIV(count + sect_off, 512) == 256) ? 0 : CEILING_DIV(count + sect_off, 512);
	outb(base + REG_DEV_SEL_PORT, ((dev.slave_bit) ? 0xF0 : 0xE0) | ((real_lba >> 24) & 0x0F));
	outb(base + REG_INFO_PORT, 0x00);
	outb(base + REG_SECT_CNT_PORT, sect_count);
	outb(base + REG_LBA_LOW_PORT, (uint8_t)real_lba);
	outb(base + REG_LBA_MID_PORT, (uint8_t)(real_lba >> 8));
	outb(base + REG_LBA_HIGH_PORT, (uint8_t)(real_lba >> 16));

	outb(base + REG_CMD_PORT, READ_SECTS_CMD);

	uint8_t status = inb(dev.base + REG_CMD_PORT);
	while(((status & 0x80) == 0x80)
	      && ((status & 0x01) != 0x01))
	{
	        status = inb(dev.base + REG_CMD_PORT);
		terminal_writestring("status:\n");
		terminal_printhex(status);
	}

	uint16_t *ptr = (uint16_t*)dest;
	for (uint16_t i = 0; i < ((sect_count == 0) ? 256 : sect_count) * 256; i++)
	{
	        const uint16_t tmp = inw(base);
		if (i >= sect_off && count > 0)
		{
			ptr[i] = tmp >> 8;
			ptr[i] |= tmp << 8;
			count--;
			terminal_printhex16(ptr[i]);
		}
	}
}*/

void ata_write_sects_28_pio(void *src, uint8_t sect_count, bdev_t *dev, uint32_t rel_lba)
{
	terminal_writestring("Writing to drive\n");

	const uint32_t real_lba = rel_lba + dev->start_lba;

	const uint16_t base = dev->base;

	outb(base + REG_DEV_SEL_PORT, ((dev->slave_bit) ? 0xF0 : 0xE0) | ((real_lba >> 24) & 0x0F));
	outb(base + REG_INFO_PORT, 0x00);
	outb(base + REG_SECT_CNT_PORT, sect_count);
	outb(base + REG_LBA_LOW_PORT, (uint8_t)real_lba);
	outb(base + REG_LBA_MID_PORT, (uint8_t)(real_lba >> 8));
	outb(base + REG_LBA_HIGH_PORT, (uint8_t)(real_lba >> 16));

	outb(base + REG_CMD_PORT, WRITE_SECTS_CMD);

        uint8_t status = inb(dev->base + REG_CMD_PORT);

	while(((status & STATUS_BSY) == STATUS_BSY)
	      && ((status & STATUS_ERR) != STATUS_ERR))
	{
	        status = inb(dev->base + REG_CMD_PORT);
		terminal_writestring("status:\n");
		terminal_printhex(status);
	}
	if (status & STATUS_ERR)
		kpanic("Status = 0x01");

	uint16_t *ptr = (uint16_t*)src;
	for (uint32_t i = 0; i < ((sect_count == 0) ? 256 : sect_count) * 256; i++)
	{
		outw(base, ptr[i]);
		terminal_printhex16(ptr[i]);
	}
}

void ata_read_sects_28_pio(void *dest, uint8_t sect_count, bdev_t *dev, uint32_t rel_lba)
{
redo:
	terminal_writestring("Reading from drive\n");

	const uint32_t real_lba = rel_lba + dev->start_lba;

	const uint16_t base = dev->base;
	//outb(base + REG_DEV_SEL_PORT, ((dev.slave_bit) ? 0xF0 : 0xE0) | ((real_lba >> 24) & 0x0F));
	outb(base + REG_DEV_SEL_PORT, ((dev->slave_bit) ? 0xF0 : 0xE0) | ((real_lba >> 24) & 0x0F));
	outb(base + REG_INFO_PORT, 0x00);
	outb(base + REG_SECT_CNT_PORT, sect_count);
	outb(base + REG_LBA_LOW_PORT, (uint8_t)real_lba);
	outb(base + REG_LBA_MID_PORT, (uint8_t)(real_lba >> 8));
	outb(base + REG_LBA_HIGH_PORT, (uint8_t)(real_lba >> 16));

	outb(base + REG_CMD_PORT, READ_SECTS_CMD);

	/*wait for a poll*/
	uint8_t status;
	/*while (1)
	  {	
	  status = inb(PRIMARY_BUS + REG_CMD_PORT);
	  if ((!(status & STATUS_BSY) && status & STATUS_DRQ) || status & STATUS_ERR || status & STATUS_DF)
	  break;
	  }
        
	  if (status & 0x01)
	  {
	  kpanic("ERROR");
	  return;
	  }*/

	/*if sect_count == 0
	  that means 256 sectors*/

        status = inb(dev->base + REG_CMD_PORT);

	while(((status & 0x80) == 0x80)
	      && ((status & 0x01) != 0x01))
	{
	        status = inb(dev->base + REG_CMD_PORT);
		terminal_writestring("status:\n");
		terminal_printhex(status);
	}
	if (status & 0x01)
		kpanic("Status = 0x01");
	/*if (status & 0x01)
	{
		ata_soft_reset(dev.dev_ctl);
		goto redo;
		}*/

	uint8_t *ptr = (uint8_t*)dest;
	for (uint32_t i = 0; i < ((sect_count == 0) ? 256 : sect_count) * 256; i++)
	{
		/*if (i % 256 == 0)
		  {
		  /*inb(REG_DEV_CTL_PORT);
		  inb(REG_DEV_CTL_PORT);
		  inb(REG_DEV_CTL_PORT);*/
		/*terminal_printhex(inb(dev.dev_ctl));
		  terminal_printhex(inb(dev.dev_ctl));
		  terminal_printhex(inb(dev.dev_ctl));
			terminal_printhex(inb(dev.dev_ctl));*/
			/*while (1)
			  {	
			  status = inb(base + REG_CMD_PORT);
			  terminal_printhex(status);
			  if ((!(status & STATUS_BSY) && status & STATUS_DRQ) || status & STATUS_ERR || status & STATUS_DF)
			  break;
			  }
        
			if (status & 0x01)
			{
				kpanic("ERROR");
				return;
				}	
				}*/
		const uint16_t tmp = inw(base);
		ptr[i*2] = (uint8_t)tmp;
		ptr[i*2 + 1] = (uint8_t)(tmp >> 8);
		terminal_printhex(ptr[i*2]);
		terminal_printhex(ptr[i*2 + 1]);
	}
}

void ata_flush_cache(bdev_t *dev)
{
	outb(dev->base + REG_DEV_SEL_PORT, MASTER_DRIVE | (dev->slave_bit << 4));
	outb(dev->base + REG_CMD_PORT, CACHE_FLUSH);

	uint8_t status = inb(dev->base + REG_CMD_PORT);
	terminal_writestring("status:\n");
	terminal_printhex(status);
	if(status == 0x00)
		return;

	while(((status & 0x80) == 0x80)
	      && ((status & 0x01) != 0x01))
	{
	        status = inb(dev->base + REG_CMD_PORT);
		terminal_writestring("status:\n");
		terminal_printhex(status);
	}

	if(status & 0x01 || status & 0x50)
	{
	        kpanic("ERROR");
		return;
	}
}
