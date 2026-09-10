# T95 S6 r12 BOP listener trace 001

## Scope and immutable inputs

This is the first runtime observation with the default-off `BX-ABI-040` BOP
catalogue listener explicitly enabled.  It is an observation record, not a
BOP implementation admission.  The listener's only output is a log identity;
the existing `50:11` startup transaction remains the separately admitted
one-shot write path.

The executable was linked in
`artifacts/build/bochs-2.6-native-startup-transaction-msvc-r12-bop-listener`.
Its build-root record declares exactly 19 objects and these four opt-ins:

```text
BX_NTVDM_ENABLE_EXECUTION_PLAN=1
BX_NTVDM_ENABLE_STARTUP_TRANSACTION=1
BX_NTVDM_ENABLE_UNMATCHED_UD_DIAGNOSTIC=1
BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER=1
```

The one retained observation is
`artifacts/analysis/t95-s6-r12-bop-listener-010-20260811-001/observation.json`.
It pins NTIO, NTDOS, COMMAND, firmware, profile and executable hashes, and
retains the complete `bochs.log`.  The runner used the unchanged real-NTIO
input profile with a 30-second watchdog.  The process was watchdog-terminated
after 33.645 seconds; that is a retained failure-to-stop outcome, not a
runnable DOS claim.

## Observed causal prefix

The relevant log lines, in order, are:

```text
127i  BOP selector=50 class=2 family=1 service-state=1 service=11
127i  guest transaction committed cs=0070 eip=00000475 address=8b00 bytes=6cd2 resume=479
143i  BOP selector=12 class=1 family=0 service-state=0 service=00
143i  unmatched-ud cpu=0 cs=010d rip=00000516
```

`50:11` is the already admitted OpenNT DOS-family NTDOS-load transaction.  The
listener precedes its mechanical commit and does not choose that commit.

`12h` is the first subsequent source-correlated BIOS-style BOP.  The source
path is `base/mvdm/dos/v86/doskrnl/ntio/sysinit1.asm` followed by the original
SoftPC table `base/mvdm/softpc.new/base/bios/bios.c`, where `BIOS[0x12]` is
`memory_size`.  The original implementation loads BDA `0040:0013` and places
that conventional-memory KiB value in AX.  The independent source/machine
contract is recorded in `bop-12-memory-size-contract-r20.md`.

The listener identifies `12h` as class 1 (SoftPC BIOS), not a service family,
and it deliberately returns no execution result.  Consequently the existing
unmatched-#UD path logs the exact same `010D:0516` boundary.  This proves that
unimplemented BOPs remain fail-closed rather than silently advancing RIP.

Later repeated `50:11` records occur at `0000:0b75` after the unmatched BOP
has already diverted control.  They are not a second successful NTDOS load,
and this record makes no semantic claim about them.  The later x87 diagnostic
and repeated unmatched #UD records are likewise downstream of that first
unimplemented `12h` boundary.

## Result and next admission

The passive listener has runtime proof for the reached BOPs and the inventory
remains the complete definition authority at
`artifacts/analysis/opennt-bop-inventory-005-20260811-001/` (JSON SHA-256
`893EB8D98A476A4D8A7AE996635B2B2BD24706594DE3666C2E664A1FB97E5C28`).

The next candidate is not a generic dispatcher.  It is one isolated `12h`
CPU-result bridge: validate the exact copied real-mode boundary, consume the
profile-proven `0x027f` value, change AX only, and resume at `fault_rip + 3`.
It requires a separately registered generic typed-result Bochs consumer and a
fresh closure; no PIC, FPU, device, filesystem, DOS service, or host-drive
capability is implied.
