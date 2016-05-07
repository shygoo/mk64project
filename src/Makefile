################ Target Executable and Sources ###############

TARGET := mk64extract
SRC_FILES := mk64extract.c

OBJ_DIR     = ./obj

##################### Compiler Options #######################

WIN64_CROSS = x86_64-w64-mingw32-
WIN32_CROSS = i686-w64-mingw32-
#CROSS     = $(WIN32_CROSS)
CC        = $(CROSS)gcc
LD        = $(CC)

INCLUDES  = 
DEFS      = 
# Release flags
CFLAGS    = -Wall -Wextra -O2 -ffunction-sections -fdata-sections $(INCLUDES) $(DEFS) -MMD
LDFLAGS   = -s -Wl,--gc-sections
# Debug flags
#CFLAGS    = -Wall -Wextra -O0 -g $(INCLUDES) $(DEFS) -MMD
#LDFLAGS   =
LIBS      = 

OBJ_FILES = $(addprefix $(OBJ_DIR)/,$(SRC_FILES:.c=.o))
DEP_FILES = $(OBJ_FILES:.o=.d)

######################## Targets #############################

default: all

all: $(TARGET)

$(OBJ_DIR)/%.o: %.c
	@[ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(TARGET): $(OBJ_FILES)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(OBJ_FILES) $(DEP_FILES)
	rm -f $(TARGET) $(TARGET).exe
	-@[ -d $(OBJ_DIR) ] && rmdir --ignore-fail-on-non-empty $(OBJ_DIR)

.PHONY: all clean default

#################### Dependency Files ########################

-include $(DEP_FILES)
