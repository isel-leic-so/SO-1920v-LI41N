BIN = bin

MEMCHANNEL_IMPL = /home/isel/src/so/SO-1920v-LI41N/aula-05-27/servers_tests/ipc/memchannel2/
INCLUDES = -I ~/src/so/SO-1920v-LI41N/lib/include/ -I$(MEMCHANNEL_IMPL)

CC = gcc
SOLIB = ~/src/so/SO-1920v-LI41N/lib/bin/solib.so

CFLAGS = -g -Wall

ECHO_SRV_SRCS = echo_srv.c $(MEMCHANNEL_IMPL)/memchan.c
ECHO_SRV_OBJS = $(ECHO_SRV_SRCS:.c=.o)

ECHO_CLI_SRCS = echo_cli.c $(MEMCHANNEL_IMPL)/memchan.c
ECHO_CLI_OBJS = $(ECHO_CLI_SRCS:.c=.o)

 
all: echo_srv echo_cli  

echo_srv:	$(ECHO_SRV_OBJS)
	$(CC) -o echo_srv  $(ECHO_SRV_OBJS) $(SOLIB) -pthread -lrt
	
echo_cli:	$(ECHO_CLI_OBJS)
	$(CC) -o echo_cli  $(ECHO_CLI_OBJS) $(SOLIB) -pthread -lrt
 

# Compile rule	
.c.o:
	$(CC)  $(CFLAGS) $(INCLUDES) -c $< 
	

clean:
	rm -rf *.o echo_cli echo_srv
	
