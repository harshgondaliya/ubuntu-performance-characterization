#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define NO_OF_ITERATIONS 100
#define PORT 5114
#define MSG_SIZE 10485760 //5MB

void main(){    
    int listening_sock;
	struct sockaddr_in server;
    
	//Create socket
	listening_sock = socket(AF_INET , SOCK_STREAM , 0);
	if (listening_sock == -1)
	{
		printf("Could not create socket");
        exit(1);
	}
	// printf("created socket\n");
    	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( PORT );
    
    // bind socket to a port
    if (bind(listening_sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        printf("bind failed");
        exit(1);
    }
    // printf("binded on socket\n");

    if (listen(listening_sock, 32) < 0) {
        printf("listen failed");
        exit(1);
    }
    // printf("listening on socket\n");

    struct sockaddr_in client_sin;
    int addr_len = sizeof(client_sin);
    int client_sock = accept(listening_sock, (struct sockaddr *) &client_sin, &addr_len);
    if (client_sock < 0) {
        printf("accept failed");
        exit(1);
    }
    char *rcv_message = (char *) malloc(MSG_SIZE); // 512 KB
    
    for(int j=0; j<NO_OF_ITERATIONS; j++){
        // printf("waiting for data at server\n");
        if( recv(client_sock, rcv_message , MSG_SIZE, MSG_WAITALL) < 0)
        {
            puts("recv failed");
            exit(1);
        }
        // printf("received data at server\n");
        // printf("sending data from server\n");
        if( send(client_sock , rcv_message , MSG_SIZE , MSG_WAITALL) < 0)
        {
            puts("Send failed");
            exit(1);
        }
        // printf("sent data from server\n");
    }
    char *tmp_message = (char *) malloc(MSG_SIZE);
    if( recv(client_sock, tmp_message , MSG_SIZE , MSG_WAITALL) < 0)
    {
        puts("recv failed");
        exit(1);
    }
    // printf("\nreceived msg:");
    // puts(tmp_message);
    // printf("%ld\n",strlen(tmp_message));
    close(client_sock);
}
