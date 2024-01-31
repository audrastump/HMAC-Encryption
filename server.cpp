#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


//SERVER
int PORT = 8080;
int main(void)
{
    //CREATING SOCKET
    int server = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(PORT);
    
    //BINDING SOCKET
    if ((bind(server, (struct sockaddr*) &servAddr, sizeof(servAddr))) < 0){
        printf("Uh oh, socket not binding\n");
        return 1;
    }

    //LISTENING FOR CLIENTS
    listen(server,9);
    sockaddr_in clientSockAddr;
    socklen_t clientSockAddrSize = sizeof(clientSockAddr);

    //ACCEPTING CLIENTS
    int client = accept(server, (sockaddr *)&clientSockAddr, &clientSockAddrSize);
    printf("Client connected at port %i\n", PORT);
    char clientMessage[3000];
    while(true)
    {
        memset(clientMessage, 0, sizeof(clientMessage));
        recv(client, clientMessage, sizeof(clientMessage),0);
        printf("Client: %s\n", clientMessage);
        printf("Server: ");

        //SERVER RESPONSE
        std::string input;
        getline(std::cin, input);
        memset(&clientMessage, 0, sizeof(clientMessage));
        strcpy(clientMessage, input.c_str());
        send(client, clientMessage, strlen(clientMessage), 0);
    }
    close(client);
    close(server);
    printf("Socket closed\n");
    return 0;   
}