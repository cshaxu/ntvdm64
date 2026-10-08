Windows 1.01 NTVDM installer patch

1. Run PATCH\SETUP.CMD from CMD. It creates a private temporary copy of the
   original flat media, replaces only that copy's MOUSE.DRV, generates its
   PIF/profile, and starts original SETUP.EXE through run16.
2. In original Setup choose the real installation directory. Choose Microsoft
   Mouse (Bus/Serial) and the intended display adapter.
3. When Setup returns, enter that actual installation directory when prompted.
   SETUP.CMD verifies WIN.COM, WIN100.BIN and WIN100.OVL, then creates the
   installed PATCH\WIN.CMD and its PIF/configuration files.
4. Run installed PATCH\WIN.CMD. It resolves run16 from PATH.

The private PATCH\TEMP directory is removed on normal return, including an
original Setup failure. If the host forcibly terminates SETUP.CMD, inspect or
remove the leftover TEMP directory manually before retrying.

No guest core image is patched directly. Original Setup is responsible for
assembling WIN100.BIN from the supplied temporary MOUSE.DRV.
