#include <fs/vfs/vfs.h>

static struct file_system_type *file_systems;

static struct super_block *sb_head;
static struct inode *i_head;
static struct dentry *d_head;


void add_inode(struct inode *inode)
{
	inode->next = i_head;
	i_head = inode;
}

void add_dir(struct dentry *dentry)
{
	dentry->next = d_head;
	d_head->prev = dentry;
	d_head = dentry;
}

void add_sb(struct super_block *sb)
{
	sb->next = sb_head;
	sb_head = sb;
}

void init_fs(struct file_system_type *root_fs, device_t dev)
{
	register_filesystem(root_fs);
	terminal_printhex32(root_fs->read_super);
	sb_head = (struct super_block*)kmalloc(sizeof(struct super_block));
	sb_head->dev = dev;
	root_fs->read_super(sb_head, sb_head->dev);
		terminal_writestring("in init_fs");
	if (sb_head->root == 0)
		kpanic("no root inode");
	i_head = sb_head->root;
	d_head = (struct dentry*)kmalloc(sizeof(struct dentry));
	d_head->inode = i_head;
	d_head->name = NULL;
}

struct super_block *find_super_block(const char *name)
{
	struct file_system_type *fs_type = *find_file_system(name, strlen(name));
	if (!fs_type)
		return NULL;
        struct super_block *sb = sb_head;
	for (sb; sb != NULL; sb = sb->next)
	{
		if (sb->fs_type == fs_type)
			return sb;
	}
	return NULL;
}

struct file_system_type **find_file_system(const char *name, uint8_t len)
{
	struct file_system_type **ptr = &file_systems;

	for (ptr; *ptr != 0; ptr = &(*ptr)->next)
	{
		if (len != 0 && !strncmp(name, (*ptr)->name, len))
			break;
	}
	return ptr;
}

int register_filesystem(struct file_system_type *fs)
{
	if (fs->next != 0)
	        return -1;
	struct file_system_type **ptr;
	ptr = find_file_system(fs->name, strlen(fs->name));

	if (*ptr != 0)
		return -1;

	*ptr = fs;

	return 0;
}

int unregister_filesystem(struct file_system_type *fs)
{
	struct file_system_type **ptr = &file_systems;

	while (*ptr)
	{
		if (*ptr == fs)
		{
			*ptr = fs->next;
			fs->next = 0;
			return 0;
		}
	        ptr = &(*ptr)->next;
	}
	return -1;
}

