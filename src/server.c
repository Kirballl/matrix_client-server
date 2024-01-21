#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>

#include "common.h"
#include "matrix.h"

int setup_server_socket() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Failed to create server_socket");
        return - 1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Failed to bind server socket to address");
        close(server_socket);
        return - 1;
    }

    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Failed to listen server socket");
        close(server_socket);
        return - 1;
    }

    return server_socket;
}

void receive_matrix(int client_socket) {
    double five_last_det[MAX_DETERMINANTS];
    int det_curr_index = 0;

    bool is_full_det_arr = false;
    int matrix_counter = 0;

    while (1) {
        Matrix* received_matrix = create_matrix();

        char buffer[MATRIX_SIZE * MATRIX_SIZE * sizeof(double) + sizeof(double)] = {0};
        ssize_t received_bytes = recv(client_socket, buffer, sizeof(buffer), 0);
        if (received_bytes <= 0) {
            free_matrix(received_matrix);
            break;
        }

        int offset = matrix_from_bytes(received_matrix, buffer);
        if (offset < sizeof(buffer)) {
            free_matrix(received_matrix);
            perror("Error in received buffer");
            break;
        }

        print_matrix(received_matrix);

        if (matrix_counter < 5 && !is_full_det_arr) {
            ++matrix_counter;
        } else {
            is_full_det_arr = true;
        }

        double determinant;
        if (det(received_matrix, &determinant) != 0) {
            printf("Error in finding det\n");
        }

        five_last_det[det_curr_index] = determinant;

        printf("Det. = %5.3f \n", five_last_det[det_curr_index]);

        if (is_full_det_arr) {
            double det_sum = 0.0;
            for (int j = 0; j < MAX_DETERMINANTS; ++j) {
                det_sum += five_last_det[j];
            }
            double avg_det = (det_sum) / MAX_DETERMINANTS;
            printf("Avg. det = %5.3f \n", avg_det);

            det_curr_index = (det_curr_index + 1) % MAX_DETERMINANTS;
            printf("Del. det = %5.3f \n", five_last_det[det_curr_index]);
        } else {
            det_curr_index = (det_curr_index + 1) % MAX_DETERMINANTS;
            printf("N/A\n");
            printf("N/A\n");
        }

        free_matrix(received_matrix);
    }
}

int main() {
    int server_socket = setup_server_socket();
    if (server_socket == - 1) {
        return -1;
    }

    printf("Server started\n");
    // while (1) {
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    int client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_address_len);
    if (client_socket == -1) {
        perror("Failed to accept new client connection");
        return -1;
        // continue;
    }
    printf("New client connected\n");

    receive_matrix(client_socket);
    //}

    close(server_socket);

    return 0;
}
