# M0 T227 S1 — Guest DOS multi-program integration admission baseline

## Objective

Establish one immutable, source-built Direct/Readonly guest program sequence
and its source/current owner map before attempting runtime repair. The
sequence must follow original NTDOS `$Exec`/PSP/`retf`, child termination by
`INT 21h/AH=4Ch`, NTDOS cleanup/`50:3C`, and COMMAND `54:0B` parent return.

## Selected witness and sources

T225 S45 selected source-built `SHARE.EXE` as the normal child because its
original source exits with `INT 21h/AH=4Ch`, `AL=0`; `QUIT.COM` is excluded
because it selects `BOP_UNSIMULATE`. T225 S44/S55 establish the original and
current route up to the presently unclassified COMMAND/core continuity
boundary. The retained source-built inputs remain NTIO, NTDOS, COMMAND and
SHARE in their declared order.

## Acceptance design

1. Record exact input identity and staging/root contract for Direct and
   Readonly without modifying a guest image.
2. Build a bounded end-to-end witness that can distinguish: child entry,
   child `4Ch`, `50:3C`, `54:0B`, normal parent continuation, and unrelated
   CPU/machine terminal.
3. Run the same declared input once per Direct/Readonly only after the witness
   is admitted and source-compatible; compare the owner-tagged terminal.
4. Any observed failure becomes a whole owner-domain diagnosis. It does not
   authorize a BOP singleton, synthetic DOS EXEC/PSP/return implementation,
   host child process, or input-specific Bochs patch.

## Exclusions

No BOP provider, guest image, profile mode, Bochs CPU/device semantic, or
host capability is changed by S1. This is an admission/source/ABI/fixture
baseline; implementation and repair require the later S selected from the
observed owner domain.