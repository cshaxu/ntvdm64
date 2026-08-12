# T95 S7 BOP 54:0E Keyboard Layout Audit

`BOP_CMD:0E` is `SVC_GETKBDLAYOUT` (`cmdsvc.h:29`). COMMAND first queries
INT 2Fh for existing KB16 state, supplies `DS:SI` and `DS:CX` scratch buffers,
then calls the BOP. It branches solely on returned DX: nonzero executes
KB16.COM; zero jumps to `End_keyb` (`tcode.asm:721-757`).

The original `cmdGetKbdLayout` documents DX=0 as no-install failure and DX=1
as a request to write an executable path/options. Its DX=0 path is reached
when the console layout is US and KB16 is not installed, among many other
host-specific conditions (`cmdkeyb.c:29-109, 157-176, 239-242`).

The contained en-US CLI profile fixes that source-defined no-install outcome:
the adapter verifies only real-mode `C4 C4 54 0E`, resumes after four bytes,
and replaces low DX with zero. It must not inspect DS:SI/CX, access guest
memory, read a registry/console locale/SystemRoot, load KB16/KEYB.COM or
KEYBOARD.SYS, or change a Bochs keyboard device.
