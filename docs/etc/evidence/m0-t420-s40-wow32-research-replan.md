# T420 S40 WOW32 research and planning conclusion

## Owner-directed scope

On 2026-09-21 the owner changed S40's exit result to successful research and
an updated T420 WOW32 plan. Five consecutive implementation packets are now
S41--S45. Former WOW16 S41, DBG S42 and VDD S43 become S46, S47 and S48.
T420 remains open pending the owner's final acceptance audit.

## Established findings and limits

The [source and experiment evidence](m0-t420-s40-wow-user-profile.md)
records the immutable USER.EXE hash, 47 checked PMODE32 consumer patterns,
26 original ordinary-thunk mappings and original registration contract:
21 input callbacks, 20 output callbacks and a SHAREDINFO return address.
Original W32Init calls UserRegisterWowHandlers; the recorded modern USER32
probe returns STATUS_ACCESS_DENIED with no populated output slots. Linking
recovered USER bodies has therefore not closed production registration.

Original client initialization consumes USERCONNECT, while server mapping
relocates section pointers into a client view. These are source evidence for
the ABI and visibility contract, not permission to import the USER server.
The earlier private-page-directory experiment was withdrawn. The subsequent
claim that the selected NTVDM necessarily uses DOSX VCPI paging remains an
unproven implementation hypothesis: S41 must inspect actual CR0/CR3 and the
selected transition path before adopting a mapping strategy.

This research conclusion does not prove working WRITE, correct shared-data
publication, completed W1--W6, or minimized current adapter implementations.
It makes their missing contracts and named receivers explicit.

## Complete handoff

The [revised proposal](../../proposals/proposal-mvdm-runtime-package-completion-001.md)
defines every new packet and acceptance gate. The
[retained checklist](../operations/m0-t420-s40-wow32-closure-checklist.md)
maps every W1--W7 item to its receiver. S41 owns data/domain, S42 owns the
interdependent task/window/message lifecycle, S43 resources/GDI/DDE, S44
remaining services plus complete registrar, and S45 real whole-provider
acceptance. S44 implements and S45 validates both inherited OEM-WOW items;
S46 revalidates guest integration and S47 retains OEM-DBG-PATH.

Pre-existing uncommitted source, imported subsets, tests and build changes
remain research candidates, not accepted production delivery. S41 reviews
data/domain/memory changes; S42 reviews task/window/message/callback code;
S43 reviews resources/GDI/DDE; S44 reviews remaining thunks and provider build
composition. Their original provenance, superseded local implementations,
formatting, build selection and tests must be reviewed before adoption or
removal. Preserve unrelated base-service and proposal changes. No guest binary
is changed and no unverified artifact is published by this planning delivery.

## Verification and delivery

This is a documentation-only scope revision. Verify the 48-row sequence,
unique receivers for all W1--W7/OEM obligations, one active S41 packet,
documentation governance, relative links and whitespace. Implementation
packets retain formal x86, all 17 direct/nested DOS routes and their own
positive/failure/teardown requirements. Research conclusion and documentation
delivery do not certify the retained dirty implementation snapshot.

Executed checks for this revision: documentation governance and relative-link
verification both returned exit 0. The read-only USER profile check returned
WOW_USER_PROFILE_SOURCE_AND_BINARY_OK, 47 consumer checks, PMODE32 and the
pinned SHA-256 6746088F2A87CD366E4FF5DA424B39B5BD91586D5F7570AFBF3C05BCA94E935C.
The plan contains exactly S1--S48 once in its ordered table. No production
source or guest binary was modified for this planning change. No new build or
runtime success is claimed; implementation verification belongs to S41--S45.
