void terminal_writestring(char*);

int sys_write(const char* str)
{
	terminal_writestring(str);
	return 0;
}
