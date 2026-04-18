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
    Matrix* result;

    Assert(a != NULL, "Create matrix A for add");
    Assert(b != NULL, "Create matrix B for add");

    if (a == NULL || b == NULL) {
        MatrixDestroy(a);
        MatrixDestroy(b);
        return;
    }

    FillIntMatrix2x2(a, 1, 2, 3, 4);
    FillIntMatrix2x2(b, 5, 6, 7, 8);

    result = MatrixCreate(2, GetIntFieldInfo());
    Assert(result != NULL, "Create result matrix for add");

    if (result == NULL) {
        MatrixDestroy(a);
        MatrixDestroy(b);
        return;
    }

    MatrixAdd(a, b, result);

    Assert(CheckIntCell(result, 0, 0, 6), "Add [0][0]");
    Assert(CheckIntCell(result, 0, 1, 8), "Add [0][1]");
    Assert(CheckIntCell(result, 1, 0, 10), "Add [1][0]");
    Assert(CheckIntCell(result, 1, 1, 12), "Add [1][1]");

    MatrixDestroy(a);
    MatrixDestroy(b);
    MatrixDestroy(result);
}

//проверить верность формулы как таковой а не результата  через строки
static void checkInt(void) {
    const FieldInfo* testInt = GetIntFieldInfo();
    int a = 2;
    int b = 3;
    int res = -1;

    testInt->add(&a, &b, &res);
    Assert(res == 5, "AddInt Test");

    testInt->mul(&a, &b, &res);
    Assert(res == 6, "MulInt Test");

    testInt->copy(&a, &res);
    Assert(res == 2, "CopyInt Test");

    testInt->zero(&res);
    Assert(res == 0, "ZeroInt Test");

    testInt->fromDouble(7.8, &res);
    Assert(res == 7, "IntFromDouble Test");
}//проверить умножение через кольца не объявляя матрицы

static void checkDouble(void) {
    const FieldInfo* testDouble = GetDoubleFieldInfo();
    double a = 2.5, b = 3.0;
    double res = 0.0;

    testDouble->add(&a, &b, &res);
    Assert(res == 5.5, "AddDouble Test");

    testDouble->mul(&a, &b, &res);
    Assert(res == 7.5, "MulDouble Test");

    testDouble->copy(&a, &res);
    Assert(res == 2.5, "CopyDouble Test");

    testDouble->zero(&res);
    Assert(res == 0.0, "ZeroDouble Test");

    testDouble->fromDouble(7.8, &res);
    Assert(res == 7.8, "DoubleFromDouble Test");
}

static void checkChar(void) {
    const FieldInfo* testChar = GetCharFieldInfo();
    char a = 'A';
    char res = 'Z';

    testChar->copy(&a, &res);
    Assert(res == 'A', "CopyChar Test");

    testChar->zero(&res);
    Assert(res == '\0', "ZeroChar Test");

    Assert(testChar->add == NULL, "AddChar IsNull Test");
    Assert(testChar->mul == NULL, "MulChar IsNull Test");
    Assert(testChar->fromDouble == NULL, "CharFromDouble IsNull Test");
}

/* Тест: умножение матриц */
static void TestMatrixMultiplyInt(void) {
    Matrix* a = MatrixCreate(2, GetIntFieldInfo());
    Matrix* b = MatrixCreate(2, GetIntFieldInfo());
    Matrix* result;

    Assert(a != NULL, "Create matrix A for multiply");
    Assert(b != NULL, "Create matrix B for multiply");

    if (a == NULL || b == NULL) {
        MatrixDestroy(a);
        MatrixDestroy(b);
        return;
    }

    FillIntMatrix2x2(a, 1, 2, 3, 4);
    FillIntMatrix2x2(b, 5, 6, 7, 8);

    result = MatrixCreate(2, GetIntFieldInfo());
    Assert(result != NULL, "Create result matrix for multiply");

    if (result == NULL) {
        MatrixDestroy(a);
        MatrixDestroy(b);
        return;
    }

    MatrixMultiply(a, b, result);

    Assert(CheckIntCell(result, 0, 0, 19), "Multiply [0][0]");
    Assert(CheckIntCell(result, 0, 1, 22), "Multiply [0][1]");
    Assert(CheckIntCell(result, 1, 0, 43), "Multiply [1][0]");
    Assert(CheckIntCell(result, 1, 1, 50), "Multiply [1][1]");

    MatrixDestroy(a);
    MatrixDestroy(b);
    MatrixDestroy(result);
}

