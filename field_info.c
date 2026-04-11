#include "field_info.h"

#include <stdio.h>

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
const FieldInfo* GetIntFieldInfo(void) {
    static FieldInfo info;
    static int initialized = 0;

    if (!initialized) {
        info.size = sizeof(int);
        info.typeName = "int";
        info.add = intAdd;
        info.mul = intMul;
        info.copy = intCopy;
        info.fromDouble = intFromDouble;
        info.zero = intZero;
        info.print = intPrint;
        initialized = 1;
    }

    return &info;
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
    static FieldInfo info;
    static int initialized = 0;

    if (!initialized) {
        info.size = sizeof(double);
        info.typeName = "double";
        info.add = doubleAdd;
        info.mul = doubleMul;
        info.copy = doubleCopy;
        info.fromDouble = doubleFromDouble;
        info.zero = doubleZero;
        info.print = doublePrint;
        initialized = 1;
    }

    return &info;
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
    static FieldInfo info;
    static int initialized = 0;

    if (!initialized) {
        info.size = sizeof(char);
        info.typeName = "char";
        info.add = NULL;
        info.mul = NULL;
        info.copy = charCopy;
        info.fromDouble = NULL;
        info.zero = charZero;
        info.print = charPrint;
        initialized = 1;
    }

    return &info;
}
