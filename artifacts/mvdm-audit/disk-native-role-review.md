# Original disk owner reconciliation

This S10 checkpoint follows `2d9a08911`. It audits all ten pending physical
disk C-source roles, merges **seven partial integrations**, and retains three
guarded profiles pending. No production source, original guest or runtime
image is changed. This is not full disk acceptance or a release qualification.

## Inputs and actual selection

The [observer](disk-image-observer.py) joins each path to the current DOS16
catalog and both OpenNT/OpenNT-4.5 WOW32 catalog identities. Its
[observations](disk-image-observations.json) record ten DOS rows, twenty WOW
IDs, catalog/scope hashes and both editions' freshly hashed original files.
Every original-edition hash matches the frozen union. The WOW catalog's
`git_blob` is retained identity metadata; it is not claimed to be a newly
reverified Git index here. All ten selected mirror files match primary OpenNT
byte-for-byte; source retention is 100%, not behavior coverage.

The original byte-identical NT `base/disks/sources` names all ten files.
Actual Ninja compile/archive edges and VALID dependency records show their
current production selection. Seven have **102 mapped functions**, whose
non-relocated bytes match at their real addresses in the held candidate.
Complete executable sections equal the cached map-producing worker. These
checks do not resolve every call relocation or prove function behavior.

The observer additionally resolves every original initializer relocation in
the **22 fixed-disk and 25 floppy BIOS slots** to the actual candidate image
pointer, and verifies original BOP13 -> disk_io and BOP40 -> diskette_io.
This proves selected dispatch construction rather than merely finding a name.

## Ownership and finite obligations

| File | Current owner / behavior | Classification and missing proof |
| --- | --- | --- |
| diskbios.c | BOP13 builds the original command structure, selects the 22-entry function table, calls host disk services and translates status/carry. | Partial. Fresh no-media reset is bounded; geometry, real reads/writes/verify/format, DMA limits, status clear and host failure/retirement remain S12/S13. |
| fdisk.c | Original task-file byte/word ports, WD2010 command state, geometry/physical attachment, IRQ and host buffer transfer. | Partial. Prove all command phases, byte/word boundaries, delayed IRQ and attach/detach/reset/failure lifetimes. Actual mapped functions do not establish them. |
| fla.c | Original FLA ports, command/result phase, DMA/non-DMA and deferred completion through GFI. | Partial. Conditional non-DMA paths, buffer ownership, partial phases, IRQ cancellation/reset overlap and retirement need actual witnesses. |
| floppy_i.c | BOP40 validates operation/drive and calls the original floppy table; diskette_int acknowledges IRQ and preserves AX/CS/IP around original INT15 host_simulate. | Partial. POST, invalid/valid drives/functions, DMA, seek-status/EOI, nested callback and failed/abnormal return remain unproved. |
| floppy.c | Original secondary BIOS geometry, media operations, DMA setup, NEC command/result translation and error paths. | Partial. All formats/transfers, media change, boundaries, retry/wait/failure ordering and provider retirement remain S12/S13. |
| gfi.c | Owns shared per-drive dispatch and host configuration calls; SPECIFY visits all four slots, other commands select one. | Partial. Actual-object fixture proves bounded empty/recording-provider dispatch and reinitialization; real media, concurrent changes and worker retirement remain open. |
| gfi_mpty.c | Original absent-drive provider publishes seven callbacks; failed commands and abnormal recalibration completion are intentional. | Partial. Four-drive/all-opcode fixture is not BIOS wait/EOI, published callback relocation or concurrent empty/real-provider transition proof. |
| gfi_ibm.c | SLAVEPC serial RPC client transports requests to a remote IBM PC. | Pending. Its source is compiled but guarded functions are not selected in this profile. Establish applicability and complete transport/provider closure before exclusion or restoration. |
| gfi_sflp.c | SLAVEPC FDC/DMA protocol and remote-drive callbacks. | Pending for the same concrete profile/transport boundary; not accepted or excluded from map absence. |
| gfi_test.c | Non-PROD diagnostic logger called before actual table dispatch; its return is ignored by GFI's diagnostic caller. | Pending T-wide diagnostic-profile disposition. It is not a production disk provider; enabling its synthetic success cannot restore real media. |

