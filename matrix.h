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
Matrix* MatrixAdd(const Matrix* a, const Matrix* b);
Matrix* MatrixMultiply(const Matrix* a, const Matrix* b);
Matrix* MatrixScalarMultiply(const Matrix* matrix, const void* scalar);

/* Вывод*/
void MatrixPrint(const Matrix* matrix);

#endif
