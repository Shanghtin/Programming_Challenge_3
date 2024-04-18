# Makefile for building the NTP and local time comparison program

CC=gcc
CFLAGS=-Wall
TARGET=time_sync

all: $(TARGET)

$(TARGET): time_sync.c
	$(CC) $(CFLAGS) -o $(TARGET) time_sync.c

clean:
	rm -f $(TARGET)

.PHONY: clean
