/*******************************************************************************
 **                                                                            *
 **                  Source File: main.c                                       *
 **                                                                            *
 **                                                                            *
 ** Date:   2015.7.11                                                          *
 ** team:    yhc&lx                                                            *
 ** Author: leixin                                                             *
 **                                                                            *
 ** Email:  340068408@qq.com                                                   *
 ** description:                                                               *
 **			Main program              			       *
 **                                                                            *
 ** modification history:                                                      *
 **                                                                            *
 *******************************************************************************/

#include<sys/socket.h>
#include<stdio.h>


#define READ_DATA_SIZE 1024

#ifndef _OOPS
#define _OOPS
#define oops(msg) {perror(msg);exit(1);}
#endif

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
