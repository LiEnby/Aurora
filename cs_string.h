#ifndef _CS_STRING
#define _CS_STRING 1
#include <stdint.h>
#include <assert.h>
#include <wchar.h>
#include <string.h>

#include "shared.h"
#include "compat.h"

typedef struct PACK(csString {
    uint32_t stringSz;
    wchar_t stringData[70];
}) csString;

size_t wchar_strlen(const wchar_t* str);
wchar_t* make_wstring(const char* str);

csString make_csstr(wchar_t* str);
csString make_csstr_ansi(char* str);
#define get_size(cstr) ((sizeof(uint32_t) + ((cstr.stringSz) * sizeof(wchar_t)))-1)
#define get_size_ptr(cstr) ((sizeof(uint32_t) + ((cstr->stringSz) * sizeof(wchar_t)))-1)


#endif
