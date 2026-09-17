# M0 T420 S9 — SoftPC comms package recovery

## Selected original manifest and disposition

The original `softpc.new/base/comms/sources` manifest selects `com`, `printer`,
`printer_`, `prt_scrn`, and `rs232_io`. All five are now byte-identical to
pinned OpenNT. The only prior difference was the `com_recv_char` cast at
`host_com_read`: it was restored from `UTINY *` to the original `char *`.
The public `host_com.h` declaration and all provider behavior were already
original; this was a removable caller spelling drift, not an adapter.

## Verification

Fresh x86 graph: `build/M0-T420/S9/formal-x86-001`.

- `original-softpc-comms.lib` and the four product programs link; VdmTib has
  one 4,208-byte owner.
- `O:\winnt` SHA-256: `run16=4D9D2C6AB4E15D37F0B3B8F02BEAB9BF7BE819BEF6110018EC40F20838874E43`,
  `basesrv=4D88FBBCA1644B26DFF59419DD91E66B0FCFE93ACE793B31533DD295F4FDE674`,
  `ntvdm=2130F89D0784B6CE77114D5B5A73A431951BF47674B5973FF973D4F499A668CE`,
  `dtmgr=43B5A0ACDF697DE34985F6A7F007A98CB30168800B9A76CB58BAD0880CBA1E7C`.
- Direct MEM, repeated MEM, nested COMMAND→MEM, `COMMAND /C`, EDIT return,
  80x25/45x34/60x50/120x30, resize, mouse, and five short-window EDIT→MEM
  passes all completed.

## Next owner

S10 owns `softpc.new/base/dos` and must retain this exact communications
package rather than introducing a second serial/printer policy.
