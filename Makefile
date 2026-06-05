# MCU     = attiny85
# F_CPU   = 8000000

# CC = avr-gcc
# OBJCOPY = avr-objcopy
# OBJDUMP = avr-objdump
# AVRSIZE = avr-size

# TARGET = watch

# # Object files: will find all .c/.h files in current directory
# #  and in LIBDIR.  If you have any other (sub-)directories with code,
# #  you can add them in to SOURCES below in the wildcard statement.
# SOURCES=main charliePlexing.c model.c #$(wildcard *.c $(LIBDIR)/*.c)
# OBJECTS=$(SOURCES:.c=.o)
# HEADERS=$(SOURCES:.c=.h)

# ## Compilation options, type man avr-gcc if you're curious.
# CFLAGS = -Os -g -std=gnu99 -Wall
# ## Use short (8-bit) data types 
# CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums 
# ## Splits up object files per function
# CFLAGS += -ffunction-sections -fdata-sections 
# LDFLAGS = -Wl,-Map,$(TARGET).map 
# ## Optional, but often ends up with smaller code
# LDFLAGS += -Wl,--gc-sections 
# ## Relax shrinks code even more, but makes disassembly messy
# ## LDFLAGS += -Wl,--relax
# ## LDFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm  ## for floating-point printf
# ## LDFLAGS += -Wl,-u,vfprintf -lprintf_min      ## for smaller printf
# TARGET_ARCH = -mmcu=$(MCU)

# ## Explicit pattern rules:
# ##  To make .o files from .c files 
# %.o: %.c $(HEADERS) Makefile
# 	 $(CC) $(CFLAGS) $(TARGET_ARCH) -c -o $@ $<;

# $(TARGET).elf: $(OBJECTS)
# 	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ -o $@

# %.hex: %.elf
# 	 $(OBJCOPY) -j .text -j .data -O ihex $< $@



# ============================================================================
# Project Configuration
# ============================================================================

TARGET  = main
MCU     = attiny85
F_CPU   = 8000000UL

# ============================================================================
# Tools
# ============================================================================

CC      := avr-gcc
OBJCOPY := avr-objcopy
SIZE    := avr-size

# ============================================================================
# Directories
# ============================================================================

SRC_DIR := .
OBJ_DIR := build

# ============================================================================
# Source Files
# ============================================================================

SRCS := $(wildcard $(SRC_DIR)/*.c)

OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# ============================================================================
# Compiler Flags
# ============================================================================

CFLAGS := \
    -mmcu=$(MCU) \
    -DF_CPU=$(F_CPU) \
    -Os \
    -Wall \
    -Wextra \
    -std=c11 \
    -MMD \
    -MP

LDFLAGS := -mmcu=$(MCU)

# ============================================================================
# Targets
# ============================================================================

all: $(TARGET).hex

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@
	$(SIZE) $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(TARGET).eep: $(TARGET).elf
	$(OBJCOPY) \
		-j .eeprom \
		--change-section-lma .eeprom=0 \
		-O ihex \
		$< $@

# ============================================================================
# Compilation
# ============================================================================

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================================================
# Cleanup
# ============================================================================

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(TARGET).elf
	rm -f $(TARGET).hex
	rm -f $(TARGET).eep

# ============================================================================
# Dependencies
# ============================================================================

-include $(DEPS)

.PHONY: all flash clean
