#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "potato.h"
#include <assert.h>

int main(int argc, char *argv[])
{
  if(argc!=4){
    printf("wrong input format!");
    return -1;
  }
  const char*port_num=argv[1];
  size_t num_players=atoi(argv[2]);
  size_t num_hops=atoi(argv[3]);

  
  assert(num_players>1);
  if(num_hops<0||num_hops>512){
    printf("wrong input format!");
    return -1;
  }
  printf("Potato Ringmaster\n");
  printf("Players = %ld\n", num_players);
  printf("Hops = %ld\n", num_hops);
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = "0.0.0.0";
  memset(&host_info, 0, sizeof(host_info));

  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags    = AI_PASSIVE;

  status = getaddrinfo(hostname, port_num, &host_info, &host_info_list);
  if (status != 0) {
    printf("wrong getaddrinfo");
    return -1;
  }

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    printf("wrong socket");
    return -1;
  }

  int yes = 1;
  status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    printf("wrong bind 1");
    return -1;
  }

  status = listen(socket_fd, num_players);
  if (status == -1) {
    printf("wrong listen");
    return -1;
  }


  struct sockaddr_storage socket_addr;
  socklen_t socket_addr_len = sizeof(socket_addr);
  int new_fd[num_players];
  struct player_act player[num_players];
  int i=0;
  for(i=0;i<num_players;i++){
    new_fd[i] = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);//
  
    if (new_fd[i] == -1) {
    printf("wrong accept with player");
    return -1;
    } 
    recv(new_fd[i],&player[i],sizeof(struct player_act),0);//hostname
    //player[i].num=num_players;
    //player[i].pid=i;
    send(new_fd[i],&num_hops,sizeof(int),0);//send # hops)
    send(new_fd[i],&num_players,sizeof(int),0);//send #
    send(new_fd[i],&i,sizeof(int),0);//send himeself pid
    printf("send pid is %d\n",i);
    recv(new_fd[i],&player[i],sizeof(struct player_act),0);//pid,host,port
   
    printf("player %d is ready to play\n", player[i].pid);
    // char buffer[512];
    //recv(new_fd[i],buffer,sizeof(char *),0);//receive ready info
   
    if(i!=0){
      //  printf("sent to i's with neigh %s\n", player[i].neighbor_port_num);
      send(new_fd[i],&player[i-1],sizeof(struct player_act),0);//send neighbor info
      if(i==num_players-1){
	int ok;//last sent
	recv(new_fd[i],&ok,sizeof(int),0);
	printf("ok is %d\n",ok);
	send(new_fd[0],&player[num_players-1],sizeof(struct player_act),0);
      }
    }
    if(i==0){
      continue;
    }
 }


  /*  //  char * message="lalala\n";
  int haha = 10000;
  srand(num_hops+num_players);
  int random = rand() % num_players;
  printf("sending to player %d\n", random);
  send(new_fd[random],&haha,sizeof(haha),0);
  printf(">>>\n");
  fd_set player_fds;
  FD_ZERO(&player_fds);
  int max_fd=0;
  for(int i=0;i<num_players;i++){

    FD_SET(new_fd[i],&player_fds);
    if(max_fd<new_fd[i]){
      max_fd=new_fd[i];
    }
    }*/

    struct ugly_potato potato;
  potato.num_hops=num_hops;
  potato.total_player=num_players;
  
  srand( (unsigned int) time(0));
  int random = rand() % num_players;
  printf("potato picks %d\n",random);
  //potato.curr_id=random;
  potato.next_id=random;
  potato.route[0]=potato.next_id;
  //send(new_fd[random],&potato,sizeof(struct ugly_potato),0);//send first potato

  fd_set player_fds;
  FD_ZERO(&player_fds);
  int max_fd=0;
  
  for(int i=0;i<num_players;i++){
    FD_SET(new_fd[i],&player_fds);
    if(max_fd<new_fd[i]){
      max_fd=new_fd[i];
    }
  }

  // printf("max_fd is %d\n",max_fd);
  send(new_fd[random],&potato,sizeof(struct ugly_potato),0);//send first potato
  
  
    if(select(max_fd+1,&player_fds,NULL,NULL,NULL)==-1){
      printf("wrong select ring\n");
      return -1;
    }
 

  for (int i = 0; i < num_players; i++){
    if (FD_ISSET(new_fd[i], &player_fds)){
      int end = recv(new_fd[i], &potato, sizeof( struct ugly_potato), MSG_WAITALL);
      printf("begin to close\n");
      if (end == -1){
	  perror("cannot receive potato at game end");
	  exit(EXIT_FAILURE);
	}
    }
    }

  /*int ss;
	for (int i = 0; i < num_players; i++){
	  ss = shutdown(new_fd[i], SHUT_RDWR);
	  if (ss != 0){
	    printf("close all server\n");
	    return -1;
	  }
	  }*/

	printf("Trace of potato:\n");

	for(int i=0;i<num_hops;i++){
	  printf("%d, ",potato.route[i]);
	}
	for (int i = 0; i < num_players; i++) {
	  struct ugly_potato stop;
	  stop.total_player=-1;
	  send(new_fd[i], &stop, sizeof(struct ugly_potato), 0);
	}

	//printf("%d",potato.route);
    /*int temp=num_hops;
  int n=1;
  while(temp>0){
    if(temp==num_hops){
      potato.potato_id=random;
      printf("potato is in %d\n",potato.potato_id);
      // potato.num_hops=num_hops;
      potato.route[0]=potato.potato_id;
      n++;
      send(new_fd[random],&potato,sizeof(struct ugly_potato),0);//first send                                                                                                                                
      recv(new_fd[random],&potato,sizeof(struct ugly_potato),0);//recv first potato                                                                                                                         
    } 
    int next=potato.potato_id;
    potato.route[n]=potato.potato_id;
    printf("next player is %d\n",next);
    send(new_fd[next],&potato,sizeof(struct ugly_potato),0);//send potato to next player
    recv(new_fd[next],&potato,sizeof(struct ugly_potato),0);
    n++;
    temp--;
    }*/


  
  
  freeaddrinfo(host_info_list);
  close(socket_fd);

  return 0;
}
