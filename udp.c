#include "project05.c"

int init_presence() {

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // datagram socket
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;          // any protocol

    struct addrinfo *results;       // array, allocated in gai()
    int e = getaddrinfo(NULL, "8221", &hints, &results);			
    if (e != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(e));
        exit(-1);
    }
    
    int fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if(fd == -1){
    	fatalp("socket");
    }
    
    int enable = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(int)) != 0) {
    	fatalp("setsocket broadcast");
    }
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0) {
    	fatalp("setsocket reuseadddr");
    }
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) != 0) {
    	fatalp("setsocket reuseport");
    }

   if(bind(fd, results->ai_addr, results->ai_addrlen) != 0) {
   		fatalp("bind");
   }

    freeaddrinfo(results);
    return fd;
    
 }


 void write_presence(int fd, char* status, char* name, char* port) {
 	char buff[64];
 	int n = snprintf(buff, 64, "%s %s %s", name, status, port);
 	if(n == 0){
 		fatalp("snprintf");
 	}
 	struct sockaddr_in addr;
 	int i = inet_pton(AF_INET, "10.10.13.255", &addr.sin_addr);
 	if(i != 1){
 		fatalp("inet_pton");
 	}
 	addr.sin_port = htons(8221);
 	addr.sin_family = AF_INET;
 	socklen_t len = sizeof(addr);
 	int bufsize = strlen(buff) + 1;
 	int s = sendto(fd, buff, bufsize, 0, (struct sockaddr *)&addr, len);
 	if(s == -1){
 		fatalp("sendto");
 	}
 }


 void read_presence(int fd, struct user_t *u) {
  	struct sockaddr_storage store;
  	char buff[64];
  	socklen_t store_len = sizeof(store);
 
 	if (recvfrom(fd, buff, sizeof(buff), 0, (struct sockaddr *)&store, &store_len) == -1) {
 		fatalp("read");
 	}

 	if (getnameinfo((struct sockaddr *) &store, &store_len, u->host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV) != 0){		//added (not working)
 		fatalp("getnameinfo");
 	}
 
  	sscanf(buff, "%s %s %s", u->name, u->status, u->port);
  	
  }


 void users_update(struct user_t user, struct users *users){
 	bool print = false;
 	bool in_arr = false;
 
 	char *name = user.name;
 	char *status = user.status;
 	
 	for(int i = 0; i < users->count; i++){	//for every user in arr...
 		struct user_t curr_index = users->users_list[i];
 		char *curr_name = curr_index.name;
 		char *curr_status = curr_index.status;
 		
 		if((strcmp(name, curr_name)) == 0){	//if in list
 			in_arr = true;
 			if(strcmp(status, curr_status) != 0){	//if status different, print
 				print = true;
 			}
 		}
 	}
 	if(!in_arr){		//if user not in arr, add 
 		users->users_list[users->count] = user;
 		printf("%s is %s on %s\n", name, status, user.port);
 		users->count ++;
 	}
 
 	if(print){
 		printf("%s is %s on %s\n", name, status, user.port);
 	}
 }
 
