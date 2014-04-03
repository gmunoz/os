.EXPORT_ALL_VARIABLES:

TOPDIR := $(shell pwd)

include common.inc

TARGET = kernel.elf
KRNL_MAP = kernel.map
KRNL_SRC = kernel.src
KRNL_SYM = kernel.sym

SUBDIRS = $(TOPDIR)/lib $(TOPDIR)/mm $(TOPDIR)/kernel
LIB_DIR = lib
MM_DIR = mm
KERNEL_DIR = kernel

BOOT_OBJ = $(KERNEL_DIR)/boot.o  # Link this one in first!
LIB_OBJS = $(LIB_DIR)/gnu.o $(LIB_DIR)/kprintf.o $(LIB_DIR)/list.o \
	$(LIB_DIR)/multiboot.o $(LIB_DIR)/rbtree.o $(LIB_DIR)/string.o \
	$(LIB_DIR)/video.o
MM_OBJS = $(MM_DIR)/kmalloc.o $(MM_DIR)/mmap.o $(MM_DIR)/mm.o \
	$(MM_DIR)/page.o $(MM_DIR)/stack.o
KERNEL_OBJS = $(KERNEL_DIR)/asm.o \
	$(KERNEL_DIR)/elf.o \
	$(KERNEL_DIR)/interrupt.o $(KERNEL_DIR)/kernel.o \
	$(KERNEL_DIR)/panic.o $(KERNEL_DIR)/sched.o \
	$(KERNEL_DIR)/task.o $(KERNEL_DIR)/idt.o \
	$(KERNEL_DIR)/gdt.o

all:
	@for dir in $(SUBDIRS); \
		do ($(MAKE) -C $$dir all); \
	done
	$(LD) $(LDFLAGS) -T kernel.ld --cref -Map $(KRNL_MAP) \
		$(BOOT_OBJ) $(LIB_OBJS) $(MM_OBJS) $(KERNEL_OBJS) -o $(TARGET)
	$(OBJDUMP) --source $(TARGET) > $(KRNL_SRC)
	$(NM) $(TARGET) | sort > $(KRNL_SYM)
	$(STRIP) $(TARGET)
	$(MBCHK) $(TARGET)
	$(SETUP)

clean:
	@for dir in $(SUBDIRS); \
		do ($(MAKE) -C $$dir clean); \
	done
	$(RM) kernel.{elf,map,src,sym}

distclean: clean
	@for dir in $(SUBDIRS); \
		do ($(MAKE) -C $$dir distclean); \
	done

include rules.inc
