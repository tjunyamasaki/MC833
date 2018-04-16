TARGET = prog
CC = gcc
CFLAGS = -Wall

.PHONY: default all clean

default: $(TARGET)
all: default

# Change to default mysql install folder!! Check with mysql_config --cflags and --libs to check if necessary
MYSQL_C_DIR = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))mysql-connector-c/bin/

# Test to automate mysql folder location
MYSQL_FLAGS = `mysql_config --cflags --libs`

# Basic compilation
% : %.c
	export PATH=$(MYSQL_C_DIR):$(PATH)
	$(CC) $(CFLAGS) -o $@ $< $(MYSQL_FLAGS)
