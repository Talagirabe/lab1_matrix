#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"
#include "field_info.h"
#include "tests.h"

/* Очистка буфера ввода при ошибке */
static void clearInputBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

static int readInt(const char* prompt, int* value) {
    printf("%s", prompt);
    if (scanf("%d", value) != 1) {
        clearInputBuffer();
        return 0;
    }
    return 1;
}

/* размера матрицы */
static int readSize(const char* prompt, size_t* value) {
    printf("%s", prompt);
    if (scanf("%zu", value) != 1) {
        clearInputBuffer();
        return 0;
    }
    return 1;
}

static int readDouble(const char* prompt, double* value) {
    printf("%s", prompt);
    if (scanf("%lf", value) != 1) {
        clearInputBuffer();
        return 0;
    }
    return 1;
}

static int readCharValue(const char* prompt, char* value) {
    printf("%s", prompt);
    if (scanf(" %c", value) != 1) {
        clearInputBuffer();
        return 0;
    }
    return 1;
}

static const FieldInfo* chooseType(void) {
    int choice = 0;

    printf("\nВыберите тип элементов:\n");
    printf("1. int\n");
    printf("2. double\n");
    printf("3. char\n");

    if (!readInt("Ваш выбор: ", &choice)) {
        printf("Ошибка ввода.\n");
        return NULL;
    }

    if (choice == 1) {
        return GetIntFieldInfo();
    }
    if (choice == 2) {
        return GetDoubleFieldInfo();
    }
    if (choice == 3) {
        return GetCharFieldInfo();
    }

    return NULL;
}

/* Ввод матрицы */
static Matrix* inputMatrix(void) {
    const FieldInfo* type = chooseType();
    size_t size = 0;

    if (type == NULL) {
        printf("Некорректный тип.\n");
        return NULL;
    }

    if (!readSize("Введите размер квадратной матрицы n: ", &size) || size == 0) {
        printf("Некорректный размер матрицы.\n");
        return NULL;
    }

    Matrix* matrix = MatrixCreate(size, type);
    if (matrix == NULL) {
        return NULL;
    }

    printf("Введите элементы матрицы:\n");

    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            void* temp = malloc(type->size);
            if (temp == NULL) {
                MatrixDestroy(matrix);
                return NULL;
            }

            char prompt[64];
            snprintf(prompt, sizeof(prompt), "Элемент [%zu][%zu]: ", i, j);

            if (type == GetCharFieldInfo()) {
                char ch;
                if (!readCharValue(prompt, &ch)) {
                    printf("Ошибка ввода.\n");
                    free(temp);
                    MatrixDestroy(matrix);
                    return NULL;
                }
                type->copy(&ch, temp);
            } else {
                double rawValue = 0.0;
                if (!readDouble(prompt, &rawValue)) {
                    printf("Ошибка ввода.\n");
                    free(temp);
                    MatrixDestroy(matrix);
                    return NULL;
                }
                type->fromDouble(rawValue, temp);
            }

            /* Прямая запись элемента в память матрицы */
            type->copy(temp, (char*)matrix->data + (i * matrix->size + j) * matrix->type->size);
            free(temp);
        }
    }

    return matrix;
}

/* Печать информации о матрице */
static void printMatrixInfo(const char* name, const Matrix* matrix) {
    if (matrix == NULL) {
        printf("%s: не создана.\n", name);
        return;
    }

    printf("%s: %zu x %zu, тип %s\n", name, matrix->size, matrix->size, matrix->type->typeName);
}

/* Создание матрицы в одном из слотов A или B, надо менять сам указатель поэотму двойной*/
static void createMatrixSlot(Matrix** slot, const char* name) {
    Matrix* matrix = inputMatrix();
    if (matrix == NULL) {
        return;
    }

    MatrixDestroy(*slot);
    *slot = matrix;

    printf("\nМатрица %s создана:\n", name);
    MatrixPrint(*slot);
}

static void showMatrixSlot(const Matrix* slot, const char* name) {
    printf("\nМатрица %s:\n", name);
    MatrixPrint(slot);
}

