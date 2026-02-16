# Aurora

This is an in-memory patcher for the Hytale Client;
 it can do things like enable joining "insecure" servers, or change hytale server auth server;

on windows, hytale (well actually sentry ..)  will try to automatically load "Secur32.dll" 
the only function it imports from it is GetUserNameW which is used to send your windows username
as telemetry back to hytale servers, ( an anti-feature to begin with. )

 .. simply stubbing that with a "return 0;" seems to work pretty well-

on linux ``LD_PRELOAD`` is an environment variable that will signal to the dynamic linker to load this library before every other library; 



# Configuration

This can be configured via Environment Variables before starting the game; 
 .. all of which are prefixed with ``AURORA_`` .. an example of which is :


```

types:
 -> boolean, must be the string "true" to enable, every other string is disabled.
 -> string, can be any string value.

AURORA_ENABLE_CONSOLE
 -> boolean, on windows, this will pop open a command prompt with the game, which can be useful for debugging; on linux this wont do anything.
AURORA_ENABLE_INSECURE_SERVERS
 -> boolean, if enabled then servers with --auth-mode=insecure or --auth-mode=offline can be joined from the client;
AURORA_ENABLE_AUTH_SWAP
 -> boolean, wether to redirect the auth server to somewhere else
AURORA_ENABLE_SINGLEPLAYER_AS_INSECURE = 1
 -> boolean, if enabled, then any singleplayer server started by the user, will have --auth-mode=insecure set. and no auth tokens passed to it.
 
AURORA_HYTALE_COM
 -> string, if "ENABLE_AUTH_SWAP" is enabled, then this is what it will replace the string "hytale.com" with.
AURORA_SESSIONS 
 -> string, if "ENABLE_AUTH_SWAP" is enabled, then this is what it will replace the string "https://sessions." with.
AURORA_ACCOUNT_DATA = "https://account-data.", // what to replace the string "https://account-data." with .
 -> string, if "ENABLE_AUTH_SWAP" is enabled, then this is what it will replace the string "https://account-data." with.
AURORA_TOOLS
 -> string, if "ENABLE_AUTH_SWAP" is enabled, then this is what it will replace the string "https://tools." with.
AURORA_TELEMETRY
 -> string, if "ENABLE_AUTH_SWAP" is enabled, then this is what it will replace the string "https://telemetry." with.

```

IMPORTANT: Due to limitations on how C# strings work, and also with the approach this is using -> any string you replace cannot be longer than the original string;
i.e; AURORA_HYTALE_COM cannot be longer than 10 characters; any string *longer will be truncated to the maximum size* size.
this is why for example the string "sanasol.ws" is exactly 10 characters, same as "hytale.com".


# Usage: 

## Windows
On windows, rename the DLL to "Secur32.dll" and place it in the same folder as the game client;

## Linux
On linux set the environment variable ``LD_PRELOAD`` to the path to the shared-object file 

## MacOS
MacOS is not supported at the moment, but it would most likely be simular to linux except with ``DYLD_INSERT_LIBRARIES`` instead.

# Naming explaination

i was looking in procmon for anything loaded by the game, 
and i noticed it seemed to attempt to open "Aurora.dll" but it always failed;
curious about this i made my own dll named "Aurora.dll" and put it in the game folders--
which didn't seem to do anything-

then i found Secur32.dll, but at that point i had already made a project in VS2026. so the name is now "Aurora"
