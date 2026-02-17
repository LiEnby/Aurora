#include <stdint.h>
#include <stdio.h>

#include "shared.h"
#include "patch.h"
#include "cs_string.h"
#include <stdlib.h>
configuration CFG = {
    .ENABLE_CONSOLE = 0, // on windows, this will pop open a command prompt with the game, which can be useful.
    .ENABLE_INSECURE_SERVERS = 1, // wether to enable joining servers with auth-mode=insecure or auth-mode=offline,
    .ENABLE_SINGLEPLAYER_AS_INSECURE = 1, // force singleplayer games to start in --auth-mode=insecure instead of --auth-mode=authenticated.
    .ENABLE_AUTH_SWAP = 0, // wether to redirect the auth server to somewhere else
    
    .HYTALE_COM = "hytale.com", // what to replace the string "hytale.com" with .
    .SESSIONS = "https://sessions.", // what to replace the string "https://sessions." with .
    .ACCOUNT_DATA = "https://account-data.", // what to replace the string "https://account-data." with .
    .TOOLS = "https://tools.", // what to replace the string "https://tools." with .
    .TELEMETRY = "https://telemetry.", // what to replace the string "https://telemetry." with .

    .SENTRY_URL = "", // disable sentry
};


static int num_swaps = 0;
static int num_codes = 0;

// these are callbacks that will happen whenever a new process is started,
// 
// this is useful to e.g; start the server in "insecure" mode;
// or otherwise change arguments given to the server process when starting the game ..
// 
// despite the name being 'argument' this also gets called for every environment variable too;
// 

// should_modify_argument will contain the program name, 
// return 1 if the program should have arguments or environment variables changed
// if return 0 then it'll be skipped and the program will start as usual.
int should_modify_argument(const char* program) {
    print("[should_modify_argument] %s\n", program);
    if (strstr(program, "java") != 0) return 1;
    else return 0;
}

// actual modification to arguments and environment here
// can check what program it is and also what the argument is
// then you can simply memcpy over it,
// 
// return 0 means it'll discard this argument, 
// return 1 means it'll keep it;
int modify_argument(const char* program, char* arg) {
    print("[modify_argument] checking %s, %s\n", program, arg);
    if (strstr(program, "java") != 0) {

        // we don't want to preload ourselves into java, 
        // doing so will very likely crash the JRE 
        if (strstr(arg, "LD_PRELOAD") != 0) {
            print("Clearing LD_PRELOAD\n");
            return 0; // discard env
        }

        // if enabled then when you create a singleplayer server, 
        // it should be set as 'auth-mode=insecure'
        // instead of auth-mode=authenticated;

        if (CFG.ENABLE_SINGLEPLAYER_AS_INSECURE) {
            // FIX: as of pre-release 10, the --session-token and --identity-token 
            // arguments passed to the server are checked for validity, so we must remove them here
            if (strstr(arg, "--session-token=") != 0) { // versions prior to pre-release 21 will pass
                print("Clearing session token\n");
                return 0; 
            }
            if (strstr(arg, "--identity-token=") != 0) {
                print("Clearing identity token\n");
                return 0; // discard argument
            }

            // FIX: as of pre-release 21, session token and identity token are passed by environment variables
            // "HYTALE_SERVER_IDENTITY_TOKEN" and "HYTALE_SERVER_SESSION_TOKEN" so we remove those too
            if (strstr(arg, "HYTALE_SERVER_IDENTITY_TOKEN") != 0) {
                print("Clearing identity token\n");
                return 0; // discard env
            }
            if (strstr(arg, "HYTALE_SERVER_SESSION_TOKEN") != 0) {
                print("Clearing session token\n");
                return 0; // discard env
            }

            // swap when the client tries to spawn a java process w argument --auth-mode=authenticated, 
            // change it to --auth-mode=insecure.
            if (strstr(arg, "--auth-mode=authenticated") != 0) {
                print("Setting auth-mode to insecure\n");
                strcpy(arg, "--auth-mode=insecure"); // change to auth-mode=insecure ..
                return 1; // keep argument
            }
        }


    }
    return 1;
}

// handle string replacement

void overwrite(csString* old, csString* new) {
    int prev = get_prot(old);

    if (change_prot((uintptr_t)old, get_rw_perms()) == 0) {
        int sz = get_size_ptr(new);

        print("overwriting %p with %p\n", old, new);
        memcpy(old, new, sz);
    }

    change_prot((uintptr_t)old, prev);
}