The [seven-row ledger](disk-native-role-dispositions.csv) records actual
chains, original identity, behavior/lifecycle limits and follow-up stages.
For each partial file, whole restored-code proportion is explicitly
unmeasurable from linked code and these bounded cases; no fabricated
percentage is derived from function counts. No mirror diff or exception is
added. The two remote-provider profiles and diagnostic profile remain in the
4,684-path denominator and pending set.

## Actual-object contract and regressions

The new [GFI fixture](../../tests/mvdm-host/gfi_object_contract_fixture.c)
links the existing selected production gfi.obj and gfi_mpty.obj without
recompiling or replacing them. It uses original insignia/xt/bios/config/gfi
types and a compile-time 28-byte seven-pointer table ABI check on x86.
Host configuration/media callbacks are recording seams; no disk or host path
is opened and no media success is represented as real I/O.

The fixture passes **433 assertions / eight recorded callbacks**:

- Initialization establishes the original real-server selector and all four
  empty drive slots; old comments describing two do not override MAX_DISKETTES.
- For each drive it checks on/off, unsupported rate, null drive type, changed
  state, reset result, all 32 command codes and guarded result-buffer extents.
- RECALIBRATE yields the original abnormal-seek/equipment result; SPECIFY fans
  out and returns the original success value independently of callback return.
  No change to that original rule is introduced.
- A recording provider verifies one-slot versus four-slot dispatch, the
  original mutation of command drive bits during SPECIFY, and reinitialization
  replacing those callbacks. Host configuration delegation is counted.
- A deliberately wrong four-byte table-size compile fails with C2118; it is a
  type-check negative, not a runtime-device failure test.

The [shared runner](../../tests/observation/verify-softpc-object-contract.ps1)
adds one GFI selection and reuses existing process/receipt plumbing. Fresh
IOS and quick-event controls pass **70 / 37 assertions**, respectively;
both retain their wrong-ABI compile negatives. The fixture build consumes
existing cached objects; no product build tree, archive or image is replaced.

## Real guest boundary

The retained independent [disk probe](../../tests/observation/disks_capability.asm)
executes original INT13 reset with no attached media, returns to DOS, and exits.
Only a compile-time wrong-carry oracle is added. The ordinary probe still
tests the original reset-success contract; no handler or medium is installed.

Four serial private-desktop runs use the existing medium-integrity modern-pwsh
[runner](../../tests/observation/verify-ccpu-sign-flags-guest.ps1), physical
stage and package-scoped RPC: accepted/candidate each pass the normal witness,
and each rejects the deliberately wrong carry expectation with guest exit 1
and S28_INT13_RESET_FAIL. Exact source/tool/package identities, parent token,
PIDs, arguments and results are retained in the captured run metadata.
There is no Computer Use or UI automation.

This does not test a configured disk, real-media reads/writes, every BIOS
entry, guest floppy interrupt completion or the older host-close defect.
The [counter-close finding](shard-3/disk-close-failure-audit.md) remains a
separate actual-object failure transition; its real-session reachability and
repair belong to S12. These new reset/GFI results neither repair nor waive it.

## Delivery and limits

The current exact-delta gate checks the seven transitions, all unrelated
ledger/shard rows, frozen/source identities, all prior full rows and recorded
object/guest contracts. Totals become **1,561 pending, 436 partial, 53 full,
2,634 excluded**. The [worklist](recovery-worklist.md) carries the remaining
disk behavior, lifetime and three profile decisions. Full DOS/WOW/Win3.x and
the held candidate's SOL regression remain open. This bounded audit does not
close S10 or authorize a default runtime publication.

The [delivery result](disk-delivery/stdout.txt) passes the exact-delta,
source/image and captured-metadata hashes, object/guest outcomes, unchanged
accepted/candidate packages, documentation/link and diff checks. Reproduce
with [the delivery verifier](disk-native-delivery-verification.ps1) in the
required owned background context. Binary fixture/COM outputs remain in
disposable build storage; the sources and 192 exact-byte metadata records
are retained with their [manifest](disk-runs/manifest.json).
