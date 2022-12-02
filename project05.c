#include "project05.h"

void init_fd (struct pollfd *pfd, int i,  int fd) {
	pfd[i].fd = fd;
	pfd[i].events = POLLIN;
	pfd[i].revents = 0;
}


int main(int argc, char **argv) {
//initialize users struct
	struct users users;
	users.count = 0;
	
	struct pollfd pfds[MAXPOLLS];
	bool eof = false;
	int num_pfds = 0;
	int num_readable = 0;

	int i = 0;
	int r;

	init_fd(pfds, num_pfds, STDIN_FILENO); 
	num_pfds ++;

	int presence_fd = init_presence();
	init_fd(pfds, num_pfds, presence_fd); 
	num_pfds ++;

	int listener_fd = init_tcp();					//new: init listener
	init_fd(pfds, num_pfds, listener_fd);
	num_pfds++;

//check if user included port num and name
	if(argc < 3) {
		printf("Usage: %s port\n", argv[0]);
		exit(EXIT_FAILURE);
		
	}

	char *username = argv[1];						
	char *port = argv[2];

	char arr[50];
	int index = 0;

	int poll_trips = 0;
    
	while(!eof){
		
		num_readable = poll(pfds, num_pfds, TIMEOUT);
		
		if (num_readable > 0){
			//if the POLLIN bitflag is set in revents
			for(int j = 0; j < num_pfds; j++) {		//loop through readable fds

				if (! (pfds[j].revents & POLLIN)){
					continue;
				} 
			
				if (pfds[j].fd == STDIN_FILENO) {
					//read from pfds
					char ch;			
					r = read(STDIN_FILENO, &ch, 1);
						
					if(r == 0) {
						eof = true;
						break;
					}
					
					arr[index] = ch;
					index++;
											
					if (ch == '\n') {
						arr[index] = '\0';
						printf("%s", arr);
						index = 0;
					}
							
				} else if (pfds[j].fd == presence_fd){
					struct user_t user;
					read_presence(presence_fd, &user);
				    users_update(user, &users);
				} 
				else if (pfds[j].fd == listener_fd) { 
					int chat_fd = accept(listener_fd, NULL, NULL);			//new code
					init_fd(pfds, num_pfds, chat_fd);						//would doing it this way skip over the new fd?
					num_pfds ++;
				}
				else {									//char_fd readable
					int chat = chat_read(pfds[j].fd);
					if(chat == -1){
						pfds[j].fd = -1;		//if their connection is closed, set the fd to -1
					}
				}
			} 

				
		} else if (num_readable == 0){
			poll_trips ++;
			if (poll_trips == 20){
				write_presence(presence_fd, "online", username, port);
				poll_trips = 0;
			}
			
		} else if (num_readable == -1){
			perror("poll failed");
			exit(-1);
		}


		i++;

	}
	
	write_presence(presence_fd, "offline", username, port);

	return 0;
}
					
