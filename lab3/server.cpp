#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <openssl/hmac.h>
#include <openssl/err.h>

//SERVER
int hmacBufferLength = 32;
int PORT = 8080;
void printHmac(const unsigned char *data, int len) {
    for (int i = 0; i < len; ++i) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

void cal_hmac(unsigned char *mac, char *message) {
    //need a 32 byte key for the 256 bit encryption
    const char key[32] = {
    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x16, 0x17,
    0x18
    };
    unsigned int len = strlen(key);
    HMAC_CTX *ctx;
    ctx = HMAC_CTX_new();
    //initializing hmac to be our key, the key length, and our encryption alg
    HMAC_Init_ex(ctx, key, len, EVP_sha256(), NULL);
    HMAC_Update(ctx, (unsigned char *)message, strlen(message));
    HMAC_Final(ctx, mac, NULL);
    HMAC_CTX_free(ctx);
}

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
        // RECIEVE HMAC AND CLIENT MESSAGE
        unsigned char receivedHMAC[hmacBufferLength];
        recv(client, receivedHMAC, hmacBufferLength, 0);
        memset(clientMessage, 0, sizeof(clientMessage));
        recv(client, clientMessage, sizeof(clientMessage), 0);
        
        // CALCULATE HMAC BASED ON CLIENT MESSAGE
        unsigned char calculatedHMAC[hmacBufferLength];
        cal_hmac(calculatedHMAC, clientMessage);
        // Printing out both server's received HMAC and calculated HMAC
        printf("Calculated HMAC: ");
        printHmac(calculatedHMAC, hmacBufferLength);
        printf("Received HMAC: ");
        printHmac(receivedHMAC, hmacBufferLength);
        
        //AUTHENTICATING HMAC
        bool authenticated = true;
        for (int i = 0; i < hmacBufferLength; ++i) {
        if (receivedHMAC[i] != calculatedHMAC[i]) {
            authenticated = false;
            break;
            }
        }
        // PRINT CLIENT MESSAGE AND AUTHENTICATION
        printf("Client Message: %s\n", clientMessage);
        printf("Authentication: %s\n", authenticated ? "Authentic Message" : "Not Authentic Message");
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