TARGET = prog
CC = gcc
CFLAGS = -Wall

.PHONY: all clean

all: $(TARGET)

# Test to automate mysql folder location
MYSQL_FLAGS = `mysql_config --cflags --libs`

# Basic compilation
% : %.c
	$(CC) $(CFLAGS) -o $@ $< $(MYSQL_FLAGS)

clean:
	rm db_server_local
