/***************i****************************************************************
 **                                                                            *
 **                  Source File: testserver.c                                 *
 **                                                                            *
 **                                                                            *
 ** Date:   2015.6.25                                                          *
 ** team:    yhc&lx                                                                  *
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
#include<cJSON.h>
#include<fcntl.h>
#include<dirent.h>

#include"md5.h"

#define PORTNUM 8080
#define HOSTLEN 256
#define oops(msg) {perror(msg);exit(1);}
#define MD5_SIZE 16
#define MD5_STR_LEN (MD5_SIZE *2)
#define READ_DATA_SIZE 1024

void handle_get_phoneinfo();
int build_server();
void show_ui();
int get_select_option();

int main(int ac, char *av[])
{
	char phone_info[READ_DATA_SIZE];
	char command[128];
	char buff[READ_DATA_SIZE];
	int sock_id,sock_fd;
	int byte;
	int selected_option;
	FILE *sock_fp;
	
	sock_id = build_server();

	while(1) {
		sock_fd = accept(sock_id,NULL,NULL);
		if(sock_fd == -1)
			oops("accept");	
		printf("Connect Success!\n");

		while(1) {
			show_ui();
			selected_option = get_select_option();
			if(selected_option == -1) { 
				perror("get_selected_option failure");
				continue;
			}	
			switch(selected_option) {
				case 0:{
					printf("Executing get_phone_info...\n");
					handle_get_phoneinfo(sock_fd,buff);		
					break;
				}
				case 1: {
					printf("Executing deliver file...\n");
					handle_transfer_file(sock_fd,buff);
					break;
				}
				case 2: {
					break;
				}
				case 3: {
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

int get_select_option()
{
	char option[128];
	
	while((fgets(option,128,stdin))!=NULL) {
		if(strlen(option) >2)
			printf("too much worlds...\ntry again:");
		else if(option[0]>='0'&&option[0]<='9')
			return option[0]-'0';
		else 
			printf("no such option!\ntry agian:");
	}
	return -1;
}


show_ui()
{
	printf("----------------------------------------------------------\n");	
	printf("-------------------Welcome to LAMT!-----------------------\n");
	printf("0.Get phone info\n");
	printf("1.Deliver a small file to phone\n");
	printf("2.Close this connect\n");
	printf("3.Quit\n");
	printf("Please select option:");
}

void handle_transfer_file(int phone_fd,char buf[])
{
	char filepath[MAXNAMLEN+1];
	int out_fd;
	int len,md5_ret;
	char md5_str[MD5_STR_LEN +1];
	char str_ready_file[BUFSIZ];
	
	select_file(filepath);
	printf("filepath is :%s\n",filepath);

	if((out_fd = open(filepath,O_RDONLY)) == -1) {
		perror("open");
		return;
	}
	md5_ret = compute_file_md5(out_fd,md5_str);
	if(md5_ret != 0) {
		printf("Cannot get file's md5 value.\n");
		return;
	}
	printf("md5_str is:%s\n",md5_str);
	len = sprintf(str_ready_file,"{\"action\":\"FILE_TRANSFER\",\"extra_data\":{\"filename\":\"%s\",\"md5\":\"%s\"}}",filepath,md5_str);	
	printf("send_message is:%s\n",str_ready_file);
	printf("sprintf len is:%d\n",len);
	write(phone_fd,str_ready_file,strlen(str_ready_file));
	if(is_ready(phone_fd,buf) !=1)
		return;
	while((len = read(out_fd,&buf,READ_DATA_SIZE))!=0) {
		if(len == -1)
			oops("read");
		if(write(phone_fd,buf,len) != len)
			oops("write");
	}
	close(out_fd);
	printf("Deliver success!\n");
}

int is_ready(int fd,char buf[])
{
	cJSON *json;
	printf("run is_ready\n");
	if(recv(fd,buf,READ_DATA_SIZE,0)  ==-1)
		oops("recv");
	printf("recv finish\n");
        json = cJSON_Parse(buf);
	if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr()); return -1;}
        else if(strcmp(cJSON_GetObjectItem(json,"action")->valuestring,"ACK") == 0) { 
		printf("%s\n",cJSON_Print(json));
		cJSON_Delete(json);
		return 1;
        }
}

void select_file(char *filename)
{	
	if(fgets(filename,MAXNAMLEN+1,stdin) == -1)
		oops("fgets");
	filename[strlen(filename)-1] = '\0';

	return;
}

int compute_file_md5(int fd, char *md5_str)
{
	int i;
	int ret;
	unsigned char data[READ_DATA_SIZE];
  	unsigned char md5_value[MD5_SIZE];
  	MD5_CTX md5;

	// init md5	
	MD5Init(&md5);

 	 while (1) {
    		ret = read(fd, data, READ_DATA_SIZE);
    		if (-1 == ret) {
      			perror("read");
      			return -1;
    		}

   		MD5Update(&md5, data, ret);

   		if (0 == ret || ret < READ_DATA_SIZE) {
     			 break;
   		}
  	}
	
  	lseek(fd,0,SEEK_SET);

  	MD5Final(&md5, md5_value);
	
	for(i = 0; i < MD5_SIZE; i++) {
    		snprintf(md5_str + i*2, 2+1, "%02x", md5_value[i]);
 	}
  	md5_str[MD5_STR_LEN] = '\0'; // add end

  	return 0;
}

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
	printf("Ready for client...\n");
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

void handle_get_phoneinfo(int fd,char buf[])
{	
	int n;
	char *send_message = "{\"action\":\"GET_PHONE_INFO\"}";

	send(fd,send_message,strlen(send_message),0);
	if((n = recv(fd,buf,READ_DATA_SIZE,0)) == -1) {
		perror("recv");
		return;
	}
	buf[n] = '\0';
	printf("recv:");
	parser_to_phoneinfo(buf);		
}

void parser_to_phoneinfo(char *s)
{
	cJSON *json;
	char *out;
	
	json = cJSON_Parse(s);
	if (!json) {
		printf("Error before: [%s]\n",cJSON_GetErrorPtr());
	}
	else {
		out=cJSON_Print(json);
		cJSON_Delete(json);
		printf("%s\n",out);
		free(out);
	}
}
