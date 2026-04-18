#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>
#include "field_info.h"

typedef struct Matrix {
    size_t size;
    void* data;
    const FieldInfo* type;
} Matrix;

/* Создание и удаление матрицы */
Matrix* MatrixCreate(size_t size, const FieldInfo* type);
void MatrixDestroy(Matrix* matrix);

/* операции */
void MatrixAdd(const Matrix* a, const Matrix* b, Matrix* result);
void MatrixMultiply(const Matrix* a, const Matrix* b, Matrix* result);
void MatrixScalarMultiply(const Matrix* matrix, const void* scalar, Matrix* result);

/* Вывод*/
void MatrixPrint(const Matrix* matrix);

#endif
