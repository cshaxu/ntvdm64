# T231 S5 OpenNT `cmdGetStartInfo` direct witness

## Question

Can the original `cmdmisc.c` `54:10` body return the historical
`DosSessionId` predicate through the typed COMMAND boundary without adding a
new COMMAND policy or exposing host state to the guest?

## Inputs and procedure

The admitted source is the mirrored OpenNT translation unit
`src/bx-vdm/bop/opennt/command/cmdmisc.c`, original owner
`base/mvdm/dos/command/cmdmisc.c`.  The body remains exactly:

```c
setAL((BYTE) (DosSessionId ? 1 : 0));
```

The only composition delta is the existing persistent COMMAND session.  It
owns a fixed-width `dos_session_id`; the scoped shim assigns the imported
`DosSessionId` global immediately before the original body, and maps `setAL`
to the existing typed CPU-result delta.  The selector/service ingress remains
outside this source and the guest never receives a host pointer.

The focused MSVC x64 `/W4 /WX /MT` fixture compiled and ran from a disposable
`build/M0-T231-S5/command-start-info-r2/` directory.  It invoked the normal
real-mode copied-event boundary twice, preserving AX high byte in both cases:

- session `dos_session_id = 0` yielded AX `0x5500`;
- session `dos_session_id = 1` yielded AX `0x5501`.

## Observation and interpretation

The local result resumed at the checked four-byte BOP continuation and used
only the original `DosSessionId ? 1 : 0` rule.  This is local Direct source
parity for `54:10`, not a claim that the later `54:01` launch lifecycle creates
or owns a DOS session.

## Follow-up

S5 continues with the original `cmdconf.c` configuration (`54:0C`, `54:0D`)
and `cmdenv.c` initialization-environment (`54:0F`) owners.  Any unavailable
historical PIF, OEM, temporary-file, or terminal-VDM dependency stays in a
named shim and must preserve the original failure path.
