ifeq ($(TOPDIR),)
TOPDIR = ..
include $(TOPDIR)/common.inc
endif

FILES = $(wildcard *.c)
OBJECTS = $(addprefix $(OBJDIR),$(patsubst %.c,%.o,$(FILES)))
ASMFILES = $(wildcard *.asm)
ASMOBJS = $(addprefix $(OBJDIR),$(patsubst %.asm,%.o,$(ASMFILES)))

all: $(ASMOBJS) $(OBJECTS)

clean:
	$(RM) *.o

distclean: clean
	$(RM) *.d

include $(TOPDIR)/rules.inc
-include $(DEP)
