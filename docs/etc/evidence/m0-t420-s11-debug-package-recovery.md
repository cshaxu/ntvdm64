# M0 T420 S11 — SoftPC debug package recovery

The original `softpc.new/base/debug/sources` manifest selects `debuggng`,
`ega_dump`, `btrace`, `trace`, `profile`, `pigyoda`, `yoda`, and `dasm`.
The generator previously selected only `trace.c`; it now selects all eight
original units in `original-softpc-base-trace.lib`. Seven were byte-exact
OpenNT already. `trace.c` had only two bare-LF formatting differences and is
now exact CRLF OpenNT formatting. No debug algorithm, overlay, or autonomous
provider was added.

All eight units compile with the selected Win32/x86 CCPU40 flags and form the
6,484-byte original debug library. Reusing unchanged S10 objects, the four
products were freshly relinked; VdmTib verification passed with one 4,208-byte
owner. Deployed hashes are `run16=C177D2E13F1A5FAB6B568BC4CE6151E78139392714A7ED426B6736C3E4A1F0D9`,
`basesrv=528E701C70A39CD5928300854D4953632FA70183BCFD1F1E9AA2F5ADE3322A2E`,
`ntvdm=7C8732948DDA2742DC183449C2FAC3199941D82BBF9814BDC973AD2136EEA747`,
and `dtmgr=B16FE72073C86CD9BCAA336F1B33C8B412D3DDF8BB89CC103FB8314766F12B8D`.

Direct MEM, repeated MEM, nested COMMAND→MEM, `COMMAND /C`, and EDIT return
passed. A first C-VID runner ended after a correct MEM screen with transient
broker RPC 1722 during cleanup; a fresh complete retry passed all four
geometries, resize, mouse, and five short-window EDIT→MEM observations.

S12 owns original `softpc.new/host/src` worker startup and lifecycle.
