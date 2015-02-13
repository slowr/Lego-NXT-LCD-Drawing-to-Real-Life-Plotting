# make PREFIX point to the toolchain path
PREFIX=../toolchain

########################################################################
# You should not need to modify these settings
########################################################################
ARMBIN=$(PREFIX)/arm/bin
SHELL = sh
CC = $(ARMBIN)/arm-elf-gcc -B $(ARMBIN)
OBJCOPY = $(ARMBIN)/arm-elf-objcopy
OBJDUMP = $(ARMBIN)/arm-elf-objdump
NM = $(ARMBIN)/arm-elf-nm
SIZE = $(ARMBIN)/arm-elf-size
REMOVE = rm -f
COPY = cp
FLASH_TOOL = $(PREFIX)/libnxt-0.3/fwflash 
ELFSIZE = $(SIZE) -A $(TARGET).elf
MCU      = arm7tdmi	# MCU name and submodel
SUBMDL   = AT91SAM7S256
# Create ROM-Image (final) or RAM-Image (debugging) 
RUN_MODE=ROM
#RUN_MODE=RAM
TARGET = firmware
#TARGET = m_sched	# comment this out if you do not have the source
#TARGET = main		# and uncomment this and read the test in main.c
# Exception-Vector placement only supported for RUN_MODE=ROM
# (placement settings ignored when using RUN_MODE=RAM)
VECTOR_LOCATION = VECTORS_IN_RAM	# - Exception vectors in RAM:

########################################################################
# Files
########################################################################
SRC  += drafting.c drawing.c helpfunc.c d_output.c input.c button.c sound.c aic.c display.c pit.c i2c.c hwinit.c spi.c led.c Cstartup_SAM7.c $(TARGET).c
ASRC  = Cstartup.S 
COBJ  = $(SRC:.c=.o) 
AOBJ  = $(ASRC:.S=.o)
LST   = $(ASRC:.S=.lst) $(SRC:.c=.lst) 

########################################################################
# Flags
########################################################################
# Compiler flags
#  -Wa,...:      tell GCC to pass this to the assembler.
#  -adhlns...: create assembler listing
CFLAGS = 
CFLAGS += -D$(RUN_MODE) -DSAM7S256 -DPROTOTYPE_PCB_4 -D__ICCARM__ -DNEW_MENU -D__WinARM__ -D__WINARMSUBMDL_$(SUBMDL)__ \
          -mcpu=$(MCU) -Os -Wall -Wcast-align -Wimplicit -Wpointer-arith -Wswitch -Wredundant-decls \
          -Wreturn-type -Wshadow -Wunused -Wnested-externs \
          -ffunction-sections -fdata-sections \
          -Wa,-adhlns=$(subst $(suffix $<),.lst,$<) -I$(PREFIX)/arm/include -I.
#CFLAGS += -Wcast-qual -Wmissing-prototypes -Wstrict-prototypes -Wmissing-declarations
#CFLAGS += -fpack-struct #increases code size too much (using __attribute__((__packed__)) in module.h
# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
DEBUG = dwarf-2
#CFLAGS += -g$(DEBUG)
# Optimization level, can be [0, 1, 2, 3, s]. 
# 0 = turn off optimization. s = optimize for size.
# (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
CFLAGS += -Os
# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CFLAGS += -std=gnu99
# Compiler flags to generate dependency files.
CFLAGS  += -MD -MP -MF .dep/$(@F).d

# Assembler flags
#  -Wa,...:    tell GCC to pass this to the assembler.
#  -ahlns:     create listing
#  -g$(DEBUG): have the assembler create line number information
AFLAGS =
AFLAGS += -D$(RUN_MODE) -Wa,-adhlns=$(<:.S=.lst),--g$(DEBUG) -D__WinARM__ -D__WINARMSUBMDL_$(SUBMDL)__
AFLAGS += -mcpu=$(MCU) -I. -x assembler-with-cpp 

ifdef VECTOR_LOCATION
AFLAGS += -D$(VECTOR_LOCATION)
CFLAGS += -D$(VECTOR_LOCATION)
endif

# Linker flags
#  -Wl,...:   tell GCC to pass this to linker.
#  -Map:      create map file
#  --cref:    add cross reference to  map file
LDFLAGS = 
LDFLAGS += -nostartfiles -Wl,-Map=$(TARGET).map,--cref,--gc-sections
LDFLAGS += -L$(PREFIX)/lib/thumb -lm#/interwork -lc -lgcc -lm
LDFLAGS += -T./$(SUBMDL)-$(RUN_MODE).ld

########################################################################
# Targets
########################################################################
.PHONY : all depend elfsize clean download

all: $(TARGET).elf $(TARGET).bin $(TARGET).lss $(TARGET).sym

depend:
	makedepend 

elfsize:
	$(ELFSIZE)

download: $(TARGET).bin
	sudo $(FLASH_TOOL) $(TARGET).bin

# Create final output file (.hex) from ELF output file.
%.hex: %.elf
	$(OBJCOPY) -O binary $< $@
	
# Create final output file (.bin) from ELF output file.
%.bin: %.elf
	$(OBJCOPY) -O binary $< $@
	#cp $@ $(TARGET).rfw
	#ls -al $(TARGET).bin

# Create extended listing file from ELF output file.
# testing: option -C
%.lss: %.elf
	$(OBJDUMP) -h -S -C $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	$(NM) -n $< > $@

.SECONDARY : $(TARGET).elf
.PRECIOUS : $(AOBJ) $(COBJ)  
%.elf:  $(AOBJ) $(COBJ)  
	$(CC) $(CFLAGS) $(AOBJ) $(COBJ)  --output $@ $(LDFLAGS)

$(COBJ) : %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@ 

$(AOBJ) : %.o : %.S
	$(CC) -c $(AFLAGS) $< -o $@

clean: 
	$(REMOVE) $(TARGET).hex $(TARGET).bin $(TARGET).obj $(TARGET).elf $(TARGET).map \
                  $(TARGET).obj $(TARGET).a90 $(TARGET).sym $(TARGET).lnk $(TARGET).lss $(TARGET).rfw \
	          $(COBJ) $(AOBJ) $(LST) $(SRC:.c=.s) $(SRC:.c=.d) .dep/*

########################################################################
# Dependencies
########################################################################
# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# DO NOT DELETE
