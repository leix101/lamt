/***************i****************************************************************
 **                                                                            *
 **                  Source File: testserver.c                                 *
 **                                                                            *
 **                                                                            *
 ** Date:   2015.6.20                                                          *
 ** team:                                                                      *
 ** Author: leixin                                                             *
 **                                                                            *
 ** Email:  340068408@qq.com                                                   *
 ** description:                                                               *
 **          .																   *
 **                                                                            *
 ** modification history:                                                      *
 **                                                                            *
 *******************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<time.h>
#include<netdb.h>
#include<strings.h>
#include<arpa/inet.h>
#include<stdlib.h>

#define PORTNUM 8080
#define HOSTLEN 256
#define oops(msg) {perror(msg);exit(1);}
#define GET_PHONE_INFO "get_phone_info"

void handle_get_phoneinfo();

int main(int ac, char *av[])
{
	struct sockaddr_in saddr;
	struct hostent *hp;
	char hostname[HOSTLEN];
	char phone_info[BUFSIZ];
	char command[128];
	char recv_buf[BUFSIZ];
	char *chars_phoneinfo="{\"action\":\"GET_PHONE_INFO\"}";
	int sock_id,sock_fd;
	int byte;
	char *result;
	FILE *sock_fp;
	

	sock_id = socket(AF_INET,SOCK_STREAM,0);
	if(sock_id == -1)
		oops("socket");
	
	bzero((void *)&saddr,sizeof(saddr));
	gethostname(hostname,HOSTLEN);
	hp = gethostbyname(hostname);

	bcopy((void *)hp->h_addr,(void *)&saddr.sin_addr,hp->h_length);
	saddr.sin_port = htons(PORTNUM);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_family = AF_INET;

	if(bind(sock_id,(struct sockaddr *)&saddr,sizeof(saddr))!= 0)
		oops("bind");
	
	if(listen(sock_id,1)!= 0) 
		oops("listen");

	while(1) {
		sock_fd = accept(sock_id,NULL,NULL);
		printf("Connect Success!\n");
		if(sock_fd == -1)
			oops("accept");
		/*
		sock_fp = fdopen(sock_fd,"r");
		if(sock_fp == NULL)
			oops("fdopen");*/	
		
		while(1) {
			printf("input command:");
			result = fgets(command,128,stdin);
			if(result != NULL) {
				printf("your input:%s\n",command);
				if(strncmp(command,GET_PHONE_INFO,strlen(GET_PHONE_INFO))==0) {
					printf("executing get_phone_info...\n");
					handle_get_phoneinfo(sock_fd,chars_phoneinfo,recv_buf);		
				}
			}
		}
		printf("Connect closed!\n");
		//fclose(sock_fp);
		shutdown(sock_fd,2);
	}
}

/*
int getline(char *str,int lim)
{
	int c,i;
	i=0;
	while((c = getchar())!= EOF &&c!='\n'&&i<lim-1)
		s[i++];
	str[i]='\0';
	return i;
}
*/

void handle_get_phoneinfo(int fd,char str[],char buf[])
{	
	int n;
	send(fd,str,strlen(str),0);
	if((n = recv(fd,buf,1024,0)) == -1) {
		perror("recv");
		return;
	}
	buf[n] = '\0';
	printf("recv:");	
	printf("%s\n",buf);
		
}
/*
int getline(char *str,int len,FILE *file)
{
	int c,len;
	
	while((c = getc(sock_fp))!=EOF) {
		
	}
}
*/

