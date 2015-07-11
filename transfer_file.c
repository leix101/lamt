/*******************************************************************************
 **                                                                            *
 **                  Source File: transfer_file.c                              *
 **                                                                            *
 **                                                                            *
 ** Date:   2015.7.11                                                          *
 ** team:    yhc&lx                                                            *
 ** Author: leixin                                                             *
 **                                                                            *
 ** Email:  340068408@qq.com                                                   *
 ** description:                                                               *
 **		1.select  file                                                 *
 **             2.compute file's md5 value                                     *
 **             3.wait for phone'ready                                         *
 **             4.transfer_file           	              		       *
 **                                                                            *
 ** modification history:                                                      *
 **                                                                            *
 *******************************************************************************/

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include<dirent.h>
#include<fcntl.h>

#include "md5.h"
#include "cJSON.h"

#ifndef _OOPS
#define _OOPS
#define oops(msg) {perror(msg);exit(1);}
#endif

#define READ_DATA_SIZE 1024
#define MD5_SIZE 16
#define MD5_STR_LEN (MD5_SIZE *2)

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

int is_phone_ready(int fd,char buf[])
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
	if(is_phone_ready(phone_fd,buf) !=1)
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
