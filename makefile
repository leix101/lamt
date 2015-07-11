lamt : main.o build_server.o ui.o transfer_file.o get_phone_info.o cJSON.o md5.o 
	gcc -o lamt main.o build_server.o ui.o transfer_file.o get_phone_info.o cJSON.o md5.o -lm
main.o : main.c
	gcc -c main.c
build_server.o : build_server.c
	gcc -c build_server.c
ui.o : ui.c
	gcc -c ui.c
transfer_file.o : transfer_file.c
	gcc -c transfer_file.c md5.c cJSON.c 
get_phone_info.o : get_phone_info.c
	gcc -c get_phone_info.c
cJSON.o : cJSON.c cJSON.h
	gcc -c -lm cJSON.c
md5.o : md5.c md5.h
	gcc -c md5.c
