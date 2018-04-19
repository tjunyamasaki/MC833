TARGET = prog
CC = gcc
CFLAGS = -Wall

.PHONY: default all clean

default: $(TARGET)
all: default

# Test to automate mysql folder location
MYSQL_FLAGS = `mysql_config --cflags --libs`

# Basic compilation
% : %.c
	$(CC) $(CFLAGS) -o $@ $< $(MYSQL_FLAGS)

clean:
	rm m_test_client m_test_server server 2> /dev/null
