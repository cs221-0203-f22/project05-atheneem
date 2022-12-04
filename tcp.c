#include "project05.h"

int init_tcp (char *port) {

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_STREAM; // datagram socket
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;          

    struct addrinfo *results;       // array, allocated in gai()
    
    int e = getaddrinfo(NULL, port, &hints, &results);	//user and port num, when write, use on recipient name/port	
    if (e != 0) {
        printf("getaddrinfo tcp: %s\n", gai_strerror(e));
        exit(-1);
    }
    
    int fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if (fd == -1) {
    	fatalp("socket");
    }

	int en = 1;
	if (ioctl(fd, FIONBIO, (char *)&en) != 0) {
		fatalp("ioctl");
	}

	int enable = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0) {
    	fatalp("setsocket reuseadddr");
    }

   if ((bind(fd, results->ai_addr, results->ai_addrlen)) != 0) {
   		fatalp("bind");
   }

   if ((listen(fd, MAXPOLLS)) != 0) {
   		fatalp("listen");
   }
   

    freeaddrinfo(results);
    return fd;
    
 }


int init_client(struct user_t *u) {

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_STREAM; // datagram socket  
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;  
    
    struct addrinfo *results;       // array, allocated in gai()
	
	int fd = socket(PF_INET, SOCK_STREAM, 0);
	if (fd == -1){
		fatalp("socket");
	}
	
	int e = getaddrinfo(u->host, u->port, &hints, &results);
    if (e != 0) {
       printf("getaddrinfo client: %s\n", gai_strerror(e));
       exit(-1);
   }

   if(connect(fd, results->ai_addr, results->ai_addrlen) == -1){
   		fatalp("connect");
   }

   return fd;
}
 

int chat_read (int chat_fd, struct users *users) {		//returns 0 if sucessful, -1 if their program is closed
	char buff[64];
	
	struct sockaddr_storage peer;
	socklen_t peer_len = sizeof(peer);
	
	int r = recv (chat_fd, buff, sizeof(buff), 0);	
	if (r == -1) {
		fatalp("recv");
	} else if (r == 0) {				//put -1 in that poll fd socket
		return -1;
	} 
	
	
	char service[64];
	
	int n = getpeername(chat_fd, (struct sockaddr *) &peer, &peer_len);
	if (n == -1) {
		fatalp("getpeername");
	}

	char host[64];
	int host_len = sizeof(host);
	if ((getnameinfo((struct sockaddr *)&peer, peer_len, host, host_len, service, NI_MAXSERV, NI_NUMERICSERV) != 0)) {
		fatalp("getnameinfo chat_read");
	} 

	char *name;
 	
 	for (int i = 0; i < users->count; i++) {	//for every user in arr...
 		struct user_t curr_index = users->users_list[i];
 		char *curr_host = curr_index.host;
 		char *curr_name = curr_index.name;
 		
 		if ((strcmp(host, curr_host)) == 0) {	//if adress matches, set name
 			name = curr_name;
  			printf("%s says:", name);
  			printf("%s\n", buff);			
 			break;
 		}
 	}

	return 0;				
	
}


int chat_write (char* message, struct users *users) {
	char name[25];
	char msg[MAXCHATLEN];
	
	if (message[0] != '@') {	//incorrect format
		return -1;
	}
	
	int i = 1;
	int name_index = 0;
	
	while (message[i] != ':') {
		char let = message[i];
		
		if (let == '\0') {		//incorrect format
			return -1;
		}
		name[name_index] = let;
		name_index++;
		i++;
	}
	name[name_index] = '\0'; 	//null terminate;
	
	int j = 0;
	i++;
	
	if (message[i] == ' '){		//skip over space
		i++;
	}
	
	while (message[i] != '\0') {
		char let = message[i];

		msg[j] = let;
		i++;
		j++;
	}
	msg[j] = '\0';		//null terminate

	int msg_len = strlen(msg) + 1;

 	for (int i = 0; i < users->count; i++) {	//for every user in arr...
		struct user_t curr_index = users->users_list[i];
		char *curr_name = curr_index.name;
		
		if ((strcmp(name, curr_name)) == 0) {	//if names match
			int client_fd = init_client(&curr_index);

			int s = send(client_fd, msg, msg_len, 0);
			if (s == -1){
				fatalp("send");
			}
							
			break;
		}
	}


	return 0;
}

