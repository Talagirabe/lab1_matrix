#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>

/* Проверка, что индекс находится в пределах матрицы */
static int MatrixHasValidIndex(const Matrix* matrix, size_t row, size_t col) {
    return matrix != NULL && row < matrix->size && col < matrix->size;
}

/* Вычисление смещения элемента в памяти */
static size_t MatrixOffset(const Matrix* matrix, size_t row, size_t col) {
    return (row * matrix->size + col) * matrix->type->size;
}

/* Работа с элементом */
static void* MatrixAt(Matrix* matrix, size_t row, size_t col) {
    if (!MatrixHasValidIndex(matrix, row, col)) {
        return NULL;
    }
    return (char*)matrix->data + MatrixOffset(matrix, row, col);
}

/* работа с элементом ток для чтенея */
static const void* MatrixAtConst(const Matrix* matrix, size_t row, size_t col) {
    if (!MatrixHasValidIndex(matrix, row, col)) {
        return NULL;
    }
    return (const char*)matrix->data + MatrixOffset(matrix, row, col);
}

/* Проверка, что две матрицы можно использовать в одной операции */
static int MatrixIsCompatible(const Matrix* a, const Matrix* b) {
    return a != NULL && b != NULL && a->size == b->size && a->type == b->type;
}

/* Создание новой  матрицы */
Matrix* MatrixCreate(size_t size, const FieldInfo* type) {
    if (size == 0 || type == NULL) {
        return NULL;
    }

    Matrix* matrix = malloc(sizeof(Matrix));
    if (matrix == NULL) {
        return NULL;
    }

    matrix->size = size;
    matrix->type = type;
    matrix->data = calloc(size * size, type->size);

    if (matrix->data == NULL) {
        free(matrix);
        return NULL;
    }

    return matrix;
}

/* Освобождение памяти */
void MatrixDestroy(Matrix* matrix) {
    if (matrix == NULL) {
        return;
    }

    free(matrix->data);
    free(matrix);
}

/* Сложение двух матриц */
Matrix* MatrixAdd(const Matrix* a, const Matrix* b) {
    if (!MatrixIsCompatible(a, b) || a->type->add == NULL) {
        return NULL;
    }

    Matrix* result = MatrixCreate(a->size, a->type);
    if (result == NULL) {
        return NULL;
    }

    void* temp = malloc(a->type->size);
    if (temp == NULL) {
        MatrixDestroy(result);
        return NULL;
    }

    for (size_t i = 0; i < a->size; i++) {
        for (size_t j = 0; j < a->size; j++) {
            const void* left = MatrixAtConst(a, i, j);
            const void* right = MatrixAtConst(b, i, j);
            void* dest = MatrixAt(result, i, j);

            a->type->add(left, right, temp);
            a->type->copy(temp, dest);
        }
    }

    free(temp);
    return result;
}

/* Умножение матриц */
Matrix* MatrixMultiply(const Matrix* a, const Matrix* b) {
    if (!MatrixIsCompatible(a, b) || a->type->mul == NULL || a->type->add == NULL || a->type->zero == NULL) {
        return NULL;
    }

    Matrix* result = MatrixCreate(a->size, a->type);
    if (result == NULL) {
        return NULL;
    }

    void* sum = malloc(a->type->size);
    void* product = malloc(a->type->size);
    void* nextSum = malloc(a->type->size);

    if (sum == NULL || product == NULL || nextSum == NULL) {
        free(sum);
        free(product);
        free(nextSum);
        MatrixDestroy(result);
        return NULL;
    }

    for (size_t i = 0; i < a->size; i++) {
        for (size_t j = 0; j < a->size; j++) {
            a->type->zero(sum);

            for (size_t k = 0; k < a->size; k++) {
                const void* left = MatrixAtConst(a, i, k);
                const void* right = MatrixAtConst(b, k, j);

                a->type->mul(left, right, product);
                a->type->add(sum, product, nextSum);
                a->type->copy(nextSum, sum);
            }

            a->type->copy(sum, MatrixAt(result, i, j));
        }
    }

    free(sum);
    free(product);
    free(nextSum);
    return result;
}

/* Умножение матрицы на скаляр */
Matrix* MatrixScalarMultiply(const Matrix* matrix, const void* scalar) {
    if (matrix == NULL || scalar == NULL || matrix->type->mul == NULL) {
        return NULL;
    }

    Matrix* result = MatrixCreate(matrix->size, matrix->type);
    if (result == NULL) {
        return NULL;
    }

    void* temp = malloc(matrix->type->size);
    if (temp == NULL) {
        MatrixDestroy(result);
        return NULL;
    }

    for (size_t i = 0; i < matrix->size; i++) {
        for (size_t j = 0; j < matrix->size; j++) {
            const void* value = MatrixAtConst(matrix, i, j);
            void* dest = MatrixAt(result, i, j);

            matrix->type->mul(value, scalar, temp);
            matrix->type->copy(temp, dest);
        }
    }

    free(temp);
    return result;
}

/* Печать матрицы */
void MatrixPrint(const Matrix* matrix) {
    if (matrix == NULL) {
        printf("Матрица не создана.\n");
        return;
    }

    for (size_t i = 0; i < matrix->size; i++) {
        printf("[ ");
        for (size_t j = 0; j < matrix->size; j++) {
            const void* value = MatrixAtConst(matrix, i, j);
            matrix->type->print(value);

            if (j + 1 < matrix->size) {
                printf("  ");
            }
        }
        printf(" ]\n");
    }
}
