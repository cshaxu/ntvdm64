# T479 S8 unchanged shared-library delivery

## Result and boundary

SoftPC revision26d118e5 is the fixed read-only source. All four existing
shared component trees (types/base/kvm-base/kvm-window) are imported unchanged:
82 source files,30 component test files,3 shared fixtures,2 integration tests
and1 integration fixture. [Exact118-file identities](m0-t479-s8-imported-files.json)
and [initial comparison](m0-t479-s8-shared-inventory.json) retain evidence.

The sole substantive existing implementation update is upstream native window
scale/snap handling. Forty-three former source differences were include-prefix
rewrites; the local Break identity is now upstream. There is no remaining local
shared-source patch. The lib/ namespace is retained beneath src/lib-nxvm;
local consumers, build manifests and observer entrypoints carry the binding.
Thirty upstream additions complete these component trees without selecting
unrelated console/audio/storage products. New clock/process providers are
covered by shared tests; production still selects its existing18-unit closure.

No original MVDM/non-MVDM mirror, original guest, addon or assets/winnt.zip
changed. Mirror difference delta is zero. The archived S6 candidate remains
unselected; its incomplete contracts still belong to the queued successor.

## Verification

All invocations were owned hidden background processes using the existing
MSVC/ccache/Ninja trees and modern PowerShell runner.

| Run | PID | Outcome |
| --- | --- | --- |
| unit002 | 24276 | x86 shared25/25,17.09s; exit0 |
| unit003 | 19036 | x64 shared25/25,11.14s; exit0 |
| integration005 | 26100 | AMD64 NTCON links; actual native modal/scale and retirement2/2,6.14s; exit0 |
| cached006 | 21492 | Complete x86 worker/WOW and x64 native builds; existing Win32 shared14/14 per architecture; exit0 |
| audit012 | 25432 |118 imported hashes,82 manifest rows, modified PowerShell syntax, governance and diff check; exit0 |
| Full011 | 27172 | Full16/16,317957ms; exit0; [full results](m0-t479-s8-full-timings.json) |
| publish014 | 23484 | Eleven images and manifest identical at assets/release and O:/winnt/system32; exit0 |

Failed attempts remain recorded: unit001 used an invalid cmd batch-path spelling;
Full010 found a missing shared include root in the independent observer
builder. Both failed before their intended product tests. The formal observer
generator and alternate build entrypoints were corrected and rebuilt before
Full011. Queue-link governance007/008 was corrected without weakening rules.

Linux-specific source/test files retain upstream bytes but were not executed
on Windows. These tests do not prove the unfinished guest multitask I/O work,
Win95 bridge, or the three S9 display-transition repairs.

## Publication and next work

[Publication receipt](m0-t479-s8-publication.json) identifies tested package
49af0246-027c-4b11-9ac0-ae94c2a8cfa5 and all11 image hashes. Addon artifacts
remain the previous bounded baseline. S8 source/package delivery is ready for
its reviewed commit/push. T479 remains open; owner-approved S9 now investigates
and imports the accepted SoftPC Win3.1 display fixes.
