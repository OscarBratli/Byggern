# Source file selection: All files by default, dependency-only with DEPS_ONLY=1
# Usage: make DEPS_ONLY=1 or make flash DEPS_ONLY=1
ifeq ($(DEPS_ONLY),1)
    # Use automatic dependency detection to find only required files
    SOURCE_FILES := $(shell ./find_deps.sh)
else
    # Default: compile all C files in src directory (excluding node-2)
    SOURCE_FILES := $(shell find src -name '*.c' -not -path "src/node-2/*")
endif

# Set this flag to "yes" (no quotes) to use JTAG; otherwise ISP (SPI) is used
PROGRAM_WITH_JTAG := yes

# Feel free to ignore anything below this line
PROGRAMMER := atmelice_isp
ifeq ($(PROGRAM_WITH_JTAG), yes)
	PROGRAMMER := atmelice
endif

BUILD_DIR := build
TARGET_CPU := atmega162
TARGET_DEVICE := m162

CC := avr-gcc
CFLAGS := -O -std=c11 -mmcu=$(TARGET_CPU) -ggdb -Isrc -I. -ffunction-sections -fdata-sections -flto
LDFLAGS := -Wl,--gc-sections -flto

OBJECT_FILES = $(SOURCE_FILES:%.c=$(BUILD_DIR)/%.o)

.DEFAULT_GOAL := $(BUILD_DIR)/main.hex

$(BUILD_DIR):
	mkdir $(BUILD_DIR)


# Ensure build subdirectory exists before compiling
$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/main.hex: $(OBJECT_FILES) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECT_FILES) -o $(BUILD_DIR)/a.out
	avr-objcopy -j .text -j .data -O ihex $(BUILD_DIR)/a.out $(BUILD_DIR)/main.hex

.PHONY: flash
flash: $(BUILD_DIR)/main.hex
	avrdude -p $(TARGET_DEVICE) -c $(PROGRAMMER) -U flash:w:$(BUILD_DIR)/main.hex:i

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: erase
erase:
	avrdude -p $(TARGET_DEVICE) -c $(PROGRAMMER) -e
	
.PHONY: show-deps
show-deps:
	@echo "Files that will be compiled (automatically detected dependencies):"
	@./find_deps.sh
	@echo ""
	@echo "Files that will be excluded (not referenced by dependency chain):"
	@./find_deps.sh > /tmp/included_files
	@find src -name '*.c' 2>/dev/null | grep -v -f /tmp/included_files || true
	@rm -f /tmp/included_files

.PHONY: memory
memory: $(BUILD_DIR)/a.out
	avr-size -C --mcu=$(TARGET_CPU) $(BUILD_DIR)/a.out

.PHONY: size
size: $(BUILD_DIR)/a.out
	avr-size $(BUILD_DIR)/a.out

.PHONY: help
help:
	@echo "Available targets:"
	@echo "  make                    - Build project (all source files)"
	@echo "  make DEPS_ONLY=1        - Build project (dependency-detected files only)"
	@echo "  make flash              - Flash to device (all files by default)"
	@echo "  make flash DEPS_ONLY=1  - Flash with dependency detection"
	@echo "  make clean              - Clean build directory"
	@echo "  make memory             - Show detailed memory usage"
	@echo "  make size               - Show raw memory sections"
	@echo "  make show-deps          - Show dependency analysis"
	@echo "  make help               - Show this help"
	@echo ""
	@echo "Variables:"
	@echo "  DEPS_ONLY=1             - Enable automatic dependency detection"

.PHONY: debug
debug:
	if pgrep avarice; then pkill avarice; fi
	avrdude -p $(TARGET_DEVICE) -c $(PROGRAMMER) -U flash:w:$(BUILD_DIR)/main.hex:i
	x-terminal-emulator -e avarice --edbg --ignore-intr :4242
	sleep 2
	avr-gdb -tui -iex "target remote localhost:4242" $(BUILD_DIR)/a.out
	killall -s 9 avarice	