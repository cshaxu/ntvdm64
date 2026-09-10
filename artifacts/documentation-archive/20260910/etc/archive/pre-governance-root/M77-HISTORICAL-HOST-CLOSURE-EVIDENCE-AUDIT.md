# M77 Historical Host Closure Evidence Audit

Status: fixed-source host-object reconciliation. Its former execution-backend
conclusion is superseded by `SOFTPC-RECONSTRUCTION-GOVERNANCE.md`, 2026-08-08.

> Governance correction: the closure facts below remain evidence, but the
> conclusion that CCPU is merely an oracle and an external backend is required
> is retired. The active route repairs and composes the OpenNT SoftPC/CCPU
> software monitor, while continuing to exclude the unavailable kernel V86
> route.

## Scope

This audit classifies the host-side material required to understand the NT4
guest contract. It does not claim a historical x86 executable, V86 support,
or a runnable `nvtdm.exe`.

## Retained Build Evidence

`src/opennt/CMakeLists.txt` keeps isolated, default-disabled object targets
for the original BIOS table, `nt_bop`, Sim32 mapping, DEM, and the supporting
provider groups. The aggregate
`ntdos64-opennt-historical-provider-archive` was rebuilt twice with clang-cl
x86/NMake; M59 records the fresh configuration archive at 2,329,118 bytes,
SHA-256 `CE6C02905636EDE24B03D111F1664CD33B3E107A0556DD42F41A73FF22EB998B`.

The archive is source/object closure evidence only. It deliberately excludes
the frozen local CCPU register bridge and keeps all trace fixtures
`EXCLUDE_FROM_ALL`; no normal runner or default CLI links the archive.

| Original responsibility | Current evidence | Runtime disposition |
| --- | --- | --- |
| BIOS selector table | original `bios.c` object is retained and compiled | behavior oracle; profile adapters must not replace it with a reduced table |
| NT BOP transport | original-source overlay `nt_bop.c` object retained and compiled | oracle for service-byte read, dispatch ordering, and IP advance |
| guest mapping helper | original `sim32.c` compiles in its required historical role; bounded pointer fixture executes | oracle only; no raw guest pointer crosses future ABI |
| DEM SVC owners | full selected DEM group compiles; original date/lock paths have bounded tests | preserve service ownership knowledge; wrapper still needs contained capabilities |
| machine/device providers | BIOS/system/XMS/input/disk/comms/video provider objects build into archive | static reachability evidence, not an admission for those live services |

## Executed Oracle Evidence

The default-disabled selector-50 trace reaches:

```text
historical CCPU fixture -> complete BIOS[50h] -> MS_bop_0
  -> DemDispatch -> demQueryDate -> BOP FE stop
```

It proves the original service-byte ordering and post-dispatch IP behavior for
one bounded path. It does not demonstrate NTIO entry, NTDOS load, COMMAND,
host filesystem policy, or a usable CPU backend. The CCPU fixture is retained
only because it is a convenient historical conformance oracle.

## Superseded Execution Conclusion

M57 establishes that the NT4 x86 build selects the V86 `MONITOR` profile;
CCPU/`CPU_40_STYLE` is a non-x86 profile. The research target excludes that
kernel V86 route. The retained full-source closure also
keeps old console/session, native RTL, event, VDD, device, and generated ABI
imports deliberately unresolved when their historical owner is not required by
the bounded trace.

The retired conclusion was that an external backend must replace this path.
The active conclusion is narrower: make this historical archive executable by
repairing only evidenced missing SoftPC/CCPU semantics and by supplying modern
host shims at live original callers. All legacy process, console, filesystem,
timing, device, and VDD imports stay out of the first non-invasive CLI unless
a live bounded trace reaches their original caller and a specific
host-capability replacement is admitted.

## Documentation Correction

`R1-ORIGINAL-CHAIN-CLOSURE-LEDGER.md` retains useful historical diagnostics,
but its older language describing direct CCPU as an executable-profile
candidate is superseded by M57, M58, M75, and this audit. Its bounded CCPU
results remain trace evidence only.
