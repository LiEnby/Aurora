#ifndef _SHARED_H
#define _SHARED_H 1
#include <stdint.h>
#include <wchar.h>
#include <assert.h>

static_assert(sizeof(wchar_t) == 2, "sizeof wchar is not 2 byte; try using -fshort-wchar");

typedef struct modinfo {
	uint8_t* start;
	size_t sz;
} modinfo;


int get_prot(void* addr);
int change_prot(uintptr_t addr, int newProt);
modinfo get_base();
int get_rw_perms();
void create_console();

// linux:  48 8D ?? ?? E8 ?? ?? ?? 00 80 ?? ?? 00 0F 84
// windows: 48 8D ?? ?? ?? E8 ?? ?? ?? ?? 80 ?? ?? ?? 00 0F 84
#define PATTERN_DEVSERVER_LINUX (mem[0] == 0x48 && mem[1] == 0x8D && mem[4] == 0xE8 && mem[8] == 0x00 && mem[9] == 0x80 && mem[12] == 0x00 && mem[13] == 0x0F && mem[14] == 0x84)
#define PATTERN_DEVSERVER_WINDOWS (mem[0] == 0x48 && mem[1] == 0x8D && mem[5] == 0xE8 && mem[10] == 0x80 && mem[14] == 0x00 && mem[15] == 0x0F && mem[16] == 0x84)

#ifdef __linux__
#define PATTERN_DEVSERVER_CURRENTPLATFORM PATTERN_DEVSERVER_LINUX
#elif _WIN32
#define PATTERN_DEVSERVER_CURRENTPLATFORM PATTERN_DEVSERVER_WINDOWS
#endif

#endif