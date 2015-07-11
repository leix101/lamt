/*******************************************************************************
 **                                                                            *
 **                  Source File: build_server.c                               *
 **                                                                            *
 **                                                                            *
 ** Date:   2015.7.11                                                          *
 ** team:    yhc&lx                                                            *
 ** Author: leixin                                                             *
 **                                                                            *
 ** Email:  340068408@qq.com                                                   *
 ** description:                                                               *
 **		build server to connect mobile phone.			       *
 **                                                                            *
 ** modification history:                                                      *
 **                                                                            *
 *******************************************************************************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

#define HOSTLEN 256
#define PORTNUM 8080

#ifndef _OOPS
#define _OOPS
#define oops(msg) {perror(msg);exit(1);}
#endif

int build_server()
{
	struct sockaddr_in saddr; 
	int socket_id;	
	struct hostent *hp;
	char hostname[HOSTLEN];

	socket_id = socket(AF_INET,SOCK_STREAM,0);
	if(socket_id == -1)
		oops("socket");
	
	bzero((void *)&saddr,sizeof(saddr));
	gethostname(hostname,HOSTLEN); hp = gethostbyname(hostname);

	bcopy((void *)hp->h_addr,(void *)&saddr.sin_addr,hp->h_length);
	saddr.sin_port = htons(PORTNUM);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_family = AF_INET;

	if(bind(socket_id,(struct sockaddr *)&saddr,sizeof(saddr))!= 0)
		oops("bind");
	
	if(listen(socket_id,1)!= 0) 

		oops("listen");
	printf("Ready for client...\n");
	return socket_id;
}
