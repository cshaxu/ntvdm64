Windows 1.01 NTVDM installer patch

1. Run PATCH\SETUP.CMD from CMD. APPLY.CMD has already preserved the media
   root MOUSE.DRV as MOUSE.DRV.BAK and installed the replacement at the
   original MOUSE.DRV path. SETUP.CMD generates transient launch settings and
   starts original SETUP.EXE through run16.
2. In original Setup choose the real installation directory. Choose Microsoft
   Mouse (Bus/Serial) and the intended display adapter.
3. When Setup returns, enter that actual installation directory when prompted.
   SETUP.CMD verifies WIN.COM, WIN100.BIN and WIN100.OVL, then creates the
   installed PATCH\WIN.CMD and its PIF/configuration files.
4. Run installed PATCH\WIN.CMD. It resolves run16 from PATH.

Media PATCH contains only SETUP.CMD, PIF.EXE, HASH.EXE and SETVER.EXE. Its
transient PIF/configuration files are removed on normal return, including an
original Setup failure.

No guest core image is patched directly. Original Setup is responsible for
assembling WIN100.BIN from the root-level replacement MOUSE.DRV.

To restore prepared media, run the tool package's UNAPPLY.CMD and enter the
same media root. It verifies the replacement before restoring MOUSE.DRV.BAK,
removes only the four verified helper files, and preserves all other PATCH
content.