void enable_dev_servers(uint8_t* mem) {
    int prev = get_prot(mem);
    // the is online mode and is singleplayer checks
    // are almost right next to eachother, it checks one, then checks the other
    // .. 
    //
    // lea     rcx, [rsp+98h+var_70]
    // call    sub_7FF7E036D780
    // cmp     byte ptr [rsp+98h+var_58], 0
    // jz      loc_7FF7DFEAFAE1
    // mov     rax, [rbx+0C8h]
    // mov     rax, [rax+18h]
    // cmp     qword ptr [rax+0B0h], 0
    // jz      loc_7FF7DFEAF93A
    // .. jz instructions always start with 0F 84 .. ..
    // so we can just scan for that
    // im pretty sure id have to change this approach if i ever wanted to support ARM64 MacOS though ..
    // (or if theres ever a 0F 84 in any of the addresses .. hm but thats a chance of 2^16 :D)

    if (change_prot((uintptr_t)mem, get_rw_perms()) == 0) {
        print("nopping is_debug check at %p\n", mem);
        for (; (mem[0] != 0x0F && mem[1] != 0x84); mem++); // locate the jz instruction ...
        memset(mem, 0x90, 0x6); // fill with NOP

        print("nopping is_debug check at %p\n", mem);
        for (; (mem[0] != 0x0F && mem[1] != 0x84); mem++); // locate the next jz instruction ...
        memset(mem, 0x90, 0x6); // fill with NOP
    }


    change_prot((uintptr_t)mem, prev);
}

void try_enable_dev_servers(uint8_t* mem) {
    if (PATTERN_DEVSERVER_CURRENTPLATFORM) {
        enable_dev_servers(mem);
        num_codes++;
    }
}

void try_swap(uint8_t* mem, csString* old, csString* new) {
    if (memcmp(mem, old, get_size_ptr(old)) == 0) {
        overwrite((csString*)mem, new);
        num_swaps++;
    }
}
void try_swaps(uint8_t* mem, swapEntry* swaps, int total_swaps) {
    for (int sw = 0; sw < total_swaps; sw++) 
        try_swap(mem, &swaps[sw].old, &swaps[sw].new);
}


// after initalizing everything, Aurora will call into this function;
// here we actually define things like what strings to swap for other strings;
// and what features to enable, etc;

void entry() {
    // if console is enabled then open the console .. 
    if (CFG.ENABLE_CONSOLE) {
        create_console();
    }

    // define all the values to swap around ..
    swapEntry swaps[] = {
        {.old = make_csstr(L"https://account-data."),                                         .new = make_csstr_ansi(CFG.ACCOUNT_DATA)},
        {.old = make_csstr(L"https://sessions."),                                             .new = make_csstr_ansi(CFG.SESSIONS)},
        {.old = make_csstr(L"https://tools."),                                                .new = make_csstr_ansi(CFG.TOOLS)},
        {.old = make_csstr(L"https://telemetry."),                                            .new = make_csstr_ansi(CFG.TELEMETRY)},
        {.old = make_csstr(L"hytale.com"),                                                    .new = make_csstr_ansi(CFG.HYTALE_COM)},
        {.old = make_csstr(L"https://ca900df42fcf57d4dd8401a86ddd7da2@sentry.hytale.com/2"),  .new = make_csstr_ansi(CFG.SENTRY_URL)},
    };

    int total_swaps = (sizeof(swaps) / sizeof(swapEntry));
    int total_codes = 1; // only 1 for now, enable dev servers patch .. 

    // sanity check :
    // 
    // make sure our swaps are always smaller than or the same size as, the original string
    // you can get away with this not being the case on windows as theres extra space, but not on Linux or Mac!
    for (int i = 0; i < total_swaps; i++) {
        assert(get_size(swaps[i].new) <= get_size(swaps[i].old));
    }
    
    // get module base address;
    modinfo modinf = get_base();
    uint8_t* mem = modinf.start;

    // scan over the games entire memory space-
    //  if matches a code patch pattern then 
    //      -> apply code patch
    //  if matches a csharp string in the swap list
    //      -> swap string for new string

    for (size_t i = 0; i < modinf.sz; i++) {
        if (CFG.ENABLE_INSECURE_SERVERS) try_enable_dev_servers(&mem[i]);
        if (CFG.ENABLE_AUTH_SWAP) try_swaps(&mem[i], swaps, total_swaps);
  
        if ((!CFG.ENABLE_AUTH_SWAP || num_swaps >= total_swaps) && (!CFG.ENABLE_INSECURE_SERVERS || num_codes >= total_codes)) break;
    }
}

