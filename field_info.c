#include "field_info.h"

#include <stdio.h>
#include <stdlib.h>

/* ---------------- int ---------------- */

static void intAdd(const void* a, const void* b, void* result) {
    *(int*)result = *(const int*)a + *(const int*)b;
}

static void intMul(const void* a, const void* b, void* result) {
    *(int*)result = (*(const int*)a) * (*(const int*)b);
}

static void intCopy(const void* src, void* dst) {
    *(int*)dst = *(const int*)src;
}

static void intFromDouble(double value, void* result) {
    *(int*)result = (int)value;
}

static void intZero(void* value) {
    *(int*)value = 0;
}

static void intPrint(const void* value) {
    printf("%d", *(const int*)value);
}
//исаользовать парадигму объект кк глобальная консттанта с ленивой инициализацией и мемоизацией
// маллоком указатель объявляем тк статические данные это отдельное место в памяти и оно создается без прямого вызова
const FieldInfo* GetIntFieldInfo(void) {
    static FieldInfo* intFields = NULL; // статический указатель на FieldInfo

    if (intFields == NULL) {            // проверка на NULL заменяет флаг
        intFields = malloc(sizeof(*intFields));
        intFields->size = sizeof(int);
        intFields->typeName = "int";
        intFields->add = intAdd;
        intFields->mul = intMul;
        intFields->copy = intCopy;
        intFields->fromDouble = intFromDouble;
        intFields->zero = intZero;
        intFields->print = intPrint;
    }

    return intFields;
}

/* ---------------- double ---------------- */

static void doubleAdd(const void* a, const void* b, void* result) {
    *(double*)result = *(const double*)a + *(const double*)b;
}

static void doubleMul(const void* a, const void* b, void* result) {
    *(double*)result = (*(const double*)a) * (*(const double*)b);
}

static void doubleCopy(const void* src, void* dst) {
    *(double*)dst = *(const double*)src;
}

static void doubleFromDouble(double value, void* result) {
    *(double*)result = value;
}

static void doubleZero(void* value) {
    *(double*)value = 0.0;
}

static void doublePrint(const void* value) {
    printf("%.2lf", *(const double*)value);
}

const FieldInfo* GetDoubleFieldInfo(void) {
    static FieldInfo* doubleFields = NULL;

    if (doubleFields == NULL) {
        doubleFields = malloc(sizeof(*doubleFields));
        doubleFields->size = sizeof(double);
        doubleFields->typeName = "double";
        doubleFields->add = doubleAdd;
        doubleFields->mul = doubleMul;
        doubleFields->copy = doubleCopy;
        doubleFields->fromDouble = doubleFromDouble;
        doubleFields->zero = doubleZero;
        doubleFields->print = doublePrint;
    }

    return doubleFields;
}

/* ---------------- char ----------------*/

static void charCopy(const void* src, void* dst) {
    *(char*)dst = *(const char*)src;
}

static void charZero(void* value) {
    *(char*)value = '\0';
}

static void charPrint(const void* value) {
    char c = *(const char*)value;
    if (c == '\0') {
        printf("_");
    } else {
        printf("%c", c);
    }
}

const FieldInfo* GetCharFieldInfo(void) {
    static FieldInfo* charFields = NULL;

    if (charFields == NULL) {
        charFields = malloc(sizeof(*charFields));
        charFields->size = sizeof(char);
        charFields->typeName = "char";
        charFields->add = NULL;
        charFields->mul = NULL;
        charFields->copy = charCopy;
        charFields->fromDouble = NULL;
        charFields->zero = charZero;
        charFields->print = charPrint;
    }

    return charFields;
}