chat_server: chat_server.h chat_server.c users.h users.c
	gcc -pthread -o chat_server chat_server.h chat_server.c users.h users.c
