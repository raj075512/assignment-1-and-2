#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <vector>
#include <algorithm>
#include <mutex>

using namespace std;

vector<int> numbers;
mutex mtx;

void* handle_client(void* arg) {
    int client_sock = *(int*)arg;
    free(arg);

    char buffer[1024];
    while (1) {
        bzero(buffer, 1024);
        int n = recv(client_sock, buffer, sizeof(buffer), 0);
        if (n <= 0) {
            printf("[-]Client disconnected.\n");
            close(client_sock);
            return NULL;
        }

        int num = atoi(buffer);
        printf("Received number: %d\n", num);

        // Lock the mutex before modifying the shared list
        mtx.lock();
        numbers.push_back(num);
        mtx.unlock();
    }

    return NULL;
}

void* sort_numbers(void* arg) {
    while (1) {
        sleep(1);  // Sort and print every 1 second

        // Lock the mutex before accessing the shared list
        mtx.lock();
        vector<int> sorted_numbers = numbers;
        mtx.unlock();

        sort(sorted_numbers.begin(), sorted_numbers.end());

        printf("Sorted numbers: ");
        for (int num : sorted_numbers) {
            printf("%d ", num);
        }
        printf("\n");
    }
    return NULL;
}

int main() {
    char* ip = (char*)"127.0.0.1";
    int port = 5400;

    int server_sock, *client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("[-]Bind error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port);

    listen(server_sock, 5);
    printf("Listening...\n");

    pthread_t sorter_thread;
    pthread_create(&sorter_thread, NULL, sort_numbers, NULL);

    while (1) {
        addr_size = sizeof(client_addr);
        client_sock = (int*)malloc(sizeof(int));
        *client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        if (*client_sock < 0) {
            perror("[-]Client accept error");
            free(client_sock);
            continue;
        }
        printf("[+]Client connected.\n");

        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handle_client, (void*)client_sock);
        pthread_detach(client_thread);  // Detach the thread so it cleans up automatically
    }

    close(server_sock);
    return 0;
}