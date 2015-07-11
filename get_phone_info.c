/*******************************************************************************
 **                                                                            *
 **                  Source File: get_phone_info.c                             *
 **                                                                            *
 **                                                                            *
 ** Date:   2015.7.11                                                          *
 ** team:    yhc&lx                                                            *
 ** Author: leixin                                                             *
 **                                                                            *
 ** Email:  340068408@qq.com                                                   *
 ** description:                                                               *
 **		1.handle_get_phoneinfo   2.parse JSON file to phone_info      *
 **                                                                            *
 ** modification history:                                                      *
 **                                                                            *
 *******************************************************************************/

#include <stdio.h>
#include <string.h>
#include<sys/types.h>
#include<sys/socket.h>

#include "cJSON.h"

#define READ_DATA_SIZE 1024

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
