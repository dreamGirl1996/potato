#ifndef _potato_h_
#define _potato_h_
#include<stdio.h> 
#include<string.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <netdb.h>

struct player_act{
   int pid;
  char port_num[6];
  //char neighbor_port_num[6];
  int num;
  // char *host;
  char host[512];
};

struct ugly_potato{
  int route[512];
  int total_player;
  int num_hops;
   int next;
  int next_id;
};


#endif
