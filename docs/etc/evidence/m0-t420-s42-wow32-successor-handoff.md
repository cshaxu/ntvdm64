# T420 S42 WOW32 successor handoff

## Question and owner disposition

On 2026-09-21 the owner directs all WOW32 research and remaining implementation
to a new queue-head T proposal. S42 changes to preservation, review, regression
and handoff closure. It no longer claims complete W1/W2 implementation.
The [successor proposal](../../proposals/proposal-wow32-complete-runtime-recovery-001.md)
owns former T420 S42 unfinished W1/W2, S43 resources/DDE, S44 services and
registration, S45 full-provider acceptance and S46 immutable WOW16/WRITE.
All earlier research remains linked, including disproved hypotheses.

T420 S43 receives former S47 debugger including OEM-DBG-PATH; S44 receives
former S48 VDD; S45 owns final non-WOW32 completeness/regression and the
owner's T acceptance handoff. This prevents a WOW-dependent WRITE gate from
remaining hidden in the supposedly non-WOW32 tail.

## Inputs and interpretation

Baseline is `158f1f1a6` with S41 delivery `801ec6e39`. The
[S42 binding ledger](m0-t420-s42-w1-w2-lifecycle-binding-ledger.md) retains
source owners, failed observations, production wiring state, tests and hashes.
The successor inherits code as reviewed research/partial implementation, not
as proof that native InitTask tests establish real guest task execution.

The confirmed desktop publication failure remains open. The client desktop
setter has no production object producer; active-guest identity-only handle
publication currently refuses instead of returning a corrupt phead; task
cleanup/dispose still lack production exit callers. These are mandatory
successor implementation items, not approved permanent feature exclusions.

## Workspace disposition and verification

Preserve original-source imports, worker/provider bindings, fixtures and
research in version control at their existing owner paths. Preserve the
owner-authorized BaseSrv, run16 and queue/proposal changes in the same reviewed
delivery. No stash-only or ignored-build-only copy counts as preservation.
Move the accidental root fixture executable into the S42 build directory
after verifying its identity. Keep the installed accepted package unchanged
unless the exact new candidate has passed its stated deployment checks.

Handoff closure requires current-source x86 worker/provider builds, the
focused page-domain, original-layout and task-cleanup checks, all 17 DOS
routes on the final candidate pair, source/provenance checks, governance,
diff review and commit/push. Record results below when obtained. This file
does not predeclare those checks passed or S42 delivered.

## Handoff verification results

Fresh roots `build/M0-T420/S42/handoff-x86-r1` and
`build/M0-T420/S42/handoff-provider-r1` compile/link all four EXEs,
VDMREDIR.DLL and all 77 selected WOW32 source bodies plus the recovered USER
owners. The worker graph completes 539 edges and provider graph 128 edges.
Logs are in `build/M0-T420/S42/handoff-r1/{worker,provider}-build.log`.
The provider consumes this fresh worker's ntvdm.lib, not a stale research lib.

| Artifact | SHA-256 |
| --- | --- |
| run16.exe | 0AC41BE74995C15F404D591C6B2EAD5B51C9163E3B566A9D2A2E3A427816BBCA |
| basesrv.exe | 6257D981AB59964B4B831BA3544671EA468ECC30722E41276954B8FD3BE04489 |
| ntvdm.exe | D5EB6E976BCFF608C5CCF403333EBC3226F3D21F4BC97F0602EC944B19A066B1 |
| dtmgr.exe | 2892D1C3CA740C3E6500A0BB3F0C8BF835CBC3F488B9B86E6DDD04140539066A |
| VDMREDIR.DLL | 9C8DC78E57CF9F3E87B8452BF8638299BEB743A655B1541B32621DE52DCFB4CF |
| WOW32.DLL | 253A1B13BFAE2C5F100951023B65E363C5D70507E1EBCC313A7C661997B7268C |

Focused current-source results: `WOW_PAGE_DOMAIN_LIFECYCLE_OK`,
`WOW_ORIGINAL_USER_CLIENT_LAYOUT_OK wnd=176 desktop=100`, and
`WOW_USER_TASK_LIFECYCLE errors=0`. The task fixture is rebuilt with
`tests/observation/verify-wow-task-lifecycle.ps1` using the two fresh roots;
its output is `build/M0-T420/S42/handoff-task-fixture-r1`.
`verify-wow-recovered-owner-bodies.ps1` verifies six original bodies after
registered adaptations and four USER declaration spans byte-for-byte.
`verify-wow-output-composition.ps1 -RequireLocalRegistration` verifies the
local registrar and 20 assignments; its output explicitly disclaims runtime
acceptance and does not erase placeholders or missing producers.

All 17 real DOS regression cases pass on the six-artifact set above, with
expected exit codes and required guest text. Summary:
`O:/winnt/logs/s42-handoff-r1-summary.json`; individual text reports share
the prefix. This includes direct/nested COMMAND, MEM and EDIT. The isolated
test restored and hash-verified the previously installed accepted package;
the partial WOW32 candidate is not published as a working Win16 delivery.
Original USER.EXE and other guest media are untouched.

The accidentally root-built `wow-dialog-lifecycle.exe` (SHA-256
5371C19B4EB37D14D1268BCEBCEEC3044404ED34DD9B2C76584F7ABAA90DC7E3)
is retained under `build/M0-T420/S42/handoff-r1`, not committed as source.
The empty Git index.lock dated 18:37 was found with no Git process at 21:25;
it was moved to the same build root before staging, without discarding data.

The preservation review includes existing BaseSrv shared-WOW exit sentinel,
run16 opt-in child-status tracing, the corrected classifier include path,
and all pending source-policy/queue proposals. The retired adapter README's
provenance is carried by current worker/OpenNT-host registers. These changes
are included with owner authorization rather than left dirty or stashed.
Original-source subsets and partial native bindings are retained research
inputs with their earlier provenance/fixtures; the successor still owes
full family-by-family production acceptance and removal of unnecessary code.

Normalized mirror delta versus admission (including README) is +198/-100
across 18 mvdm paths. The raw Git delta is +3893/-3795 because precomp.h and
wuser.c restore upstream CRLF from an LF index; that format noise is not new
algorithm code. OpenNT-host preserves 21 imported original source/header
files plus its register: +5367/-10 raw lines. Existing/non-mirror source
preservation is +5042/-989 before final EOF cleanup. This is preservation of
previously uncommitted work, not a claim of net implementation reduction.
The successor must minimize it against the original-source contracts.

Documentation governance, relative links and staged whitespace checks are
required again immediately before the handoff commit. The commit containing
this record is the S42 handoff delivery; remote identity and clean status are
checked after push. T420 remains open and WOW32 functionality remains partial.
