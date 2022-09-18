#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define NO_OF_ITERATIONS 1000
#define PORT 5010
#define MSG_SIZE 64

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
		
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons( PORT );
    
    // bind socket to a port
    if (bind(listening_sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        printf("bind failed");
        exit(1);
    }

    if (listen(listening_sock, 32) < 0) {
        printf("listen failed");
        exit(1);
    }

    struct sockaddr_in client_sin;
    int addr_len = sizeof(client_sin);
    int client_sock = accept(listening_sock, (struct sockaddr *) &client_sin, &addr_len);
    if (client_sock < 0) {
        printf("accept failed");
        exit(1);
    }
    char *rcv_message = (char *) malloc(64); // 64 Bytes
    
    for(int j=0; j<NO_OF_ITERATIONS; j++){
        if( recv(client_sock, rcv_message , MSG_SIZE , MSG_WAITALL) < 0)
        {
            puts("recv failed");
            exit(1);
        }
        // printf("server received data %d: size%ld\n",j, strlen(rcv_message));
        // puts(rcv_message);
        if( send(client_sock , rcv_message , MSG_SIZE , MSG_WAITALL) < 0)
        {
            puts("Send failed");
            exit(1);
        }
        // printf("server sent data %d\n",j);
    }
    char *tmp_message = (char *) malloc(64);
    if( recv(client_sock, tmp_message , MSG_SIZE , 0) < 0)
    {
        puts("recv failed");
        exit(1);
    }
    printf("\nreceived msg:");
    puts(tmp_message);
    printf("%ld\n",strlen(tmp_message));
    close(client_sock);
}
