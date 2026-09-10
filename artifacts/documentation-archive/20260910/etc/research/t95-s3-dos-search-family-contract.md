# M0 T95 S3: Paired DOS Search-Family Contract

## Decision

The first reached search boundary, `50:0A` (`SVC_DEMFINDFIRSTFCB`), is not an
independent capability. OpenNT dispatch defines a four-service family:

| Service | ID | Source entry | Guest client |
| --- | ---: | --- | --- |
| pathname first | `50:09` | `demFindFirst` | `$FIND_FIRST` |
| FCB first | `50:0A` | `demFindFirstFCB` | `$DIR_SEARCH_FIRST` |
| pathname next | `50:0B` | `demFindNext` | `$FIND_NEXT` |
| FCB next | `50:0C` | `demFindNextFCB` | `$DIR_SEARCH_NEXT` |

All four share the same historical `FFINDLIST` lifetime. An adapter may not
resume any one member until it can safely create, validate, clear, and release
the paired continuation state for both output forms.

## Proven Guest Inputs

| Family | First inputs | Next inputs | Source evidence |
| --- | --- | --- | --- |
| FCB | `ES:DI` canonical OEM wildcard path; `DS:SI` `SEARCHBUF`; `AL` extended-FCB flag; `DL` attributes | `DS:SI` same `SEARCHBUF`; `AL`/`DL` | `demsrch.c`, `search.asm` |
| pathname | `DS:DX` canonical OEM wildcard path; `CX` attributes; DTA address loaded through the previously registered DTA locator | DTA address through that locator | `demsrch.c`, `search.asm`, `demgset.c` |

Both canonical paths originate in the NTDOS `OpenBuf`, explicitly declared as
128 bytes. The future request read must be bounded to that staging capacity,
must require a NUL terminator within it, and must reject malformed/non-OEM
input before any host lookup. The present generic guest-read action caps a
single read at 64 bytes. That is insufficient; S3 does **not** authorize
splitting it ad hoc or adding a search-specific reader. S5 must introduce any
needed generic, bounded transport first.

## Exact Path DTA Layout

`SIZEOF_DOSSRCHDTA` is source-defined as 43 bytes. The packed, 16-bit guest
layout is therefore fixed as follows; all multibyte values are little-endian.

| Offset | Bytes | Field | First/next behavior |
| ---: | ---: | --- | --- |
| 0 | 4 | historical `pFFindEntry` | clear before first; validate and clear on invalid/exhausted next; future adapter writes a non-pointer cookie here |
| 4 | 4 | `FFindId` | clear before first; adapter-owned search ID on retained continuation |
| 8 | 13 | reserved | preserved except where source writes defined fields |
| 21 | 1 | attributes | result attributes |
| 22 | 2 | DOS write time | result |
| 24 | 2 | DOS write date | result |
| 26 | 2 | size low | result |
| 28 | 2 | size high | result |
| 30 | 13 | 8.3 NUL-terminated result name | result |

The original 32-bit host writes its pointer and ID through `STOREDWORD`.
That proves the guest slots are 32-bit values, not that an x64 adapter may
store a host pointer. The future value at offset 0 is a fixed-width,
unforgeable-in-context adapter cookie; offset 4 is a separate opaque ID. Both
are checked against the adapter's session table, never dereferenced.

## FCB Staging and Result Layout

NTDOS declares `SEARCHBUF` as **53 bytes**. The OpenNT packed C structures
identify the following 52-byte semantic prefix; the one-byte allocation
remainder and the documented `dec si` alignment workaround must be preserved
as an observation in the first runtime trace, not normalized away by x64 C
struct packing.

| Offset | Bytes | Field |
| ---: | ---: | --- |
| 0 | 1 | drive number |
| 1 | 8 | space-padded filename |
| 9 | 3 | space-padded extension |
| 12 | 2 | current block, zeroed |
| 14 | 2 | record size, zeroed |
| 16 | 4 | low file size |
| 20 | 8 | embedded directory filename |
| 28 | 3 | embedded directory extension |
| 31 | 1 | attributes |
| 32 | 4 | historical `pFFindEntry`, replaced by adapter cookie |
| 36 | 4 | `FFindId`, replaced by adapter ID |
| 40 | 2 | DOS-reserved dummy |
| 42 | 2 | DOS time |
| 44 | 2 | DOS date |
| 46 | 2 | DOS-reserved cluster field |
| 48 | 4 | low file size |

`$DIR_SEARCH_FIRST` transfers the continuation region back into the caller's
FCB and copies the directory-entry portion to the DTA. `$DIR_SEARCH_NEXT`
reconstructs the staging buffer from that FCB continuation. It does not
dereference the historical pointer slot. This permits a cookie design, but
does not permit a pointer-shaped adapter shortcut.

## Result, State, and Error Rules

- First calls clear cookie and ID before searching. A successful returned entry
  is filled as DOS 8.3 name/ext, attributes, local DOS time/date, and low file
  size. A retained look-ahead session receives cookie/ID; a last result may
  validly clear both while still succeeding.
- Next validates **both** values. Invalid, stale, mismatched, volume-label
  FCB-next, exhausted, and cancelled state clear both and report the DOS
  no-more-files behavior. Session/process termination also releases all
  retained entries.
- Path first normalizes `ERROR_FILE_NOT_FOUND` to no-more-files and bad
  pathname/directory to path-not-found before returning. Path next clears both
  reserved words on no entry.
- The original FCB-next code appears to set CF/error after an empty result,
  then unconditionally executes `FillFCBSrchBuf` and clears CF. This source
  contradiction is intentionally unresolved. S5 cannot choose a replacement
  outcome without a source-built trace or another authoritative consumer
  observation.
- Volume-label search has separate first-call behavior and must be explicitly
  rejected or fully specified as part of the eventual complete capability; it
  cannot silently fall through to ordinary host enumeration.

## Implementation Gates

1. **S4** owns the frozen-drive, read-only namespace projection, deterministic
   ordering, 8.3 collision policy, attributes, and no-reparse containment.
2. **S5** first enlarges or sequences generic bounded guest reads without
   search semantics; only then may it perform this family’s request/response
   transactions and opaque session table.
3. A result commit is one atomic multi-write plus CPU outcome. No partial DTA
   or FCB state may become guest-visible.
4. No Bochs change is proposed. Bochs only consumes already-generic bounded
   read/write/result mechanics.

## Unresolved Observations

- The runtime significance of `SEARCHBUF` byte 52 versus the known 52-byte
  semantic prefix.
- The exact source-built observable outcome of the FCB-next empty-result path.
- The complete volume-label behavior for the intended first profile.

These remain explicit S5 trace gates, rather than permissions to invent DOS
filesystem behavior in S3.
