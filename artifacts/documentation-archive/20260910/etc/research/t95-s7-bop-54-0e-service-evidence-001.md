# T95 S7 BOP 54:0E Fixed Keyboard-Layout Service Evidence

## Question

Can the first contained en-US COMMAND profile resume the reached `BOP_CMD:SVC_GETKBDLAYOUT` call without adding keyboard hardware, KEYB/KB16, host locale state, or guest-memory side effects?

## Inputs

The reached instruction is `C4 C4 54 0E` at real-mode `95AB:05D9` in the r44 trace. `src/opennt/base/mvdm/dos/command/tcode.asm:721-757` branches only on returned `DX`: zero skips `run_keyb`; nonzero starts KB16. The historical host owner in `src/opennt/base/mvdm/dos/command/cmdkeyb.c:29-242` returns `DX=0` when its console/locale lookup does not produce a KB16 install request.

## Procedure

`bx_ntvdm_cmd_keyboard_layout_v1_dispatch` accepts only a valid real-mode vector-6 event and copied four-byte instruction window. It produces a normal resume at `fault_rip + 4` and a CPU delta that sets only low `DX` to zero. It neither reads nor writes guest memory and ignores the original scratch `DS:SI`/`DS:CX` inputs.

The focused fixture was configured and run in fresh root `artifacts/build/t95-s7-kbdlayout-c11-001`:

```text
bx-ntvdm keyboard-layout: fixed EN-US no-KB16 response verified
bx-ntvdm-adapter-runtime-test: identity-to-pending-transaction path verified
```

## Observation

The service produces the source-defined no-install outcome (`DX=0`) with no guest-memory range, host capability, Bochs device, or OpenNT-source change. The existing generic BOP observation remains responsible for recording the call; this handler is a single admitted response, not a dispatcher.

## Interpretation and Confidence

High confidence for the contained fixed en-US/no-KB16 profile: COMMAND's direct branch condition and the historical owner both identify zero `DX` as the no-install path. This does not recreate `cmdInitConsole`, infer a host keyboard layout, or support a profile that requests KB16.

## Follow-up

Generate one fresh native r45 closure containing this one adapter source, run its declared MSVC/x86 target once, and take one watchdog-bounded trace with the same source-built `QUIT.COM` and nonempty `/q` tail used by r44 trace 037. Classify the next reached BOP before admitting any new service or machine feature.
