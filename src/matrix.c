#include "matrix.h"
#include "common.h"
#include <math.h>

void print_matrix(const Matrix* matrix) {
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            printf("%.2f\t", matrix->data[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

Matrix* create_matrix(){
    Matrix* matrix = (Matrix*)malloc(sizeof(Matrix));
    if (matrix == NULL) {
        return NULL;
    }

    return matrix;
}

void free_matrix(Matrix* matrix) {
    if (matrix == NULL) {
        return;
    }
    free(matrix);
}

Matrix* create_rand_matrix() {
    Matrix* result = create_matrix();

    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            result->data[i][j] = rand() % 100;
        }
    }

    return result;
}

int matrix_to_bytes(Matrix* matrix, char* result) {
    int offset = 0;
    double checksum = 0.0;

    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            memcpy(result + offset, &(matrix->data[i][j]), sizeof(double));
            offset += sizeof(double);

            checksum += matrix->data[i][j];
        }
    }

    memcpy(result + offset, &checksum, sizeof(double));
    offset += sizeof(double);


    return offset;
}

int matrix_from_bytes(Matrix* result, char* data) {
    int offset = 0;
    double received_sum = 0.0;

    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            memcpy(&(result->data[i][j]), data + offset, sizeof(double));
            received_sum += result->data[i][j];

            offset += sizeof(double);

        }
    }
    double checksum = 0.0;
    memcpy(&checksum, data + offset, sizeof(double));
    offset += sizeof(double);

    if (fabs(checksum - received_sum) > DBL_EPSILON) {
        perror("Error in received matrix");
        return -1;
    }

    return offset;
}


int get_elem(const Matrix* matrix, size_t row, size_t col, double* val) {
    if (matrix == NULL) {
        return -1;
    }
    *val = matrix->data[row][col];
    return 0;
}

int set_elem(Matrix* matrix, size_t row, size_t col, double val) {
    if (matrix == NULL) {
        return -1;
    }
    matrix->data[row][col] = val;
    return 0;
}

void swap_rows(Matrix* matrix, size_t row1, size_t row2) {
    for (size_t i = 0; i < MATRIX_SIZE; i++) {
        double temp = matrix->data[row1][i];
        matrix->data[row1][i] = matrix->data[row2][i];
        matrix->data[row2][i] = temp;
    }
}

Matrix* copy_matrix(const Matrix* source) {
    if (source == NULL) {
        return NULL;
    }

    Matrix* copy = create_matrix();
    if (copy == NULL) {
        return NULL;
    }

    for (size_t row_index = 0; row_index < MATRIX_SIZE; row_index++) {
        for (size_t col_index = 0; col_index < MATRIX_SIZE; col_index++) {
            double val;
            if (get_elem(source, row_index, col_index, &val) == 0) {
                set_elem(copy, row_index, col_index, val);
            } else {
                free_matrix(copy);
                return NULL;
            }
        }
    }

    return copy;
}

Matrix* triangularize(const Matrix* matrix, double * det_sign) {
    if (matrix == NULL) {
        return NULL;
    }

    Matrix* result = copy_matrix(matrix);
    if (result == NULL) {
        return NULL;
    }
    int swaps_count = 0;

    for (size_t row_index = 0; row_index < MATRIX_SIZE - 1; row_index++) {
        size_t max_row_index = row_index;
        double max_val = fabs(result->data[row_index][row_index]);

        for (size_t  find_index = row_index + 1; find_index < MATRIX_SIZE; find_index++) {
            double val = fabs(result->data[find_index][row_index]);
            if (val > max_val) {
                max_row_index = find_index;
                max_val = val;
            }
        }

        if (fabs(max_val) < DBL_EPSILON) {
            *det_sign = 0.0;
            for (size_t r_index = 0; r_index < MATRIX_SIZE; r_index++) {
                for (size_t c_index = 0; c_index < MATRIX_SIZE; c_index++) {
                    set_elem(result, r_index, c_index, 0.0);
                }
            }
            return result;
        }

        if (row_index != max_row_index) {
            swap_rows(result, row_index, max_row_index);
            swaps_count++;
        }

        for (size_t i = row_index + 1; i < MATRIX_SIZE; i++) {
            double factor = result->data[i][row_index] / result->data[row_index][row_index];
            for (size_t j = row_index + 1; j < MATRIX_SIZE; j++) {
                result->data[i][j] -= factor * result->data[row_index][j];
            }
            result->data[i][row_index] = 0;
        }
    }
    *det_sign = swaps_count % 2 == 0 ? 1.0 : -1.0;

    return result;
}

int det(const Matrix* matrix, double* val) {
    if (matrix == NULL || val == NULL) {
        return -1;
    }

    double det_sign = 1;
    Matrix* trgl_matrix = triangularize(matrix, &det_sign);
    if (trgl_matrix == NULL) {
        return -1;
    }

    if (fabs(det_sign) < DBL_EPSILON) {
        free_matrix(trgl_matrix);
        *val = 0.0;
        return 0;
    }
    *val = 1.0 * det_sign;

    for (size_t curr_index = 0; curr_index < MATRIX_SIZE; curr_index++) {
        double diagonal_element;
        if (get_elem(trgl_matrix, curr_index, curr_index, &diagonal_element) != 0) {
            free_matrix(trgl_matrix);
            return -1;
        }
        *val *= diagonal_element;
    }
    if (fabs(*val) < DBL_EPSILON && signbit(*val)) {
        *val *= (-1.0);
    }

    free_matrix(trgl_matrix);
    return 0;
}