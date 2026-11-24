# T484 S3 P179 shared controller path binding

Win31 identity, Win95 identity and Win95 editor tests separately patched the
same128-byte named path slots in independently authored controller images.
Extend the existing native win31-probe-paths tool with a fields mode and reuse
its bounded loader, unique-marker check, ASCII validation, field capacity and
save path. Accept only VM/W31 witness marker names. Validate every requested
field in memory before saving the image; a rejected later field does not leave
partial on-disk changes. Disk write failure is not claimed to be atomic.
The original matrix two-image mode remains supported and regressed.

The three runners retain their original field/value mappings and variant
selection; only field mutation moves out of PowerShell. Remaining identity
COM scalar/command/environment filling, PIF/config preparation and shared
staging are not migrated by this batch. Do not treat these three original
candidates as complete. No original guest, product or mirror source changes.

Tests extend the existing path-binding suite rather than creating another
framework. Initial generic run PID7144 passes original exact-byte/field checks,
four old failure cases and five new fields-mode failure cases. The first actual
controller attempt PID29996 fails because the new test did not quote CMD define
arguments like the established Invoke-VmIdentityBuild wrapper; no runtime or
product failure is inferred. Correct the test invocation to the same quoted
CMD form and keep its failed logs.

Final build/t484-s3-controller-paths-verified PID13476 runs22.4seconds, exit0.
Five actual identity controller variants (base,reuse,shutdown,input,combined)
and the Win95 editor controller produce byte-for-byte the same complete images
as the previous field-fill algorithm. All non-field bytes and image sizes are
unchanged; all nine rejection cases pass without partial writes. This is binary
construction equivalence, not a new Win31/Win95 runtime acceptance claim.

Cached full and11 raw/22 published identity verification runs as PID30080.
315/355 disposed,40 remain; ledger reasons narrow the remaining work instead
of inflating completion. Existing allowed byte-equivalence evidence avoids
repeating unchanged guest runs. Whole-task clean acceptance remains S5.

PID30080 completes in60.8seconds, exit0;11 raw/22 published identities match.
No redundant publication or original guest installation writes. Final runner
parse checks and governance are retained in closure-result.json/closure.log.
