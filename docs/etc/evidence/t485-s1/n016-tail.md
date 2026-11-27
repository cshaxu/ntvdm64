# N016 obsolete fragments and XP published declarations - S15 P7

Starting HEAD2df3d9e26; fixed XP commit
`a2f6d7c93aa4f11efc51147566d5961c59a8ef89`. Git object raw bytes are the
source, not newline-normalized shell output. Evidence: build/t485-s15/n016-part4.
[Source disposition](n016-tail-sources.json), [census](n016-tail-census.json).

## Whole-file decisions

DIR positions16-21 close the existing N016 inventory. winbase.x, winbasep.x
and winnls.x have no corresponding filename anywhere in the pinned XP tree
and no current source/test/build references. They are obsolete generated
OpenNT fragments, not required production providers; remove them rather than
keeping obsolete files at an XP-owned path. winnls.w moves to base/published;
winsvc.x moves to published/genxwin. Both retain complete XP bytes including
unused declarations; neither has a current build consumer. No new XP-only
component or original guest build is introduced.

winss.h is also absent from XP. Its live WINSS/BASESRV/CONSRV/USERSRV constants
are already in byte-exact XP public/sdk/inc/ntcsrmsg.h. All five consumers
already include that header: run16 main, common base_rpc_client_launch,
BaseClient vdm.c, base_dispatch_test and base_client_rpc_first_test.
None uses winss's GDI/MMSND/USERK service IDs or its indirect Console wait API.
Delete the obsolete header and five redundant includes; no copied declarations,
new forwarding header, changed API numbering or RPC implementation.
vdm.c removal is inside its existing adaptation block; subsequent original
#line resets and all original excluded arms remain untouched.

## Metrics and interpretation

| File | Prior nonerror points/lines | Final disposition |
| --- | ---: | --- |
| winbase.x | 1/11840 | Removed, absent XP/unreferenced |
| winbasep.x | 1/646 | Removed, absent XP/unreferenced |
| winnls.w | 0/0 | XP byte-exact at original published path |
| winnls.x | 1/1816 | Removed, absent XP/unreferenced |
| winss.h | 1/94 | Removed, XP declaration owner already present |
| winsvc.x | 0/0 | XP byte-exact at original genxwin path |
| paired BaseClient vdm.c | 16/235 | 16/234 |

Current non-guest census:1315files(-4),750raw exact(0),755nonerror same(0),
560different(-4),7747points(-4),176175lines(-14397).1212XP/103OpenNT.
Of the14397-line reduction,14396 comes from retiring four obsolete mirrors;
one comes from removing the redundant include. Do not describe retirement
as14396 lines of restored XP algorithm. Both moved templates were already
exact against their old OpenNT baseline and are now exact against XP.

The paired vdm.c retains its established exclusion projection: original and
new bytes differ solely by the inserted winss include. Raw XP diff remains
17points while lines309->308, proving no new split; unchanged excluded bodies
leave nonerror16points with235->234lines. The general branch matcher cannot
map product-only outer guards to original guards, so it is not used to guess
a new projection. Per-file originals and hashes remain in the census.

## Verification

Cached full build and runtime7 dual publication passed,13images/26hashes.
Existing BASE dispatch140assertions, original server lifecycle, real RPC reply,
seven COMMAND cases and actual WOW WINMINE window passed.
[Delivery receipt](n016-tail-result.json). P6 Full24/24 remains the checkpoint;
this batch has six inventory files plus one paired mirror caller, fewer than10.
N016 is21/21 reconciled; overall52/159 directories and S15 4/14 delivered.
Next N017. No original guest binary changed or rebuilt.