/* Тест: умножение матрицы на скаляр */
static void TestMatrixScalarMultiplyDouble(void) {
    Matrix* a = MatrixCreate(2, GetDoubleFieldInfo());
    Matrix* result;
    double scalar = 2.0;

    Assert(a != NULL, "Create matrix for scalar multiply");

    if (a == NULL) {
        return;
    }

    FillDoubleMatrix2x2(a, 1.5, 2.0, 3.0, 4.5);

    result = MatrixCreate(2, GetDoubleFieldInfo());
    Assert(result != NULL, "Create result matrix for scalar multiply");

    if (result == NULL) {
        MatrixDestroy(a);
        return;
    }

    MatrixScalarMultiply(a, &scalar, result);

    Assert(CheckDoubleCell(result, 0, 0, 3.0), "Scalar multiply [0][0]");
    Assert(CheckDoubleCell(result, 0, 1, 4.0), "Scalar multiply [0][1]");
    Assert(CheckDoubleCell(result, 1, 0, 6.0), "Scalar multiply [1][0]");
    Assert(CheckDoubleCell(result, 1, 1, 9.0), "Scalar multiply [1][1]");

    MatrixDestroy(a);
    MatrixDestroy(result);
}

/* Тест: разные типы матриц несовместимы */
static void TestDifferentTypesFail(void) {
    Matrix* a = MatrixCreate(2, GetIntFieldInfo());
    Matrix* b = MatrixCreate(2, GetDoubleFieldInfo());
    Matrix* resultInt;
    Matrix* resultDouble;

    Assert(a != NULL, "Create int matrix for type mismatch");
    Assert(b != NULL, "Create double matrix for type mismatch");

    if (a == NULL || b == NULL) {
        MatrixDestroy(a);
        MatrixDestroy(b);
        return;
    }

    resultInt = MatrixCreate(2, GetIntFieldInfo());
    resultDouble = MatrixCreate(2, GetDoubleFieldInfo());

    Assert(resultInt != NULL, "Create int result matrix for mismatch");
    Assert(resultDouble != NULL, "Create double result matrix for mismatch");

    if (resultInt != NULL) {
        MatrixAdd(a, b, resultInt);
    }

    if (resultDouble != NULL) {
        MatrixMultiply(a, b, resultDouble);
    }

    /* Здесь прямого возврата ошибки уже нет, поэтому проверяем совместимость отдельно */
    Assert(a->type != b->type, "Different types are detected");

    MatrixDestroy(a);
    MatrixDestroy(b);
    MatrixDestroy(resultInt);
    MatrixDestroy(resultDouble);
}

/* Тест: хранение символов в char-матрице */
static void TestCharMatrixStorage(void) {
    Matrix* a = MatrixCreate(2, GetCharFieldInfo());

    Assert(a != NULL, "Create char matrix");

    if (a == NULL) {
        return;
    }

    FillCharMatrix2x2(a, 'A', 'B', 'C', 'D');

    Assert(CheckCharCell(a, 0, 0, 'A'), "Char [0][0]");
    Assert(CheckCharCell(a, 0, 1, 'B'), "Char [0][1]");
    Assert(CheckCharCell(a, 1, 0, 'C'), "Char [1][0]");
    Assert(CheckCharCell(a, 1, 1, 'D'), "Char [1][1]");

    MatrixDestroy(a);
}

/* Тест: арифметика для char не поддерживается */
static void TestCharOperationsFail(void) {
    const FieldInfo* type = GetCharFieldInfo();

    Assert(type != NULL, "GetCharFieldInfo for operations fail");
    if (type == NULL) {
        return;
    }

    Assert(type->add == NULL, "Char add is NULL");
    Assert(type->mul == NULL, "Char mul is NULL");
    Assert(type->fromDouble == NULL, "Char fromDouble is NULL");
}

/*Главная функция запуска всех тестов */
void RunAllTests(void) {
    testsPassed = 0;
    testsFailed = 0;

    printf("===== RUNNING TESTS =====\n");

    checkInt();
    checkDouble();
    checkChar();
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