# Makefile, versao 1
# RC

SOURCES = data.c tcpServer.c udpServer.c server.c
SOURCES+=
OBJS = $(SOURCES:%.c=%.o)
CC   = gcc
CFLAGS =-Wall -g -w -std=gnu99 -I../
LDFLAGS=-lm
TARGET = FS

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(TARGET) $(LDFLAGS) 

server.o: server.c
udpServer.o: udpServer.c udpServer.h
tcpServer.o: tcpServer.c tcpServer.h
data.o: data.c data.h

$(OBJS):
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo Cleaning...
	rm -f $(OBJS) $(TARGET)


