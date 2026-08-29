# M0 T310 S8 P4 — original RtlFillMemoryUlong call-contract closure

## Scope

- Mirror source: `src/mvdm-host/softpc.new/host/src/copy_fnc.c`.
- Warning records: `T310-S8-P4-CLASS-018839` / `WARN-037071, WARN-037072`.
- Scope is limited to the original SoftPC host buffer-fill import; it does not alter guest addressing, mapping-manager ownership, controller behavior or warning policy.

## Original contract and correction

`memset4` retains the original call order and byte-count calculation:
`RtlFillMemoryUlong(laddr, count << 2, data)`. The selected modern header set
did not publish this historical NTDLL declaration, causing MSVC to assume an
`int` return on both architectures.

`MVDM-HOST-DIV-081` adds the exact local call declaration:

```c
VOID NTAPI RtlFillMemoryUlong(PVOID Destination, ULONG Length, ULONG Pattern);
```

The target buffer is a private SoftPC host backing buffer. It is not a guest
numerical address and does not expose a host resource identity, so a session
mapping-manager route would be incorrect. No implementation body, provider
selection, pointer-token conversion or warning suppression was added.

## Verification

- Formal MSVC `/MT` x64 Ninja graph rebuilt `copy_fnc.c` and archived
  `original-softpc-host-roots.lib`; the prior `C4013` is absent.
- Formal MSVC `/MT` x86 Ninja graph rebuilt the same original source and
  archive; the same prior `C4013` is absent.
- Remaining `C4005` command-line macro redefinition and historical archive
  duplicate-symbol `LNK4006` diagnostics remain visible and are outside this
  call-contract repair.

## Disposition

`T310-S8-P4-CLASS-018839` is closed as a source-shaped historical NTDLL
declaration recovery. The baseline classification record remains historical
scan evidence; it is not a claim that the old diagnostic remains emitted by
the repaired graph.
