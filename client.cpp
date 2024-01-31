#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

//CLIENT
int main(void)
{
    //VARIABLE DEFINITION
    int PORT = 8080;
    const char *IP = "127.0.0.1";
    char MESSAGE[3000]; 

    //CREATING SOCKET
    sockaddr_in clientSocketAddress;
    memset(&clientSocketAddress, 0, sizeof(clientSocketAddress));
    clientSocketAddress.sin_family = AF_INET;
    clientSocketAddress.sin_addr.s_addr = inet_addr(IP);
    clientSocketAddress.sin_port = htons(PORT);
    int client = socket(AF_INET, SOCK_STREAM, 0);

    //CONNECTING TO SERVER
    if (connect(client,(sockaddr*) &clientSocketAddress, sizeof(clientSocketAddress))>=0){
        printf("Successfully connected to server\n");
    }

    while(true)
    {
        //SENDING MESSAGE TO SERVER
        std::string input;
        std::getline(std::cin, input);
        memset(&MESSAGE, 0, sizeof(MESSAGE));
        strcpy(MESSAGE, input.c_str());
        send(client, (char*)&MESSAGE, strlen(MESSAGE), 0);
        printf("Waiting for server response\n");

        //RECEIVING MESSAGE FROM SERVER
        memset(&MESSAGE, 0, sizeof(MESSAGE));
        recv(client, (char*)&MESSAGE, sizeof(MESSAGE), 0);
        printf("Client: ");
    }
    close(client);
    printf("Socket closed\n");
    return 0;    
}