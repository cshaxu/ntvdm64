# M0 T198 S120: Declined Fast-Read Token Lifecycle Map

## Question

What can the exported `4E534005` fast-read token prove about the source-built
guest path, and what exact observation is still needed before a provider
correction could be admitted?

## Original Guest Contract

NTDOS places the SFT's two handle words in `BP` (low) and `AX` (high) before
`SVC_DEMFASTREAD`; `FastOrSlow` chooses that BOP for a non-pipe x86 handle
when the MIPS compatibility bit is clear.  CF selects the fallback to normal
`50:16`; ZF selects whether the fast service first seeks to `BX:SI`.

On the normal last-reference close path, `$Close` loads the same SFT handle
into `BP:AX` and invokes `SVC_DEMCLOSE` (`50:02`).  The original DEM close
consumes `AX:BP`, closes the host handle and returns CF clear.  Thus a BOP
ledger may safely correlate an accepted `50:02` token with a completed
source-derived namespace generation, but it must not infer path identity
from its ordinal.

## Current Namespace Contract

The immutable namespace starts at generation `4E534001`.  An accepted open
returns its current generation; an accepted close increments it once.
It contains five possible entries after composition (COMMAND, CONFIG,
AUTOEXEC, selected target and terminal QUIT), but `open()` scans by copied
canonical pathname and does not impose an entry-order or one-open-per-file
rule.  Therefore the numerical generation counts accepted closes, not files.

| S119 fact | Direct source conclusion | Not proven |
| --- | --- | --- |
| `4001`, `AX=000D` accepted | The CONFIG materialization was open and read. | The intervening opens. |
| `4003`, `AX=003E` then `C4D0` accepted | One open generation served two reads; its full-image read is COMMAND.COM. | That the first read is a separate file. |
| `4005` declined | At least the namespace generation had advanced four times from initialization; the current fast-read preconditions failed. | Which pathname created that generation, why `4004` was closed, or whether the requested data is target/QUIT/another COMMAND open. |

The provider may decline before returning a result for a stale/mismatched
token, no open namespace, an invalid requested span, or any failed read
precondition.  In S119, `CX=0020` is within the provider's maximum and the
physical `DS:DX` window is valid, but the ledger intentionally contains no
live namespace diagnostic or `50:12/50:02` record.  It therefore cannot
separate a handle/lifecycle mismatch from a valid-but-unavailable namespace
resource.

## Disposition

No runtime source is changed by S120.  S121 may add one fixture-only,
fixed-capacity **DEM lifecycle correlation record** at already-existing
composition boundaries.  It may retain only copied selector/service outcome,
`AX:BP` token and a source-defined namespace diagnostic (`open`, file index,
generation and offset).  It must not retain copied path bytes, host pointers,
guest memory, provider selection state or alter result routing.  A later
separate admission, not S121 itself, would decide whether a single native run
is justified.

## Evidence

- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/handle.asm:122-151,455-469,564-579`
- `src/opennt/base/mvdm/dos/dem/demhndl.c:28-87`
- `src/bx-vdm/bx_ntvdm_readonly_namespace.c:100-155`
- `src/bx-vdm/bx_ntvdm_dem_readonly_file_service.c:58-61,130-153,178-186`
- `docs/etc/research/t198-s119-fastread-ledger-native-observation-001.md`
