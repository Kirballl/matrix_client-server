#pragma once

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>

#define MATRIX_SIZE 6

typedef struct Matrix {
    double data[MATRIX_SIZE][MATRIX_SIZE];
} Matrix;

Matrix* create_matrix();
void free_matrix(Matrix* matrix);
Matrix* create_rand_matrix();

void print_matrix(const Matrix* matrix);

int matrix_to_bytes(Matrix* matrix, char* result);
int matrix_from_bytes(Matrix* result, char* data);

int get_elem(const Matrix* matrix, size_t row, size_t col, double* val);
int set_elem(Matrix* matrix, size_t row, size_t col, double val);
void swap_rows(Matrix* matrix, size_t row1, size_t row2);
Matrix* copy_matrix(const Matrix* source);
Matrix* triangularize(const Matrix* matrix, double * det_sign);
int det(const Matrix* matrix, double* val);
