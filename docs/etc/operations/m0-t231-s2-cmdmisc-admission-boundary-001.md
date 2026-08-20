# M0 T231 S2 cmdmisc admission boundary

`cmdComSpec` (`54:02`) and `cmdGetCurrentDir` (`54:04`) are original
functions in `cmdmisc.c`, but that translation unit also owns command queue,
context registration, console and startup services.  T231 does not create a
parallel handler file.  The port must compile the original file through a
shim that supplies only the actually selected function's CCPU-style register
and guest-memory contract, and must leave unrelated services unregistered.

For `54:02`, the retained source algorithm is: first-call gate; obtain the
guest `DS:DX` COMSPEC string; compose `COMSPEC=` in original shared storage;
set the original AL console/WOW indication.  The shim owns checked C-string
input and a fixed guest register/result record.

For `54:04`, the retained source algorithm is: obtain guest `DS:SI`, select
the requested DOS drive from AL, query its physical/host type, look up or
initialize the original `=X:` current-directory environment value, and set
the original CF/AX outcomes.  The shim owns only checked guest output and
ordinary Win32 directory/environment capability.

The initial S2 compile experiment must therefore prove that unselected
`cmdmisc.c` functions cannot be reached through `CmdDispatch`; it must not
turn them into no-op stubs or inherit a v1 dispatcher.
