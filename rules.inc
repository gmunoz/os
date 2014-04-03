# Generic rules for building all types of files in this project

AS = nasm

DEP	= $(OBJECTS:%.o=%.d)

.SUFFIXES:	.c .asm

%.o:%.asm
	$(AS) -f elf -o $*.o $<

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.d:%.c
	@echo -n Creating dependencies for $<...
	@$(CC) -M $(CFLAGS) -c -I$(TOPDIR)/include $< > $@
	@echo " Done"
