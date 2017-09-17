#####用户自定义变量区######
#CC=mipsel-linux-g++
CC=gcc
AR=ar
TARGET=libsqlite3.a
SRC_BASE_DIR=.
BUILD_DIR=/tmp/build
PUBHEADDIR=include_h
PUBHEADS=$(shell find $(PUBHEADDIR))
CCFLAGS=-Wall
FILEXXX=c

ifneq ($(MAKECMDGOALS), release)
CCFLAGS+=-g3 -DCC_DEBUG
OWNLIBS=
else
OWNLIBS=
endif
LIBS=

#####用户自定义变量区######

SRC_DIRS=$(shell find $(SRC_BASE_DIR) ! -path "*wetimClient" -type d)
OBJ_DIRS=$(SRC_DIRS:$(SRC_BASE_DIR)%=$(BUILD_DIR)%)
ALL_FILES= $(shell find $(SRC_BASE_DIR) ! -path "*wetimClient*" -print)
SRC_FILES= $(filter %.$(FILEXXX), $(ALL_FILES))
SRC_HEADS= $(filter %.h, $(ALL_FILES))

ALLOBJS=$(SRC_FILES:$(SRC_BASE_DIR)/%.$(FILEXXX)=$(BUILD_DIR)/%.o)
OBJS_TMP=$(SRC_HEADS:$(SRC_BASE_DIR)/%.h=$(BUILD_DIR)/%.o)

OBJS_HEAD=$(filter $(OBJS_TMP), $(ALLOBJS))
OBJS_NOHEAD=$(filter-out $(OBJS_HEAD), $(ALLOBJS))
OBJS_PUBS=$(filter-out $(OBJS_HEAD), $(OBJS_TMP))
PUBHEADS+=$(OBJS_PUBS:$(BUILD_DIR)/%.o=$(SRC_BASE_DIR)/%.h)

.PHONY:clean

$(TARGET): $(OBJ_DIRS) $(OBJS_HEAD) $(OBJS_NOHEAD) 
#	$(CC) $(OBJS_HEAD) $(OBJS_NOHEAD) $(LIBS) -o $@
	$(AR) -r $(TARGET) $(OBJS_HEAD) $(OBJS_NOHEAD) && ranlib $(TARGET)
$(OBJ_DIRS): 
	! [ -d $@ ] && mkdir -p $@

$(OBJS_HEAD):$(BUILD_DIR)/%.o : $(SRC_BASE_DIR)/%.$(FILEXXX) $(SRC_BASE_DIR)/%.h $(PUBHEADS)
	$(CC) $(CCFLAGS) -c $< -o $@

$(OBJS_NOHEAD):$(BUILD_DIR)/%.o : $(SRC_BASE_DIR)/%.$(FILEXXX) $(PUBHEADS)
	$(CC) $(CCFLAGS) -c $< -o $@



clean:
	rm -f $(OBJS_HEAD) $(OBJS_NOHEAD) $(TARGET)

release:$(TARGET)
	
