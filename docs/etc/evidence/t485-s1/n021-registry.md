# N021 registry declarations - S15 P26

Starting HEAD8d8d36f1a. N021 DIR position12, ntregapi.h. Fixed XP SP1
commit a2f6d7c93aa4f11efc51147566d5961c59a8ef89, blob
77f3cae0ad7cc82aaa4dc65e5f3ed11d11602a6d. Previous OpenNT blob
77c1ad5280b3b2a49423219eab40c8be030a1a63. Raw originals, initial bytes,
compiler logs and final measurements are under build/t485-s15/n021-registry.

## Whole-file disposition

Restore the complete XP registry declaration framework, including unused
WOW64 access flags, key information classes, limits, save formats and service
prototypes. Restore the original C++ linkage wrappers. The old C++ #error
exclusions do not justify rejecting a supported product compilation context.
No #error remains in this file. Original LF/EOF and selected diagnostic lines
are verified, with zero matched-source line discrepancies.

Actual x86 and x64 native-service-layout compilations of the raw XP header
independently fail on the same six conflicts with the Windows SDK. Keep only:

- KEY_SET_INFORMATION_CLASS and KEY_VALUE_ENTRY complete-type omissions;
- NTSYSCALLAPI omission on NtNotifyChangeMultipleKeys,
  NtQueryMultipleValueKey, NtRenameKey and NtSetInformationKey.

For the four functions, retain every original parameter and calling convention;
only the duplicate declaration's import attribute is omitted. Both actual
compile probes then pass. The SDK supplies these existing native APIs; there
is no new registry implementation, callback, transport or runtime selector.
The header is consumed through the original nt.h umbrella by imported host
code and Base/CSR consumers. Full verification exercises the published product.
Exact original ranges and C2011/C2375 evidence are retained in omissions.json
and the delivery receipt. No guest source or binary is modified or rebuilt.

## Difference accounting

Before OpenNT5points/17lines; after XP6points/20lines, delta+1point/+3lines.
Raw and nonerror measurements coincide because no exclusion remains. The small
increase is within the owner's standing approval; XP NtRenameKey's additional
prototype needs the same SDK import binding as the three old declarations.
There is one individual OpenNT-to-XP baseline transition, no added/removed file.

Global candidate1324files,775byte-exact,786nonerror identical/538different,
7750points/150814lines. Delta0files/0exact/0same/0different/+1point/+3lines.
1256XP/68OpenNT. LF/EOF/#line differences remain counted.

## Full failure and targeted correction

The first Full passed nine gates including all three Windows desktops and
Solitaire Options, then failed wow-frontiers when the observer did not exit.
Its timeout-live record already shows the Solitaire main window. The diagnostic
child walk followed a stale numeric parent-PID relation into the test runner
and observer itself (PID21636), then stopped producing output. This is not
proof of a registry runtime failure.

The observer previously matched only Toolhelp parent PID. The correction pins
parent/child process handles through traversal, rejects a child older than its
parent identity, and excludes the observing process. It affects timeout-only
diagnostic traversal, not launching, inputs, application-window assertions or
product state. The failed observation is retained; it is not relabeled a pass.
No product binary or mirror diff is changed by this test correction.

The retry runner is a recorded disposable copy of the canonical Full runner:
only already-passed gate bodies are skipped; all assertions remain identical.
Product package manifests are compared before merging results. After rebuilding
the observer, repeat its affected wow-frontiers gate separately. Other passed
gates retain their original package/observer identities and impact analysis;
no whole Full rerun is warranted for this diagnostic-only correction.

## Ten-file checkpoint

Final cached full build and runtime27 dual publication passed:13 host images,
26 destination hashes. Both native capture fixtures pass3475 assertions,
both layouts pass, all42 service cases pass and actual guest COMMAND/NetAPI
returns NETAPI-OK. Full24/24 is consolidated against the final published package: nine initial
passes, remaining-gate retry and final observer WOW revalidation. The initial
timeout failure and diagnostic-only correction remain in the receipt.
[Receipt](n021-registry-result.json); [global census](n021-registry-census.json).
This completes the ten-file checkpoint and resets its counter to zero.
N02112/17 delivered; next ntrtl.h. Overall56/159, S15 8/14 directories.
The known Win95 exit-one boundary remains; no guest changes.
