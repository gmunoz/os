FILES = $(wildcard *.c)
OBJECTS = $(addprefix $(OBJDIR),$(patsubst %.c,%.o,$(FILES)))

all: $(OBJFILES)

#test_list.elf:
#	$(LD) -g -Ttext 0x400000 --cref -Map test_list.map test_list.o ../asm.o ../setjmp.o ../mm/kmalloc.o ../mm/mm.o ../mm/page.o ../mm/mmap.o ../task.o ../cr0_cr3.o ../lib/list.o ../lib/rbtree.o ../mm/stack.o ../kprintf.o ../gnu.o -o test_list.elf

#test_task0.elf:
#	$(LD) -g -T test_task0.ld --cref -Map test_task0.map test_task0.o ../kprintf.o ../gnu.o -o $@
#	$(OBJDUMP) --source $@ > test_task0.src
#	$(NM) $@ | sort > test_task0.sym

clean:
	$(RM) *.{d*,o} *.{elf,map,src,sym}

include $(TOPDIR)/rules.inc
-include $(DEP)
