#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <openssl/hmac.h>
using namespace std;

void cal_hmac(unsigned char *mac, const char *message) {
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
    //creating and initializng our context
    unsigned int len = strlen(key);
    HMAC_CTX *ctx;
    ctx = HMAC_CTX_new();
    //initializing hmac to be our key, the key length, and our encryption alg
    HMAC_Init_ex(ctx, key, len, EVP_sha256(), NULL);
    //giving message to HMAC
    HMAC_Update(ctx, (unsigned char *)message, strlen(message));
    HMAC_Final(ctx, mac, NULL);
    HMAC_CTX_free(ctx);
}
void print_hex(const unsigned char *data, int len) {
    for (int i = 0; i < len; ++i) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

int main(void) {
    int PORT = 8080;
    const char *IP = "127.0.0.1";
    char serverMessage[3000]; 
    //SETTING UP SOCKET
    sockaddr_in clientSocketAddress;
    memset(&clientSocketAddress, 0, sizeof(clientSocketAddress));
    clientSocketAddress.sin_family = AF_INET;
    clientSocketAddress.sin_addr.s_addr = inet_addr(IP);
    clientSocketAddress.sin_port = htons(PORT);
    int client = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(client,(sockaddr*) &clientSocketAddress, sizeof(clientSocketAddress))>=0){
        printf("Successfully connected to server\n");
    }

    while(true) {
        printf("Client: ");
        std::string input;
        std::getline(std::cin, input);
        // Calculate HMAC
        unsigned char mac[EVP_MAX_MD_SIZE];
        cal_hmac(mac, input.c_str());
        
        printf("HMAC: ");
        print_hex(mac, EVP_MAX_MD_SIZE);

        // SEND HMAC MESSAGE
        send(client, mac,32, 0);
       
        size_t inputLength = input.length();
        send(client, input.c_str(), inputLength, 0);

        //RECEIVE SERVER MESSAGE
        printf("Waiting for server response\n");
        memset(&serverMessage, 0, sizeof(serverMessage));
        recv(client, (char*)&serverMessage, sizeof(serverMessage), 0);
        printf("Server: %s\n", serverMessage);   
    }
    close(client);
    printf("Socket closed\n");
    return 0;    
}

