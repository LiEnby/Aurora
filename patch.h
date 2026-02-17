#ifndef _PATCH_H
#define _PATCH_H

#include "cs_string.h"

typedef struct swapEntry {
	csString new;
	csString old;
} swapEntry;

typedef struct configuration {
	// general settings ...
	int ENABLE_CONSOLE; // on windows, this will open a command prompt with the game
	int ENABLE_INSECURE_SERVERS; // this will allow joining servers with --auth-mode=insecure or --auth-mode=offline set.
	int ENABLE_SINGLEPLAYER_AS_INSECURE; // this will force singleplayer servers to use --auth-mode=insecure,
	int ENABLE_AUTH_SWAP; // this will enable redirecting the auth server to another location.
	
	// auth server config ... 
	char HYTALE_COM[11]; // hytale.com
	char SESSIONS[18]; // https://sessions.
	char ACCOUNT_DATA[22]; // https://account-data.
	char TOOLS[15]; // https://tools.
	char TELEMETRY[19]; // https://telemetry.

	// sentry cfg
	char SENTRY_URL[61]; // https://ca900df42fcf57d4dd8401a86ddd7da2@sentry.hytale.com/2
} configuration;
extern configuration CFG;

void entry();
int should_modify_argument(const char* program);
int modify_argument(const char* program, char* arg);

#endif