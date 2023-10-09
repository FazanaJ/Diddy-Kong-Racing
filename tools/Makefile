CC := gcc
CXX := g++
CFLAGS := -I . -Wall -Wextra -Wno-unused-parameter -pedantic -std=c99 -O2 -s
CXXFLAGS := -I . -Wall -Wextra -std=c++17
LDFLAGS := -lm
PROGRAMS := n64crc dkr_assets_tool flips

BUILD_DIR := dkr_assets_tool_classes/_build

ENUMS_HEADER := ../include/enums.h

# From: https://stackoverflow.com/questions/2483182/recursive-wildcards-in-gnu-make/18258352#18258352
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

default: all

n64crc_SOURCES := n64crc.c

flips_SOURCES := flipssrc/flips.cpp
flips_CFLAGS := -Wall -Wextra -fopenmp -Os -flto -fuse-linker-plugin -fomit-frame-pointer -fmerge-all-constants \
-fvisibility=hidden -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables \
-ffunction-sections -fdata-sections -Wl,--gc-sections \
-Wl,-z,relro,-z,now,--as-needed,--hash-style=gnu,--relax

all: $(PROGRAMS)

clean:
	rm -Rf $(PROGRAMS) $(BUILD_DIR)

distclean: clean

n64crc: n64crc.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

dkr_assets_tool_CPP_FILES := dkr_assets_tool.cpp \
                          $(call rwildcard,dkr_assets_tool_classes,*.cpp)

dkr_assets_tool_C_FILES := $(call rwildcard,dkr_assets_tool_classes,*.c)

dkr_assets_tool_OBJ_FILES := $(foreach file,$(dkr_assets_tool_C_FILES),$(BUILD_DIR)/$(file:.c=.o)) \
                             $(foreach file,$(dkr_assets_tool_CPP_FILES),$(BUILD_DIR)/$(file:.cpp=.o))

DUMMY != mkdir -p $(sort $(dir $(dkr_assets_tool_OBJ_FILES)))

$(BUILD_DIR)/%.o: %.c
	$(CC) -c $^ -o $@ $(CFLAGS)
$(BUILD_DIR)/%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

dkr_assets_tool: $(dkr_assets_tool_OBJ_FILES)
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LDFLAGS) -lpthread

flips: flipssrc/flips.cpp
	$(CXX) $^ -o $@ $(flips_CFLAGS) $(LDFLAGS) -lpthread

.PHONY: all clean distclean default

