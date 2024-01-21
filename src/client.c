#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "common.h"
#include "matrix.h"

int setup_client_socket () {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Failed to create client_socket");
        return -1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Failed to connect to server");
        close(client_socket);
        return -1;
    }

    return client_socket;
}

void send_matrix_to_server(int client_socket) {
    Matrix* matrix = create_rand_matrix();

    print_matrix(matrix);

    char buffer[MATRIX_SIZE * MATRIX_SIZE * sizeof(double) + sizeof(double)] = {0};

    int offset = matrix_to_bytes(matrix, buffer);

    free_matrix(matrix);

    send(client_socket, buffer, offset, 0);
}

int main() {
    int client_socket = setup_client_socket();
    if (client_socket == - 1) {
        return -1;
    }

    char choice;
    printf("Enter Y to send new random matrix to server, or any other character to exit \n");
    int scanf_res = scanf(" %c", &choice);

    while (scanf_res == 1 && (tolower(choice) == 'y')) {
        send_matrix_to_server(client_socket);

        printf("Enter Y to send new random matrix to server, or any other character to exit \n");
        scanf_res = scanf(" %c", &choice);
    }

    close(client_socket);

    return 0;
}
