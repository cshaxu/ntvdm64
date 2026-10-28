# HASH.EXE

`HASH.EXE <file>` prints that file's SHA-256 in uppercase hexadecimal followed
by a newline. It deliberately does not compare a supplied expected value,
read manifests, or make installation decisions; callers such as `SETUP.CMD`
own those policies.
