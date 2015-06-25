/***************i****************************************************************
 **                                                                            *
 **                  Source File: testserver.c                                 *
 **                                                                            *
 **                                                                            *
 ** Date:   2015.6.25                                                          *
 ** team:    ych&lx                                                                  *
 ** Author: leixin                                                             *
 **                                                                            *
 ** Email:  340068408@qq.com                                                   *
 ** description:                                                               *
 **   								               *
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
int build_server();
void show_ui();

int main(int ac, char *av[])
{
	char phone_info[BUFSIZ];
	char command[128];
	char recv_buf[BUFSIZ];
	char *chars_phoneinfo="{\"action\":\"GET_PHONE_INFO\"}";
	int sock_id,sock_fd;
	int byte;
	char *result;
	FILE *sock_fp;
	
	sock_id = build_serve();
	while(1) {
		sock_fd = accept(sock_id,NULL,NULL);
		if(sock_fd == -1)
			oops("accept");	
		printf("Connect Success!\n");

		while(1) {
			show_ui();
			result = fgets(command,128,stdin);
			if(result != NULL) {
				printf("\nyour input is:%s\n",command);
				if(strncmp(command,"1",1)==0){
					printf("executing get_phone_info...\n");
					handle_get_phoneinfo(sock_fd,chars_phoneinfo,recv_buf);		
				}
				if(strncmp(command,"3",1)==0) {
					break;
				}
				if(strncmp(command,"4",1)==0) {
					close(sock_fd);
					shutdown(sock_fd,2);
					exit(1);
				}
			}
		}
		printf("Connect closed!\n");
		shutdown(sock_fd,2);
	}
}

show_ui()
{
	printf("----------------------------------------------------------\n");	
	printf("-------------------Welcome to LAMT!-----------------------\n");
	printf("1.Get phone info\n");
	printf("2.\n");
	printf("3.close this connect\n");
	printf("4.quit\n");
	printf("Please choose option:");
}

int build_serve()
{
	struct sockaddr_in saddr; 
	int socket_id;	
	struct hostent *hp;
	char hostname[HOSTLEN];

	socket_id = socket(AF_INET,SOCK_STREAM,0);
	if(socket_id == -1)
		oops("socket");
	
	bzero((void *)&saddr,sizeof(saddr));
	gethostname(hostname,HOSTLEN);
	hp = gethostbyname(hostname);

	bcopy((void *)hp->h_addr,(void *)&saddr.sin_addr,hp->h_length);
	saddr.sin_port = htons(PORTNUM);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_family = AF_INET;

	if(bind(socket_id,(struct sockaddr *)&saddr,sizeof(saddr))!= 0)
		oops("bind");
	
	if(listen(socket_id,1)!= 0) 
		oops("listen");
	return socket_id;
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

