#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

int main() {
    char *ip = (char*)"127.0.0.1";
    int port = 5400;

    int sock;
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[1024];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP client socket created.\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("[-]Connection error");
        exit(1);
    }
    printf("Rahul : Connected to the server.\n");

    srand(time(0));  // Seed for random number generation

    while (1) {
        int random_num = rand() % 100;  // Generate a random number between 0 and 99
        snprintf(buffer, sizeof(buffer), "%d", random_num);  // Convert number to string

        send(sock, buffer, strlen(buffer), 0);  // Send the random number to the server
        printf("Sent number: %d\n", random_num);

        sleep(2);  // Wait for 2 seconds before sending the next number
    }

    close(sock);
    printf("Disconnected from the server.\n");

    return 0;
}