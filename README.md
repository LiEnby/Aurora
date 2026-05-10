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
 -> string, can be any ANSI string value.

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
AURORA_SENTRY_URL
 -> string, if "ENABLE_AUTH_SWAP" is enabled, then this what it will replace the string "https://ca900df42fcf57d4dd8401a86ddd7da2@sentry.hytale.com/2" with.
```

IMPORTANT: Due to limitations on how C# strings work, and also with the approach this is using -> any string you replace cannot be longer than the original string;
i.e; AURORA_HYTALE_COM cannot be longer than 10 characters; any string *longer will be truncated to the maximum size* size.
this is why for example the string "sanasol.ws" is exactly 10 characters, same as "hytale.com";


# Examples 
= sanasol auth server =


```
AURORA_ENABLE_AUTH_SWAP=true 
AURORA_HYTALE_COM=sanasol.ws
```


= offline servers on offical game =

```
AURORA_ENABLE_INSECURE_SERVERS=true 
AURORA_ENABLE_SINGLEPLAYER_AS_INSECURE=false
AURORA_ENABLE_AUTH_SWAP=false
```


= localhost auth server =
```
AURORA_ENABLE_INSECURE_SERVERS=true 
AURORA_ENABLE_AUTH_SWAP=true 
AURORA_ENABLE_SINGLEPLAYER_AS_INSECURE=true
AURORA_SESSIONS=http://127.0.0
AURORA_ACCOUNT_DATA=http://127.0.0
AURORA_TOOLS=http://127.0.0
AURORA_TELEMETRY=http://127.0.0
AURORA_HYTALE_COM=.1:59313 
AURORA_SENTRY_URL=http://key@127.0.0.1/2
```


-> this works because despite `http://127.0.0.1:59313` being too long (>10 chars) normally.

 .. because the subdomain is stored seperately (i.e `https://sessions.`) and appended to the `hytale.com` domain; 
 
 .. then will result (after processing) as the URL .. `http://127.0.0.1:59313`
 
 .. .. using this method you can get a maximum length of (24) characters, 
  
  as thats the size of the smallest subdomain `https://tools.` (its used for bug reports iirc;)


= disable telemetry =
```
AURORA_ENABLE_AUTH_SWAP=true
AURORA_TELEMETRY=http://a/a?=
AURORA_SENTRY_URL=http://a@a/2
```

-> this works because it this sets the telemetry 'subdomain', to `http://a/a?=`, 

  .. the domain 'hytale.com' will be appended to this as so the resulting url would be: `http://a/a?=hytale.com//telemetry/client` 
  
  .. the domain for this url is now `a` which is not going to resolve to any host, and `hytale.com/telemetry/client` is now the query part, 
  
-> sentry (a telemetry suite) expects a public key, as username part, and then a version as path;
  
  .. `http://a@a/2` has a username of `a`, on the domain `a` with version 2, which, is a valid URL but not going to resolve to any host.


# Usage: 

## Windows
On windows, rename the DLL to ``Secur32.dll`` and place it in the same folder as the game client;

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
