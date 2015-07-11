/*******************************************************************************
 **                                                                            *
 **                  Source File: ui.c                                         *
 **                                                                            *
 **                                                                            *
 ** Date:   2015.7.11                                                          *
 ** team:    yhc&lx                                                            *
 ** Author: leixin                                                             *
 **                                                                            *
 ** Email:  340068408@qq.com                                                   *
 ** description:                                                               *
 **		1.shou UI          2.get user's select			       *
 **                                                                            *
 ** modification history:                                                      *
 **                                                                            *
 *******************************************************************************/

#include <stdio.h>
#include <string.h>

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
