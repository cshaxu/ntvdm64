# T225 S30 segment-access tuple source, ABI, and failure map

## Need

S29 proves that the first native `#GP(13)` is profile-neutral and that the
original Bochs log is emitted by `BX_CPU_C::read_virtual_word_32`.  The current
first-fault event is intentionally missing the selected segment and rejected
access values.  No BOP/provider or OpenNT source can supply Bochs' internal
segment-cache fact.

## Original owner and exact failure branches

The adopted original source in `src/bx-core/cpu/access32.cc:493-545` owns the
word-read operation.  Its two relevant source branches are:

| Branch | Original predicate | Existing result |
| --- | --- | --- |
| direct limit rejection | `seg->cache.valid & SegAccessROK` and `offset >= seg->cache.u.segment.limit_scaled` | log `read_virtual_word_32(): segment limit violation`, then `exception(int_number(s), 0)` |
| access-check rejection | the first predicate is false and `read_virtual_checks(seg, offset, 2)` returns false | `exception(int_number(s), 0)` |

S28's exact original log selects the direct-limit branch for the observed
native failure.  It does not establish that every future non-UD fault is this
branch, so the diagnostic must label the branch rather than infer it from an
exception vector.

`exception.cc:842-883` is the already-admitted first-fault owner.  It captures
CPU state only after an access path calls `exception()`, and therefore cannot
recover `s`, `offset`, cached limit, or which access branch was taken.

## Four-rung recovery ledger

| Rung | Disposition |
| --- | --- |
| 1. Original source reuse | Retain Bochs' original `read_virtual_word_32` predicates, logging, `exception(int_number(s), 0)` ordering and return/longjmp behavior unchanged.  No OpenNT host translation unit owns this internal state. |
| 2. Smallest adapter/shim | Rejected: bx-vdm and an external shim have neither the `BX_CPU_C` object nor safe access to its segment cache; exporting either would violate the core boundary.  The existing mantle observer is a copied sink, not an adapter route. |
| 3. Registered external-code intrusion | Required: one default-off call immediately before each of the two original rejection calls, carrying only copied scalar mechanical values to mantle.  This is registered as BX-TRACE-085 before code changes. |
| 4. Newly authored behavior | Rejected: the observer introduces no emulation, guest state, selector decode, exception result, or fallback.  It may copy a fact and otherwise declines. |

## Proposed private ABI

The new mantle-private record is versioned, fixed width, pointer-free and
contains only `magic`, `version`, `struct_bytes`, `cpu_id`, access kind
(`virtual-read-word`), branch kind (`direct-limit` or `read-check`), segment
register index, selected segment value, effective offset, width (`2`), cached
valid bits, and scaled limit.  It contains no instruction bytes, guest-memory
bytes/address, linear/physical address, selector/service identity, OpenNT/DOS
term, profile, provider, host pointer, or CPU result.

The record is accepted only while the existing private first-fault diagnostic
is armed and only once.  The original core branch continues immediately to its
existing `exception()` call.  The already implemented first-fault observer
then requests the existing finite stop; no new stop mechanism is introduced.
An unarmed, invalid, duplicate, or unrelated event declines and the original
path remains byte-for-byte behaviorally unchanged.

## Required tests and exclusions

The formal CPU5/P-MMX MSVC x64 `/MT` graph must add a focused fixture that
proves disabled decline, one test-owned direct-limit record, correct copied
tuple, duplicate decline, and the retained first-fault finite stop.  It must
also prove the generic-UD bridge does not participate.  Only after that
fixture passes may one immutable Direct and one immutable Readonly run be made
with the existing `--observe-first-fault` control; they must be compared as
mechanical evidence, not interpreted as a repair.

No descriptor fix, CPU-state change, code decode, BOP route, guest-memory
copy, machine device, BIOS/PIC/FPU/timer work, profile behavior, or OpenNT
provider is admitted by this map.
