#ifndef FIELD_INFO_H
#define FIELD_INFO_H

#include <stddef.h>

typedef struct FieldInfo {
    size_t size;
    const char* typeName;

    void (*add)(const void* a, const void* b, void* result);
    void (*mul)(const void* a, const void* b, void* result);
    void (*copy)(const void* src, void* dst);
    void (*fromDouble)(double value, void* result);
    void (*zero)(void* value);
    void (*print)(const void* value);
} FieldInfo;

const FieldInfo* GetIntFieldInfo(void);
const FieldInfo* GetDoubleFieldInfo(void);
const FieldInfo* GetCharFieldInfo(void);

#endif
