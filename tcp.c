#include "project05.h"

int init_tcp(char *port) {

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
    if(fd == -1){
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
 

int chat_read (int chat_fd, struct users *users) {		//returns 0 if sucessful, -1 if their program is closed
	char buff[64];
	
	struct sockaddr_storage peer;
	socklen_t peer_len = sizeof(peer);
	
	int r = recv (chat_fd, buff, sizeof(buff), 0);	//recheck these inputs
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
	if ((getnameinfo((struct sockaddr *)&peer, peer_len, host, host_len, service, NI_MAXSERV, NI_NUMERICSERV) != 0)){
		fatalp("getnameinfo chat_read");
	} 

	char *name;
 	
 	for(int i = 0; i < users->count; i++){	//for every user in arr...
 		struct user_t curr_index = users->users_list[i];
 		char *curr_host = curr_index.host;
 		char *curr_name = curr_index.name;
 		
 		if((strcmp(host, curr_host)) == 0){	//if adress matches, set name
 			name = curr_name;
  			printf("%s says:", name);
  			printf("%s\n", buff);			
 			break;
 		}
 	}

	return 0;				
	
}

/*
void what_write(struct users *users){
	
}
*/
//when listener_fd readable
//char_fd = accept(fd, NULL, NULL);

//look at notes

//when char_fd readable,
		//declare buff to read into, len = its len
	//reciev(fd, buff,len, 0);
	//returns -1 if error, returns 0 if peer you're trying to talk to has quit their program
	
	//what host are they on?
	//getpeername(sdfdsf) (returns -1 if error)

	//look up user by host name 
		//associate a host w a user, record user host to put it in struct
			//when read presence, get name info (changed code there)
	//once know, then say ("%s says: %s", u->name, buff) this user says hi


//write messages:
//look up user
//at newline, send it instead of end message
