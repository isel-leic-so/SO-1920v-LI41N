BIN = bin
INCLUDES = -I/home/isel/src/so/lib/include
CC = gcc
SOLIB = ~/src/so/lib/bin/solib.so

CFLAGS = -g -Wall

MEMCHAN_SRV_SRCS = memchan_srv.c memchan.c
MEMCHAN_SRV_OBJS = $(MEMCHAN_SRV_SRCS:.c=.o)

MEMCHAN_CLI_SRCS = memchan_cli.c memchan.c
MEMCHAN_CLI_OBJS = $(MEMCHAN_CLI_SRCS:.c=.o)

all: memchan_srv memchan_cli

memchan_srv:	$(MEMCHAN_SRV_OBJS)
	$(CC) -o memchan_srv  $(MEMCHAN_SRV_OBJS) $(SOLIB) -pthread -lrt
	
memchan_cli:	$(MEMCHAN_CLI_OBJS)
	$(CC) -o memchan_cli  $(MEMCHAN_CLI_OBJS) $(SOLIB) -pthread -lrt

# Compile rule	
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	

clean:
	rm -rf *.o tmemchan
	
