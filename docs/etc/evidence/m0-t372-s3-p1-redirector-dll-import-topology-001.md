# M0 T372 S3 P1 — Original Redirector DLL import topology

## Procedure

The current formal CPU40/x86 graph first built the complete original
`original-mvdm-redir.lib` using its selected `vdmredir/sources` manifest:

```text
build\M0-T371\S2\formal-x86\run-ninja-parallel.cmd original-mvdm-redir.lib
```

All fifteen original provider translation units compiled and the library was
created. A disposable whole-archive DLL probe then linked the library against
public system libraries including `netapi32.lib`, with `/force:unresolved`.
The output is diagnostic-only and is not a product or runtime input.

## Result

The probe has 66 unique unresolved symbols from eleven original provider
objects: `vrdisp`, `vrdlc5c`, `vrdlcbuf`, `vrdlcpst`, `vrinit`, `vrmisc`,
`vrmslot`, `vrnetapi`, `vrnetb`, `vrnmpipe` and `vrremote`.

They form one original host-import surface, not 66 provider rewrites:

- **SoftPC/CCPU state:** `Sim32pGetVDMPointer`; `get*`/`set*` register and
  flag accessors; `getMSW`; `WaitIfIdle`; `call_ica_hw_interrupt`; and
  `SoftPcEoi`. These are historical `ntvdm.lib` machine imports and must be
  served by the existing `adapter-mvdm-host-out/softpc` interface shape.
- **Existing session/Redirector seams:** the six async functions, three
  mailslot mapping functions, host-resource handle conversion, bounded guest
  copies, and `opennt_create_cdecl_thread_named`. These are already named
  source-shaped adapter/session boundaries; no duplicate mapper is permitted.
- **Public modern capability:** standard NetAPI imports resolved once
  `netapi32.lib` entered the probe. Their use remains owned by the original
  `vrnetapi`/`vrremote` bodies, not by a replacement NetAPI provider.
- **Historical non-public or later-group functions:** `RxpTransactSmb`,
  `XsNet*`, `Netp*`, `SystemFunction*`, `GetLanmanSessionKey`, NetBIOS list
  helpers, `VDDInstallUserHook`, and DLC/physical-completion helpers. These
  are deliberately retained for S4/S5 group disposition; they cannot be
  faked merely to make a DLL link.

## Architectural consequence

The original `VDMREDIR` DLL depended on the parent NTVDM process through
`ntvdm.lib`. Linking all SoftPC libraries into the DLL would produce duplicate
machine/global state and is rejected. Statically putting Redirector bodies in
`nt_bop.c` would also violate `LoadVdmRedir` availability semantics.

S3 is therefore expanded within its existing owner package to first construct
the single same-shaped host-import ABI: original register/memory/ICA calls
resolve through `adapter-mvdm-host-out/softpc`; existing session adapters
remain imports; and later provider groups retain explicit import/failure
dispositions. This is a global package boundary discovered before any leaf
service is enabled, not a trace-derived repair.

No production source was changed by P1. The disposable probe and its logs are
not retained as a build or product artifact.
