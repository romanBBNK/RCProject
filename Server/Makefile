# Makefile, versao 1
# RC

SOURCES = server.c
SOURCES+=
OBJS = $(SOURCES:%.c=%.o)
CC   = gcc
CFLAGS =-Wall -g -std=gnu99 -I../
LDFLAGS=-lm
TARGET = FS

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(TARGET) $(LDFLAGS) 

server.o: server.c

$(OBJS):
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo Cleaning...
	rm -f $(OBJS) $(TARGET)


