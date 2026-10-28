# Original BIOS integration and real dispatch witness

This S10 checkpoint follows `e0b7e8a42`. Seven pending original BIOS files
become **partial**, not full. The [per-file dispositions](bios-native-role-dispositions.csv)
bind exact primary-OpenNT paths, hashes, retained code and finite remaining
contracts. No mirror, runtime image or original guest is modified.

## Source, composition and actual image

The [observer](bios-image-observer.py) verifies the real worker compile edges,
archive membership, recorded VALID dependency blocks and object hashes. It
binds the map to identical complete executable sections in the sealed
candidate and the cached map-producing worker. Package identity resources
may differ; whole-file equality is not assumed. For 52 mapped functions it
compares all non-relocated object bytes at the actual mapped image address.
That comparison does not validate every function's call relocation.

Separately, every pointer in the original **256-entry BIOS** and **30-entry
EMS** vectors is checked against its real COFF DIR32 relocation, addend,
selected definition and actual image value. Static definitions are resolved
at their own object owner. The [observations](bios-image-observations.json)
retain the individual functions, pointers, source and image identities.
Image-defined base/NT control tables contain only their empty sentinels;
zero-filled PE storage is interpreted by the loader's normal zero-fill rule.
This is initial-image evidence, not a claim that writable state never changes.

The [negative observer](bios-image-negative.py) changes only BIOS slot FF in
a disposable copy of the project host EXE to the illegal-BOP address. It
never executes that copy. Executable bytes still match, but the actual
vector-target check rejects exactly `(255, '_control_bop')`. Its
[result](bios-image-negative-result.json) records both hashes and the changed
slot. Original packages and all guest bytes remain unchanged.

| File | Mapped functions | Retained original owner and unproved boundary |
| --- | ---: | --- |
| bios.c | 10 | The whole BIOS initializer and virtualized mouse wrappers are retained. New guest calls exercise control, illegal and invalid EMS entries, not every BOP provider, mouse instance swap, nested callback or teardown. |
| cmos_bis.c | 2 | cmos_read/write preserve original port order and index/NMI restoration. Actual callers, all addresses/NMI combinations, RTC effects and reset/failure semantics remain S13 obligations. Non-NT set_tod is not the selected NT tick owner. |
| cntlbop.c | 2 | Original generic/NT lookup and wrong-host errors are exercised. Empty tables mean explicit ERR_NO_FUNCTION, not invented successful services. The wrong-host guest case uses AH=2; other AH values and optional nonselected host callbacks are not dynamically accepted. |
| emm_fncs.c | 33 | Initialization, 30 function slots, return-from-call and reset stay original. Only four invalid-function returns are newly executed. Valid allocations, handles, map save/restore, reallocation, buffer bounds, move/exchange, alternate maps, access keys, call/jump unwind and failure/retirement remain S12 obligations. |
| ill_bop.c | 1 | Original PROD intentionally returns without mutation. Sixty-four guest calls exercise that profile; non-PROD environment/trace/YODA branches are not thereby accepted. This is not a project success stub. |
| rom_basc.c | 1 | Original unsupported-ROM-BASIC host_error call is linked. Reporting, continue/error and retirement outcomes remain unverified; no BASIC interpreter or successful ROM BASIC execution is claimed. |
| slave_bs.c | 3 | Original unexpected-interrupt, IRQ9 redirect and x87 IRQ acknowledgement bodies are linked. The existing D11 slave-port finding requires real PIC/guest tests before a correction; no new test here exercises these routines. |

All seven originals retain **100 percent original text**. The 52 mapped
functions and 286 bound pointers are composition measurements, not a
restored-behavior percentage. No defensible whole-file behavior/code-coverage
ratio follows from the selected guest cases; each partial row explicitly
records that unmeasurable basis and its remaining lifecycle/fault work.
No file becomes full and no capability is excluded by a currently unset
optional build switch. The selected profile is NTVDM/CPU40/CCPU/SPC386/PROD;
host_def.h enables LIM for this composition.

## Real guest procedure and assertions

The independently authored [COM probe](../../tests/observation/bios_dispatch_probe.asm)
runs through the retained CCPU `bop(n)` -> `BIOS[n]` route. It does not change
COMMAND, NTIO, NTDOS or any original guest, and does not substitute a host
provider. The proven medium-integrity modern-pwsh runner creates a short
physical runtime stage and private desktop, preserving package isolation.
There is no Computer Use, UI automation or synthetic keyboard input.

```powershell
tests/observation/verify-ccpu-sign-flags-guest.ps1 `
  -Workload BiosDispatch -PackageRoot <accepted-or-candidate-package> `
  -Observer build/tools/console-startup-observer.exe `
  -LogonHelper build/t449-s2-message-construction-r3/logon-env.dll `
  -BuildRoot <fresh-repo-build-directory>
```

The real package roots and full commands are recorded under `bios-runs/`.
Each normal package run checks **836 cases**:

* BOP FF: all 256 AL values for generic AH=1 and NT AH=4 return AX=1;
  all 256 AL values for wrong-host AH=2 return AX=2: 768 cases.
* BOP F0 and F7: 32 repetitions each preserve AX: 64 cases.
* BOP 67: AH=00,3F,60,FF return AH=84 while preserving AL=5A: four cases.

Each case also checks EAX's upper word, full EBX/ECX/EDX/ESI/EDI/EBP, SP,
DS/ES and defined arithmetic/direction flags over four patterns. Output is
short enough for the real 53-column viewport. No valid EMS allocation,
active hardware acknowledgement or unsupported error dialog is invoked.

Both the accepted package and sealed candidate pass the normal 836-case
workload. `-NegativeOracle` deliberately expects zero instead of the first
control error; both packages exit one at phase C with no success marker,
and the runner recognizes that rejection. The same batch reruns normal and
wrong-oracle Control and SignFlags contracts: four additional passes.
Thus all eight runner contracts pass; deliberate guest exit-one outcomes
are negative-oracle successes, not hidden runtime failures.

## Remaining recovery and classification

The [existing slave BIOS finding](shard-0/slave-bios-eoi-source-review.md)
still needs real ISR/mask/EOI-callback evidence, no-ISR/master-only, AEOI,
repeat delivery and the actual guest vector/BOP route. Current PIC reset
tests cannot stand in for that handler. Source inspection of EMS also shows
stepwise page/map mutation and reset of only selected state; establish the
actual provider failure and reset contracts before declaring a defect or
inventing rollback. New byte matching and invalid-function cases do not
prove these boundaries.

The [worklist](recovery-worklist.md) retains finite S11/S12/S13/S17 owners.
The exact seven-row checker preserves all unrelated rows, 4,684 frozen paths,
52 full rows and the registered source-drift set. Totals become **1,576 pending,
424 partial, 52 full and 2,632 excluded physical file roles**.

Owned process records, input hashes, raw guest outputs, wrong-oracle results
and the bounded metadata manifest are retained in `bios-runs/`; generated
SDK bodies and test EXEs/COMs are not imported. The source/image/negative
checks are replayed by the checkpoint verifier; preserved-package and
documentation gates are separate. No product source changed, so this is not
a new runtime release or a claim of full product regression. The earlier
candidate SOL failures remain unexplained and it remains unpublished.
S10 and T473 remain open.
