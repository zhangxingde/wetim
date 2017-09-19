#####用户自定义变量区######
#CC=mipsel-linux-g++
CC=g++
AR=ar
TARGET=imChatServer.exe
SQLITE3OBJ=libsqlite3.a
SRC_BASE_DIR=..
BUILD_DIR=/tmp/build
PUBHEADDIR=include_h
PUBHEADS=$(shell find $(PUBHEADDIR))
CCFLAGS=-Wall -DSYS_SERVER -DUSE_SQLITE3 -fPIC -I$(SRC_BASE_DIR) -I.. -I../include_h
FILEXXX=cpp

ifneq ($(MAKECMDGOALS), release)
CCFLAGS+=-g3 -DCC_DEBUG
RELEASE=
else
RELEASE=release
endif
LIBS=-pthread -ldl

#####用户自定义变量区######

SRC_DIRS=$(shell find $(SRC_BASE_DIR) ! -path "*wetimClient" ! -path "*.git*"  ! -path "*build-*" -type d)
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

$(TARGET): $(OBJ_DIRS) $(OBJS_HEAD) $(OBJS_NOHEAD) $(SQLITE3OBJ)
	$(CC) $(OBJS_HEAD) $(OBJS_NOHEAD) $(LIBS) $(SQLITE3OBJ) -o $@
#	$(AR) -r lib$(TARGET).a $(OBJS_HEAD) $(OBJS_NOHEAD) && ranlib lib$(TARGET).a
$(OBJ_DIRS): 
	! [ -d $@ ] && mkdir -p $@

$(OBJS_HEAD):$(BUILD_DIR)/%.o : $(SRC_BASE_DIR)/%.$(FILEXXX) $(SRC_BASE_DIR)/%.h $(PUBHEADS)
	$(CC) $(CCFLAGS) -c $< -o $@

$(OBJS_NOHEAD):$(BUILD_DIR)/%.o : $(SRC_BASE_DIR)/%.$(FILEXXX) $(PUBHEADS)
	$(CC) $(CCFLAGS) -c $< -o $@

$(SQLITE3OBJ):
	make -C ../sql/sqlite3 $(RELEASE) TARGET=$@
	[ -f ../sql/sqlite3/$@ ] && cp -f ../sql/sqlite3/$@ .  


clean:
	rm -f $(OBJS_HEAD) $(OBJS_NOHEAD) $(TARGET) $(SQLITE3OBJ)
	make -C ../sql/sqlite3 clean

release:$(TARGET)
	
