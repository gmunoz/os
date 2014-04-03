# Contains all common environment variables. Useful for inclusion when
# working directly in a sub-directory.

CC = gcc
AS = nasm
LD = ld
AR = ar
OBJDUMP = objdump
NM = nm
STRIP = strip
MBCHK = mbchk
RM = rm -f
SETUP = ./scripts/setup.sh

INCDIR = $(TOPDIR)/include

ifeq ($(CC),gcc)
CFLAGS = -Wall -fno-builtin -nostdinc -nostartfiles -nostdlib -I$(INCDIR) --std=c99
LD = ld
endif
ifeq ($(CC),icc)
CFLAGS = -fp -no-gcc -nolib_inline -nostdinc -nostartfiles -nostdlib -I$(INCDIR) -std=c99
LD = xild
endif

LDFLAGS = -g
