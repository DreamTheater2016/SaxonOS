#include <fs/devfs/devfs.h>

static struct super_block *super_block;

struct inode devfs_root;

bdev_t *block_head;

struct super_operations devfs_super_ops = {devfs_read_inode, 0, 0, 0, 0, 0};

struct file_operations devfs_file_ops = {/*devfs_write*/0, /*devfs_read*/0, /*devfs_open*/0, /*devfs_release*/0};

struct inode_operations devfs_inode_ops = {/*&devfs_file_ops*/0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

bdev_t *find_bdev(device_t dev)
{
	for (bdev_t **ptr = &block_head; *ptr; ptr = &(*ptr)->next)
	{
		if (MINOR_NUMBER(dev) == (*ptr)->minor)
		{
			return *ptr;
		}
	}
}

/*this is temporary*/
device_t add_bdev(bdev_t *dev)
{
        block_head = dev;
	return ((BLOCK_DEV << 8) | 0);
}

void devfs_read_inode(struct inode *dest)
{
        dest->i_ops = &devfs_inode_ops;
	dest->i_sb = super_block;
	dest->i_ops = &devfs_inode_ops;
	if (dest->i_no > 0)
	{
		if (MAJOR_NUMBER(dest->i_no == BLOCK_DEV))
		{
			for (bdev_t **ptr = &block_head; *ptr; ptr = &(*ptr)->next)
			{
				if (MINOR_NUMBER(dest->i_no) == (*ptr)->minor)
					dest->permissions = (*ptr)->permissions;
			}
		}
	}
	dest->mount_point = false;
	dest->counter = 0;
}

ssize_t devfs_read(struct file *filp, char *dest, size_t count, size_t *pos)
{
	if (MAJOR_NUMBER(filp->f_inode->i_no) == BLOCK_DEV)
	{
		return bdev_read(filp->f_inode->i_no, dest, count, pos);
	}
	kpanic("in devfs read only supports block devices");
}

ssize_t devfs_write(struct file *filp, const char *src, size_t count, size_t *pos)
{
	if (MAJOR_NUMBER(filp->f_inode->i_no) == BLOCK_DEV)
	{
		return bdev_write(filp->f_inode->i_no, src, count, pos);
	}
	kpanic("in devfs write: unsupported");
}

int devfs_open(struct inode *inode, struct file *filp)
{
	filp->pos = 0;
	filp->f_inode = inode;
	return 0;
}

int devfs_release(struct inode *inode, struct file *filp)
{
	return 0;
}

ssize_t bdev_read(device_t dev, char *dest, int count, size_t *pos)
{
        uint32_t block = *pos / BLOCK_SIZE;
        uint32_t offset = *pos % BLOCK_SIZE;
        uint32_t chars;
	uint32_t read = 0;
	struct buffer_head *bh;
	register char *ptr;

	//bdev_t *bdev = find_bdev(dev);


	while (count > 0)
	{
		//ata_read_sects_28_pio(buff, 1, bdev, block);
	        bh = get_block(block, dev);

		chars = (count < BLOCK_SIZE) ? count : BLOCK_SIZE;
		ptr = offset + bh->buff;
		offset = 0;
		block++;
		*pos += chars;
		read += chars;
		count -= chars;
		while (chars-- > 0)
			*(dest++) = *(ptr++);
		put_block(bh);
	}
	return read;
}

ssize_t bdev_write(device_t dev, const char *src, int count, size_t *pos)
{
	uint32_t block = *pos / BLOCK_SIZE;
        uint32_t offset = *pos % BLOCK_SIZE;
        uint32_t chars;
	uint32_t written = 0;
	struct buffer_head *bh;
	register char *ptr;

	//bdev_t *bdev = find_bdev(dev);


	while (count > 0)
	{
		//ata_read_sects_28_pio(buff, 1, bdev, block);
		bh = get_block(block, dev);

		bh->dirty = true;

		chars = (count < BLOCK_SIZE) ? count : BLOCK_SIZE;
		ptr = offset + bh->buff;
		offset = 0;
		block++;
		*pos += chars;
		written += chars;
		count -= chars;
		while (chars-- > 0)
			*(ptr++) = *(src++);
		put_block(bh);
	}
	return written;
}


struct super_block *devfs_read_super(struct super_block *sb)
{
	if (sb->fs_type == 0)
	{
		sb->fs_type = *find_file_system("devfs", 5);
	}
	//sb->mount_point = i_head;
	devfs_read_inode(&devfs_root);
	devfs_root.i_no = 0;
	sb->root = &devfs_root;
	sb->sb_ops = &devfs_super_ops;
	super_block = sb;
}
