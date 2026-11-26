# S10 P1: C-VID glue

Fixed XP source a2f6d7c93aa4f11efc51147566d5961c59a8ef89,
base/mvdm/softpc.new/base/cvidc/ev_glue.c; hash verified against files.csv.
Entry source is the delivered S9 baseline 5822d1a00. No guest changes.

## Differences and decisions

| Frozen point | Actual reason and original alternative | Result |
| --- | --- | --- |
| HXP-R-00640 | CHAIN2 strings need the dirty cells of the written byte span, zero count must not dirty a cell, and the glue must restore its scratch arguments. The original unchained string marker uses offset+count as its inclusive high bound; a span ending at a cell boundary marks the next cell. | Keep the narrow argument conversion and existing marker; no second bitmap. |
| HXP-R-00641 | gfx_updt.c's CHAIN2 EGA-graphics selection calls SetMarkPointers(4). Original ev_glue has cases 0-3 only, so restoration retains the preceding mode's marker rather than selecting the CHAIN2 provider. | Keep case 4 with original byte/word markers and the bounded string wrapper. |
| HXP-R-00642 | setup_vga_globals must publish the selected original CPU/SAS/video providers before consumption. The actual fixture checks Cpu.Private, Cpu.Sas, Cpu.Video, overwrite callback and setup/re-setup behavior. | Keep the one initialization call. |
| HXP-R-00643 | Generated slot carriers do not themselves provide the selected CCPU bindings; qevnt owns restart state, and host STATUS restoration cannot use c_bsic.c's privilege-filtered setFLAGS/c_setEFLAGS. | Keep the original-owner provider list; inline two single-caller binding wrappers into its one initializer. |

The final change preserves the private-table assignments, Cpu.Private/Sas
publication, overwrite slot, public assignments and video publication order.
The two small private/public wrapper functions are removed, not moved into
another component. The generated slot lists and all provider expressions are
unchanged. Linearizing the old wrappers yields the same functional token
sequence as the final initializer. No callback queue, duplicate state or
extra initialization path is added. The calibration callback returns zero to
the original qevnt no-calibration path; the separate restart getter/setter
still accesses qevJumpRestart. The direct STATUS bit setters preserve the
existing host restore semantics, including IOPL/IF independently of CPL.

The CHAIN2 aperture remains the existing bounded video offset domain. This
change does not widen the address/count contract to arbitrary 32-bit spans.
The original dirty bitmap implementation remains in sevid001.c; this P does
not change its algorithms or claim a new exception/reentry guarantee.

## Metrics and line mapping

Non-error, byte-sensitive differences are **21 points/1663 lines ->5/102**:
16 fewer points and 1561 fewer lines. Original LF is restored; original
comments/declarations remain. The one PROD exclusion is preserved and
596 matched original content lines retain XP logical line numbers.
See [metrics](s10-p1-metrics.json) and [complete census](s10-p1-census.csv).

The 30-file S10 baseline is 201/102863; after P1 it is 185/101302. The original
CHAIN4 else in ega_read.c was mapped with its verified XP line anchor. Added
CPU40 writer-table guards in egawrtm0.c/egwrtm12.c have no original conditional
counterpart: their complete differences remain counted rather than masking
original declarations under a newly added error branch. These are counting
decisions only; those files retain their later ordered review slots.

All four frozen points remain in pending decisions with refreshed evidence;
partial simplification is not full restoration or owner approval. S10 reviewed
1/30 files and 4/1060 frozen points; restored frozen points=0.

## Verification

The existing actual-production-library C-VID fixture now executes marker 4
over even/odd offsets, cell boundaries, zero lengths and the final aperture
byte. An independent per-written-byte bitmap oracle checks the whole bitmap,
dirty bounds, no-op behavior and scratch restoration. It also retains the
existing vector/selector, physical-plane read/write/string/SAS and concurrent
scratch tests. This supplements rather than substitutes for real-package tests.

Cached full build and dual publication of runtime1 completed. Targeted
fixture and actual MEM/NTVDMD plus NESticle results are being collected under
build/t485-s10/p1. Full remains S13. Commit/push and final receipt pending.

### Final verification

Build/publication runner PID 7008 and targeted runner PID 11676 completed.
C-VID fixture exited 0, including the added 30 dirty-span cases and existing
original-vector/plane/SAS checks. The final package passed real MEM/NTVDMD
exports, debugger and task lifecycle, and actual NESticle rendered menu with
Escape hide/restore. The 26 destination hashes were checked again after tests.
All 30 census source hashes match. No original guest or winnt.zip changes.
Source and targeted verification complete; P1 commit/push pending.

Delivery receipt: f3dda8496 committed and pushed to origin/main; governance and relative-link checks passed.