/* Сложение A + B */
static void addMatrices(const Matrix* a, const Matrix* b) {
    Matrix* result;

    if (a == NULL || b == NULL) {
        printf("\nСначала создайте обе матрицы.\n");
        return;
    }

    result = MatrixCreate(a->size, a->type);
    if (result == NULL) {
        printf("\nОшибка создания матрицы результата.\n");
        return;
    }

    MatrixAdd(a, b, result);

    printf("\nРезультат сложения:\n");
    MatrixPrint(result);
    MatrixDestroy(result);
}

/* Умножение A * B */
static void multiplyMatrices(const Matrix* a, const Matrix* b) {
    Matrix* result;

    if (a == NULL || b == NULL) {
        printf("\nСначала создайте обе матрицы.\n");
        return;
    }

    result = MatrixCreate(a->size, a->type);
    if (result == NULL) {
        printf("\nОшибка создания матрицы результата.\n");
        return;
    }

    MatrixMultiply(a, b, result);

    printf("\nРезультат умножения матриц:\n");
    MatrixPrint(result);
    MatrixDestroy(result);
}

/* Умножение одной матрицы на скаляр */
static void scalarMultiplyMatrix(const Matrix* matrix, const char* name) {
    double rawScalar = 0.0;
    void* scalar;
    Matrix* result;

    if (matrix == NULL) {
        printf("\nМатрица %s не создана.\n", name);
        return;
    }

    if (!readDouble("\nВведите скаляр: ", &rawScalar)) {
        printf("Ошибка ввода.\n");
        return;
    }

    scalar = malloc(matrix->type->size);
    if (scalar == NULL) {
        return;
    }

    if (matrix->type->fromDouble == NULL) {
        printf("\nДля данного типа матрицы умножение на скаляр не поддерживается.\n");
        free(scalar);
        return;
    }

    matrix->type->fromDouble(rawScalar, scalar);

    result = MatrixCreate(matrix->size, matrix->type);
    if (result == NULL) {
        printf("Ошибка создания матрицы результата\n");
        free(scalar);
        return;
    }

    MatrixScalarMultiply(matrix, scalar, result);

    printf("\nРезультат умножения матрицы %s на скаляр:\n", name);
    MatrixPrint(result);

    free(scalar);
    MatrixDestroy(result);
}


static void printMenu(void) {
    printf("\n=========================================\n");
    printf("      МЕНЮ РАБОТЫ С КВАДРАТНОЙ МАТРИЦЕЙ\n");
    printf("=========================================\n");
    printf("1. Создать матрицу A\n");
    printf("2. Создать матрицу B\n");
    printf("3. Показать информацию о матрицах\n");
    printf("4. Вывести матрицу A\n");
    printf("5. Вывести матрицу B\n");
    printf("6. Сложить A + B\n");
    printf("7. Умножить A * B\n");
    printf("8. Умножить матрицу A на скаляр\n");
    printf("9. Умножить матрицу B на скаляр\n");
    printf("10. Запустить тесты\n");
    printf("0. Выход\n");
}

int main(void) {
    Matrix* matrixA = NULL;
    Matrix* matrixB = NULL;
    int choice = -1;

    do {
        printMenu();

        if (!readInt("Выберите пункт: ", &choice)) {
            printf("Ошибка ввода.\n");
            continue;
        }

        switch (choice) {
            case 1:
                createMatrixSlot(&matrixA, "A");
                break;
            case 2:
                createMatrixSlot(&matrixB, "B");
                break;
            case 3:
                printMatrixInfo("A", matrixA);
                printMatrixInfo("B", matrixB);
                break;
            case 4:
                showMatrixSlot(matrixA, "A");
                break;
            case 5:
                showMatrixSlot(matrixB, "B");
                break;
            case 6:
                addMatrices(matrixA, matrixB);
                break;
            case 7:
                multiplyMatrices(matrixA, matrixB);
                break;
            case 8:
                scalarMultiplyMatrix(matrixA, "A");
                break;
            case 9:
                scalarMultiplyMatrix(matrixB, "B");
                break;
            case 10:
                RunAllTests();
                break;
            case 0:
                printf("\nЗавершение программы.\n");
                break;
            default:
                printf("\nТакого пункта нет.\n");
                break;
        }
    } while (choice != 0);

    MatrixDestroy(matrixA);
    MatrixDestroy(matrixB);
    return 0;
}
