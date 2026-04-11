#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"
#include "field_info.h"
#include "tests.h"

/* Счётчики успешных и проваленных тестов */
static int testsPassed = 0;
static int testsFailed = 0;

static void Assert(int condition, const char* testName) {
    if (condition) {
        printf("[OK] %s\n", testName);
        testsPassed++;
    } else {
        printf("[FAIL] %s\n", testName);
        testsFailed++;
    }
}

/*  Заполнение int-матрицы 2x2 значениями */
static void FillIntMatrix2x2(Matrix* matrix, int a00, int a01, int a10, int a11) {
    int values[4] = {a00, a01, a10, a11};

    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 2; j++) {
            /* вычисляем адрес ячейки вручную */
            void* cell = (char*)matrix->data +
                         (i * matrix->size + j) * matrix->type->size;

            /* копируем значение в матрицу */
            matrix->type->copy(&values[i * 2 + j], cell);
        }
    }
}

/* Заполнение double-матрицы 2x2  */
static void FillDoubleMatrix2x2(Matrix* matrix, double a00, double a01, double a10, double a11) {
    double values[4] = {a00, a01, a10, a11};

    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 2; j++) {
            void* cell = (char*)matrix->data +
                         (i * matrix->size + j) * matrix->type->size;

            matrix->type->copy(&values[i * 2 + j], cell);
        }
    }
}

/* Заполнение char-матрицы 2x2 */
static void FillCharMatrix2x2(Matrix* matrix, char a00, char a01, char a10, char a11) {
    char values[4] = {a00, a01, a10, a11};

    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 2; j++) {
            void* cell = (char*)matrix->data +
                         (i * matrix->size + j) * matrix->type->size;

            matrix->type->copy(&values[i * 2 + j], cell);
        }
    }
}

/* Проверка значения в int-ячейке */
static int CheckIntCell(const Matrix* matrix, size_t row, size_t col, int expected) {
    const int* value = (const int*)((const char*)matrix->data +
                                    (row * matrix->size + col) * matrix->type->size);

    return *value == expected;
}

/* Проверка значения в double-ячейке (с учётом погрешности) */
static int CheckDoubleCell(const Matrix* matrix, size_t row, size_t col, double expected) {
    const double* value = (const double*)((const char*)matrix->data +
                                          (row * matrix->size + col) * matrix->type->size);

    double diff = *value - expected;
    if (diff < 0) diff = -diff;

    return diff < 1e-9; /* допускаем небольшую ошибку */
}

/* Проверка char-ячейки */
static int CheckCharCell(const Matrix* matrix, size_t row, size_t col, char expected) {
    const char* value = (const char*)((const char*)matrix->data +
                                      (row * matrix->size + col) * matrix->type->size);

    return *value == expected;
}

/* Тест: создание int-матрицы */
static void TestMatrixCreateInt(void) {
    Matrix* matrix = MatrixCreate(2, GetIntFieldInfo());

    Assert(matrix != NULL, "Create int matrix");
    Assert(matrix && matrix->size == 2, "Matrix size correct");
    Assert(matrix && matrix->type == GetIntFieldInfo(), "Matrix type int");

    MatrixDestroy(matrix);
}

/* Тест: создание char-матрицы */
static void TestMatrixCreateChar(void) {
    Matrix* matrix = MatrixCreate(2, GetCharFieldInfo());

    Assert(matrix != NULL, "Create char matrix");
    Assert(matrix && matrix->type == GetCharFieldInfo(), "Matrix type char");

    MatrixDestroy(matrix);
}

/* Тест: сложение матриц */
static void TestMatrixAddInt(void) {
    Matrix* a = MatrixCreate(2, GetIntFieldInfo());
    Matrix* b = MatrixCreate(2, GetIntFieldInfo());

    FillIntMatrix2x2(a, 1, 2, 3, 4);
    FillIntMatrix2x2(b, 5, 6, 7, 8);

    Matrix* result = MatrixAdd(a, b);

    Assert(result != NULL, "Add matrices");
    Assert(result && CheckIntCell(result, 0, 0, 6), "Add [0][0]");
    Assert(result && CheckIntCell(result, 1, 1, 12), "Add [1][1]");

    MatrixDestroy(a);
    MatrixDestroy(b);
    MatrixDestroy(result);
}

/* Тест: умножение матриц */
static void TestMatrixMultiplyInt(void) {
    Matrix* a = MatrixCreate(2, GetIntFieldInfo());
    Matrix* b = MatrixCreate(2, GetIntFieldInfo());

    FillIntMatrix2x2(a, 1, 2, 3, 4);
    FillIntMatrix2x2(b, 5, 6, 7, 8);

    Matrix* result = MatrixMultiply(a, b);

    Assert(result != NULL, "Multiply matrices");
    Assert(result && CheckIntCell(result, 0, 0, 19), "Mul [0][0]");
    Assert(result && CheckIntCell(result, 1, 1, 50), "Mul [1][1]");

    MatrixDestroy(a);
    MatrixDestroy(b);
    MatrixDestroy(result);
}

/* Тест: умножение на скаляр */
static void TestMatrixScalarMultiplyDouble(void) {
    Matrix* a = MatrixCreate(2, GetDoubleFieldInfo());
    double scalar = 2.0;

    FillDoubleMatrix2x2(a, 1.5, 2.0, 3.5, 4.0);

    Matrix* result = MatrixScalarMultiply(a, &scalar);

    Assert(result != NULL, "Scalar multiply");
    Assert(result && CheckDoubleCell(result, 0, 0, 3.0), "Scalar [0][0]");

    MatrixDestroy(a);
    MatrixDestroy(result);
}

/* Тест: разные типы должны давать ошибку */
static void TestDifferentTypesFail(void) {
    Matrix* a = MatrixCreate(2, GetIntFieldInfo());
    Matrix* b = MatrixCreate(2, GetDoubleFieldInfo());

    Assert(MatrixAdd(a, b) == NULL, "Add different types fails");
    Assert(MatrixMultiply(a, b) == NULL, "Multiply different types fails");

    MatrixDestroy(a);
    MatrixDestroy(b);
}

/*Тест: char хранится правильно*/
static void TestCharMatrixStorage(void) {
    Matrix* a = MatrixCreate(2, GetCharFieldInfo());

    FillCharMatrix2x2(a, 'A', 'B', 'C', 'D');

    Assert(CheckCharCell(a, 0, 0, 'A'), "Char A");
    Assert(CheckCharCell(a, 1, 1, 'D'), "Char D");

    MatrixDestroy(a);
}

/* Тест: операции для char запрещены */
static void TestCharOperationsFail(void) {
    Matrix* a = MatrixCreate(2, GetCharFieldInfo());
    Matrix* b = MatrixCreate(2, GetCharFieldInfo());

    Assert(MatrixAdd(a, b) == NULL, "Char add fails");
    Assert(MatrixMultiply(a, b) == NULL, "Char multiply fails");

    MatrixDestroy(a);
    MatrixDestroy(b);
}

/*Главная функция запуска всех тестов */
void RunAllTests(void) {
    testsPassed = 0;
    testsFailed = 0;

    printf("===== RUNNING TESTS =====\n");

    TestMatrixCreateInt();
    TestMatrixCreateChar();
    TestMatrixAddInt();
    TestMatrixMultiplyInt();
    TestMatrixScalarMultiplyDouble();
    TestDifferentTypesFail();
    TestCharMatrixStorage();
    TestCharOperationsFail();

    printf("\n===== TEST SUMMARY =====\n");
    printf("Passed: %d\n", testsPassed);
    printf("Failed: %d\n", testsFailed);
}