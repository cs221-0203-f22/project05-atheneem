#include <stdio.h>
#include <poll.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>


#define TIMEOUT 100	//miliseconds
#define MAXPOLLS 64	//num poll fds in arr
#define BROADCAST 1


struct user_t {
	char name[64];
	char port[64];
	char status[64];
	char host[64];
};

struct users {
	struct user_t users_list[64];
	int count;
};


void fatalp(char* string) {
	perror(string);
	exit(EXIT_FAILURE);
}

//udp.c
int init_presence();
void read_presence(int fd, struct user_t *u);
void write_presence(int fd, char* status, char* name, char* port);
void users_update(struct user_t user, struct users *users);


//tcp.c
int init_tcp();
int chat_read (int chat_fd);
