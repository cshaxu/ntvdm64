# T190 S1 COMMAND bootstrap contract map 001

Date: 2026-08-12  
Packet: M0 T190 S1  
Disposition: source/ABI/failure map complete; no source change or runtime run.

## Scope and sources

This map treats `54:02` and `54:0F` as one COMMAND bootstrap state machine,
not two independent BOP patches.  Original ownership and order are:

1. `softpc.new/host/src/nt_bop.c`, `MS_bop_4`, reads the fourth BOP byte and
   invokes `CmdDispatch`;
2. `dos/command/cmddisp.c` maps index 2 to `cmdComSpec` and index 15 to
   `cmdGetInitEnvironment`; `inc/cmdsvc.h` gives the stable service IDs; and
3. `dos/command/cmdmisc.c` and `dos/command/cmdenv.c` implement the pair.

The historical host composition cannot join the non-invasive CLI: it uses
CCPU/SAS `GetVDMAddr`, global COMMAND lifecycle state, Win32 environment APIs,
allocation and process state.  Thus the existing adapter seam is correctly a
source-derived provider, but it must retain the original service order and
the bounded copied-memory ABI.

## Original contracts

| Service | Original inputs | Original state/result | CLI-compatible disposition |
| --- | --- | --- | --- |
| `54:02` `cmdComSpec` | first-call only; NUL-ended OEM text at `DS:DX` | cache `COMSPEC=` plus text; cache byte count; AL is `!fConOutput || VDMForWOW` | bounded guest gather, validate a NUL-terminated ASCII/OEM value, cache only copied text and return AL=1 for the non-WOW/no-console profile |
| `54:0F` `cmdGetInitEnvironment` | first-call only; destination `ES:0`; `BX` capacity in paragraphs | if insufficient, return required paragraphs in BX; otherwise write cached COMSPEC plus original prepared environment and return required BX or zero | reuse the cached COMSPEC; preserve the `BX` retry/write transaction; omit ambient Win32 environment deliberately, retaining only CLI-declared environment material when separately admitted |

`cmddata.c` establishes the original `lpszComSpec[64+8]`, `cbComSpec`, and
`IsFirstCall` state.  This gives the first profile a maximum copied COMSPEC
text of 64 bytes plus the `COMSPEC=` prefix and terminator.  A fixed 64-byte
read is the minimum safely bounded request: it can find the original
NUL-terminated input without an unbounded guest scan.  No host pointer or
environment string crosses the boundary.

## Current seam and gap

`bx_ntvdm_cmd_comspec_bootstrap_service` already owns the correct component
shape: v4 requests a checked gather; completion caches an adapter-local
environment string; v2 `54:0F` prepares the existing multi-write/retry
transaction.  Its incorrect restriction is that it derives an expected
profile `COMMAND.COM` pathname and requires byte-for-byte equality before
accepting the gather.  Original `cmdComSpec` makes no such comparison.  T189
observed the resulting `54:02` pass-through.

The existing `54:0F` source-derived environment is intentionally smaller than
the historical ambient Win32 environment.  It must stay that way until a
separate CLI-declared environment capability is admitted; it must not call
`GetEnvironmentStrings`, expose host variables, or become DOS environment
implementation.

## T190 implementation slice and closure plan

T190 S2 may replace only the equality restriction with a fixed-size gather and
source-shaped NUL/ASCII/64-byte validation.  It must preserve first-call
state, `COMSPEC=` construction, AL=1 profile result, existing `54:0F` BX
insufficient/retry/write behavior, and pass-through on rejected input.  It
must add focused positive/negative tests and recompose all changed adapter
objects before any single frozen runtime observation.  It must not modify
Bochs, guest COMMAND source, the generic BOP ingress, host environment, or
other COMMAND services.
