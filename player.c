#include "potato.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <assert.h>


int main(int argc, char *argv[])
{
  if(argc!=3){
    printf("input error");
    return -1;
  }
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = argv[1];
  const char *port     = argv[2];
  
  
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    printf("wrong getaddrinfo");
    return -1;
  } //if

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    printf("wrong socked_fd");
    return -1;
  }  
  int yes=1;
  status=setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));

    struct player_act player;      
 
    //srand( (unsigned int) time(NULL) );//generate port num
    //int random1 = rand() % 30000;
    //sprintf(player.port_num,"%d",random1);
    char str[512];
    gethostname(str,sizeof(str));
    struct hostent *hp;
        if ((hp=gethostbyname(str)) == NULL){
                return -1;
        }

        int i = 0;
        while(hp->h_addr_list[i] != NULL)
        {
	  memcpy( player.host, hp->h_name, strlen(hp->h_name)+1 );
	  // player.host=hp->h_name;
                i++;
        }

  
  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    printf("wrong connect 1");
    return -1;
  }
  //struct player_act player;
  send(socket_fd,&player,sizeof(struct player_act), 0);//send host
  int hops=0;
  recv(socket_fd,&hops,sizeof(int),0);
  //printf("hop is %d\n",hops);
  int num=0;
  recv(socket_fd,&num,sizeof(int), 0);
  player.num=num;
  i=0;
  recv(socket_fd,&i,sizeof(int), 0); //reecv pid
  int pport=i+10164;
  sprintf(player.port_num,"%d",pport);
  player.pid=i;

  printf("Connected as player %d ",player.pid);
  printf("out of %d total player\n",player.num);
  send(socket_fd,&player,sizeof(struct player_act), 0);//send pid,host,port

  //server build
  struct addrinfo server_info;
  struct addrinfo *server_info_list;
  int sstatus;
  memset(&server_info,0,sizeof(server_info));
  server_info.ai_family   = AF_UNSPEC;
  server_info.ai_socktype = SOCK_STREAM;
  server_info.ai_flags    = AI_PASSIVE;
  sstatus=getaddrinfo(player.host,player.port_num,&server_info,&server_info_list);
  if (sstatus != 0) {
    printf("wrong getaddrinfo");
    return -1;
  }
  int ssocket_fd;
  ssocket_fd = socket(server_info_list->ai_family,
                     server_info_list->ai_socktype,
                     server_info_list->ai_protocol);
  if(ssocket_fd==-1){
    printf("wrong socket\n");
  }
 yes = 1;
  sstatus = setsockopt(ssocket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  sstatus = bind(ssocket_fd, server_info_list->ai_addr, server_info_list->ai_addrlen);
  if (sstatus == -1) {
    printf("wrong bind 1");
    return -1;
  }

  sstatus = listen(ssocket_fd, 100);
  if (sstatus == -1) {
    printf("wrong listen");
    return -1;
  }
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    
  //build client
    struct player_act neighbor;
    int sssocket;
    int csocket;
    char client_host[512];
    char client_port[6];
    int cstatus;
    struct addrinfo client_info;
  struct addrinfo *client_info_list;
  memset(&client_info, 0, sizeof(client_info));
  client_info.ai_family = AF_UNSPEC;
  client_info.ai_socktype = SOCK_STREAM;  

  if(player.pid==0){
    sssocket = accept(ssocket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if(sssocket==-1){
      printf("wrong accept1\n");
      return -1;
    }
    recv(socket_fd,&neighbor,sizeof(struct player_act),0);
    cstatus = getaddrinfo(neighbor.host, neighbor.port_num, &client_info, &client_info_list);
    if (cstatus != 0) {
      printf("0's get wrong\n");
      return -1;
    }
    csocket = socket(client_info_list->ai_family,
                        client_info_list->ai_socktype,
                        client_info_list->ai_protocol);
    if (csocket == -1) {
      printf("wrong build socket\n");
      return -1;
    }
    cstatus = connect(csocket, client_info_list->ai_addr, client_info_list->ai_addrlen);
    if (cstatus == -1) {
      printf("wrong client connect\n");
      return -1;
    }
  }
  else{
    recv(socket_fd,&neighbor,sizeof(struct player_act),0);
    cstatus = getaddrinfo(neighbor.host, neighbor.port_num, &client_info, &client_info_list);
    if (cstatus != 0) {
      printf("0's get wrong\n");
      return -1;
    }
    csocket = socket(client_info_list->ai_family,
                        client_info_list->ai_socktype,
                        client_info_list->ai_protocol);
    if (csocket == -1) {
      printf("wrong build socket\n");
      return -1;
    }
    cstatus = connect(csocket, client_info_list->ai_addr, client_info_list->ai_addrlen);
    if (cstatus == -1) {
      printf("wrong client connect\n");
      return -1;
    }
    if (player.pid != player.num - 1) {
      sssocket = accept(ssocket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len); // create real socket for its neighbor                             
      if (sssocket == -1) {
	printf("Error: cannot accept connection on socket");
	return -1;
      }
    }
    else{
      int ok = 1;
      send(socket_fd, &ok, sizeof(int), 0);
      //cout << "I am waiting for palyer 0 to connect" << endl;
      sssocket = accept(ssocket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
      if (sssocket == -1) {
	printf("Error: cannot accept connection on socket");
	return -1;
      }
    }
  }

  

  
   fd_set three_fds;
 

  int next_p = player.pid;
  srand( (unsigned int) time(NULL) + next_p);
  int judge;
  // struct ugly_potato potato;
  // int next=1;
  int max_fd=0;
  
  while(1){
    
    int fd[3]={socket_fd,sssocket,csocket};
    FD_ZERO(&three_fds);
    for(int i=0;i<3;i++){
      FD_SET(fd[i],&three_fds);
      if(max_fd<fd[i]){
	max_fd=fd[i];
      }
    }

    //judge = rand() % 2;
    if (select(max_fd + 1, &three_fds, NULL, NULL, NULL) == -1){
      printf("wrong select player\n");
    }
    if (FD_ISSET(socket_fd, &three_fds)){           
      int cnt;
      struct ugly_potato potato;
      cnt=recv(socket_fd,&potato,sizeof(struct ugly_potato),MSG_WAITALL);//receive potato first time
      potato.next=1;
      /*if(player.pid!=potato.curr_id){
	continue;
	}*/
      if(cnt<=0){
	perror("wrong select\n");
	printf("wrong recv 1\n");
      }
      if(potato.total_player==-1){
	close(sssocket);
	break;
      }
     judge = rand() % 2;
     printf("judge1 is %d\n",judge);
     printf("potato is from ring to %d\n",potato.next_id);
	potato.num_hops--;
      if(judge==1){//right
	if(potato.next_id==0){
	  potato.next_id=potato.total_player-1;
	}else{
	  potato.next_id=player.pid-1;
	}
	potato.route[potato.next]=potato.next_id;
	printf("next is %d\n",potato.next);
	printf("Sending potato1.1 to %d\n", potato.next_id);
	potato.next++;
	send(csocket,&potato,sizeof(struct ugly_potato),0);
      }else if(judge==0){
      if(potato.next_id==potato.total_player-1){
	potato.next_id=0;
        }else{
          potato.next_id=player.pid+1;
	}
      potato.route[potato.next]=potato.next_id;
      //printf("next is %d\n",next);
      printf("Sending potato1.0 to %d\n", potato.next_id);
      potato.next++;
      printf("next is %d\n",potato.next);  
      send(sssocket,&potato,sizeof(struct ugly_potato),0);
      }

      continue;
      }
    if (FD_ISSET(sssocket, &three_fds)){
      /*if(potato.next_id!=player.pid){
	continue;
      }*/
      
      judge = rand() % 2;
      struct ugly_potato potato;
      recv(sssocket,&potato,sizeof(struct ugly_potato),MSG_WAITALL);//receive potato other time
      
      /* if(potato.next_id!=player.pid){
	continue;
      }*/
      judge = rand() % 2;
      printf("judge2 is %d\n",judge);
      if(potato.num_hops>0){
      potato.num_hops--;
      if(judge==1){
       if(potato.next_id==0){
          potato.next_id=potato.total_player-1;
        }else{
          potato.next_id=player.pid-1;
        }
	potato.route[potato.next]=potato.next_id;
	printf("next is %d\n",potato.next);
	printf("Sending potato2.1 to %d\n", potato.next_id);
	potato.next++;
	send(csocket,&potato,sizeof(struct ugly_potato),0);
      }else if(judge==0){
	if(potato.next_id==potato.total_player-1){
	  potato.next_id=0;
        }else{
          potato.next_id=player.pid+1;
        }
	potato.route[potato.next]=potato.next_id;
	printf("next is %d\n",potato.next);
	printf("Sending potato2.0 to %d\n", potato.next_id);
	potato.next++;
	send(sssocket,&potato,sizeof(struct ugly_potato),0);
      }
      }else{//num_hops==0
	potato.num_hops--;
	printf("I'm it\n");
	send(socket_fd,&potato,sizeof(struct ugly_potato),0);
	continue;
      }

      continue;
    }
    if (FD_ISSET(csocket, &three_fds)){
      //printf("cccc\n");
      /*if(potato.next_id!=player.pid){
	continue;
	}*/
      
      judge = rand() % 2;
      printf("judge3 is %d\n",judge);
      struct ugly_potato potato;
      recv(csocket,&potato,sizeof(struct ugly_potato),MSG_WAITALL); //receive potato other time
      
      if(potato.num_hops>0){
      potato.num_hops--;
      if(judge==1){
        if(potato.next_id==0){
          potato.next_id=potato.total_player-1;
	  printf("total num is %d\n",potato.total_player);
	}else{
          potato.next_id=player.pid-1;
        }
        potato.route[potato.next]=potato.next_id;
	printf("next is %d\n",potato.next);
	printf("Sending potato3.1 to %d\n", potato.next_id);
	potato.next++;
	send(csocket,&potato,sizeof(struct ugly_potato),0);
      }else if(judge==0){
        if(player.pid==potato.total_player-1){
          potato.next_id=0;
        }else{
          potato.next_id=player.pid+1;
        }
        potato.route[potato.next]=potato.next_id;
	printf("next is %d\n",potato.next);
	printf("Sending potato3.0 to %d\n", potato.next_id);
	potato.next++;
	send(sssocket,&potato,sizeof(struct ugly_potato),0);
      }
      }else{
	potato.num_hops--;
	printf("I'm it\n");
        send(socket_fd,&potato,sizeof(struct ugly_potato),0);
	continue;
      }

      continue;
    }
    
    // next++;
    }


  /*struct ugly_potato potato;
    recv(socket_fd,&potato,sizeof(struct ugly_potato),0);//receive potato                                                                                                                               
  
    int curr=0;
    if(potato.curr_id==player.pid){
      printf("potato's hops are %d\n",potato.num_hops);
      potato.route[curr]=player.pid;
      printf("route is %d\n",potato.route[curr]);
      int num_players=potato.total_player;

      int next_p = player.pid;
      srand( (unsigned int) time(NULL) + next_p);
      int judge = rand() % 2;
      printf("judge is %d\n",judge); 
      if(judge==1){//right                                                                                                                                                                                    
      if(player.pid==0){
        potato.next_id=num_players-1;
	printf("right player's is %d\n",potato.next_id);
      }else{
        potato.next_id=player.pid-1;
      }
      potato.num_hops--;
      send(fnew_fd,&potato,sizeof(struct ugly_potato),0);//second send
    }
    else if(judge==0){
      if(player.pid==num_players-1){
 	potato.next_id=0;
        }else{
          potato.next_id=player.pid+1;
        }
      printf("left player's is %d\n",potato.next_id);
      potato.num_hops--;
      send(osocket_fd,&potato,sizeof(struct ugly_potato),0);//second send
      }


      }*/



  freeaddrinfo(host_info_list);
  // freeaddrinfo(first_info_list);
  // freeaddrinfo(other_info_list);


  close(socket_fd);

  close(csocket);
  close(sssocket);
  //close(listen_socket);
  return 0;
}

