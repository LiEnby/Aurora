#include "cs_string.h"
#include <stdlib.h>

size_t wchar_strlen(const wchar_t* str) {
    size_t len = 0;
    do {
        len++;
    } while (*str++ != L'\0');
    return len - 1;
}

wchar_t* make_wstring(const char* str) {
    size_t sz = strlen(str);
    
    sz = (sz + sizeof(char)) * 2;
    wchar_t* wstr = malloc(sz);
    memset(wstr, 0x00, sz);

    for (int i = 0; str[i] != 0; i++) {
        wstr[i] = (wchar_t)(str[i]);
    }

    return wstr;
}

csString make_csstr(wchar_t* str) {
    csString cstr = { 0 };
    cstr.stringSz = (uint32_t)wchar_strlen(str);
    memcpy(cstr.stringData, str, wchar_strlen(str)*sizeof(wchar_t));
    return cstr;
}

csString make_csstr_ansi(char* str) {
    csString cstr = { 0 };

    wchar_t* wstr = make_wstring(str);

    cstr.stringSz = (uint32_t)wchar_strlen(wstr);
    memcpy(cstr.stringData, wstr, wchar_strlen(wstr) * sizeof(wchar_t));

    free(wstr);

    return cstr;
}
