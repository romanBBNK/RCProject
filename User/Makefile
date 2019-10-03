# Makefile, versao 1
# RC

SOURCES = tcp.c udp.c user.c
SOURCES+=
OBJS = $(SOURCES:%.c=%.o)
CC   = gcc
CFLAGS =-Wall -g -std=gnu99 -I../
LDFLAGS=-lm
TARGET = user

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(TARGET) $(LDFLAGS) 

user.o: user.c
udp.o: udp.c udp.h
tcp.o: tcp.c tcp.h

$(OBJS):
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo Cleaning...
	rm -f $(OBJS) $(TARGET)


