TARGET = prog
CC = gcc
CFLAGS = -Wall

.PHONY: default all clean

default: $(TARGET)
all: default

# Change to default mysql install folder!! Check with mysql_config --cflags and --libs to check if necessary
MYSQL-C-DIR = /usr/local/mysql

# Folders where the lib and headers for the c connector
#HEADERS = -I$(MYSQL-C-DIR)/include
#LIBS = -L$(MYSQL-C-DIR)/lib -lmysqlclient

# Test to automate mysql folder location
MYSQL_FLAGS = `mysql_config --cflags --libs`

# Basic compilation
$(TARGET) : 
	$(CC) $(CFLAGS) -o server server.c
	$(CC) $(CFLAGS) -o client client.c

clean:
	rm client server
