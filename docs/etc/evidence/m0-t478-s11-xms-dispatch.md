# T478 S11 — public XMS functions versus private service dispatch

Baseline: 9b74d4fab. Finding 22 is dispositioned without production changes.
The owner requires original semantics, no invented invalid-input tolerance and
no repeated repair of already resolved XMS block results.

## Contract and current selection

Public XMS function AH is not the byte indexed by XMSDispatch. Original HIMEM
checks AH against NumFns at himem.asm:464 before dispatching its public function:
invalid values return AX=0 / BL=ERR_NOTIMPLEMENTED. Its functions emit literal
XMSSVC service constants. This read-only guest reference establishes the host
contract only; it is not a guest audit or modification.

Host MS_bop_2 reads the private service byte following BOP_XMS and calls
XMSDispatch, then advances IP. xmssvc.h and xmssvc.inc agree on all constants:
00 through 0B, with XMS_LASTSVC=0C. The selected table has all twelve original
handlers, in the original order. Removing comments, #line and the excluded DBG
block yields identical token sequences for current xmsdisp.c and both original
families. DIV-516 correctly keeps the unselected debug branch excluded.

The original release dispatcher has no fallback for arbitrary out-of-range
private BOP bytes. Its debug check is a diagnostic-build behavior, not proof
that normal public XMS invalid functions need a missing host handler. Promoting
that debug behavior into release would add tolerance under the present scope.
No such change is made. The original limitation remains explicit.

## Verification and limits

[Actual-dispatch probe](../../../artifacts/mvdm-audit/shard-3/xms-dispatch-s11-probe.c),
[runner](../../../artifacts/mvdm-audit/shard-3/xms-dispatch-s11-probe.cmd) and
[results](../../../artifacts/mvdm-audit/shard-3/xms-dispatch-s11-results.json).
The current product xmsdisp.obj calls twelve sentinel handlers in the expected
order, exactly once each, and returns TRUE. This proves routing, not all
handler semantics. No out-of-range memory access was deliberately executed.
The public invalid-function guard is source evidence, not a new guest test.

An initial test filename containing dispatch triggered Windows executable
installer detection (launch reported elevation required); using the same bytes
as xms-service-probe.exe runs normally. No elevated product behavior, code
change or UAC bypass is part of this test.

[S7](m0-t478-s7-xms-boundaries.md) already repaired the real zero/equal-size
block return defects and tested actual providers plus immutable DOS execution.
Those repairs are retained and not repeated. There is no missing dispatcher,
service constant or applicable alternate branch to reconnect here.

## Disposition

Retain original release private-input limits; reopen only for a valid original
caller emitting a missing/out-of-range service, project divergence, or explicit
approval for stronger private-BOP error handling. No new XMS capability,
replacement implementation or guest change. Package
694495a1-285e-4e8c-940f-3f2425de4445 (0.0.478/protocol 67) remains unchanged;
all ten release hashes match. S9's Full result remains product evidence, not a
new S11 run. No new product build/publication is needed; no O:/winnt write or push.
