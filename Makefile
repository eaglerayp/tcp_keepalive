#
# Makefile for tcp_ka
#

CLEANFILES	 = tcp_ka_client tcp_ka_server
CFLAGS		+= -O2 -Werror -Wall -Wextra -m64 -fno-omit-frame-pointer
all: tcp_ka_client tcp_ka_server
	
tcp_ka_client: tcp_ka_client.c
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $^
tcp_ka_server: tcp_ka_server.c
	$(CC) -o $@ $^
clean:
	rm -f $(CLEANFILES) 
