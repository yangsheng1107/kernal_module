# ------------------------------------------------
# Generic Makefile
#
# Author: yangsheng1107@gmail.com
# Date  : 2016-02-23
#
# Changelog :
#   0.01 - first version
# ------------------------------------------------

# project name (generate executable with this name)
TARGET=test
PROJ_DIR = $(shell pwd)
UNAME_S := $(shell uname -s)

CFLAGS  = -c -Wall
LDFLAGS =

ifeq ($(UNAME_S),Linux)
    CFLAGS += -D LINUX
endif
ifeq ($(UNAME_S),Darwin)
    CFLAGS += -D OSX
endif

#header path
CFLAGS += -I$(PROJ_DIR) \

#source
C_FILES := $(wildcard *.c )
OBJ_FILES := $(addprefix , $(C_FILES:.c=.o))

.PHONY : clean all

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(LDFLAGS) $(OBJ_FILES) -o $@

.c.o:
	$(CC) $(DEBUG) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJ_FILES)
	rm -rf $(TARGET)