# Original non-MVDM relative-path correction

The owner corrected the two artificial host/abi intermediate directories.
The selected non-MVDM mirror now retains its original relative roots (`base`,
`ds/netapi`, `private`, `public`, `windows`). Prior declaration provenance is
preserved in [the historical record](m0-t473-original-abi-provenance.md).

The earlier candidate had three different same-relative-path header pairs.
The eight product Ninja dependency databases contained uses of the host
subsets but no uses of the three full ABI copies. A flat isolated candidate
retaining the selected subsets compiled five actual NTVDM translation units.
Replacing the three subsets simultaneously with their full original copies
failed all five. Replacing each header independently failed on the real
`nt_timer.c` command: full ntexapi lacks required `EVENT_TYPE`; full ntrtl
conflicts with current SDK declarations/types; and full nturtl redefines
SDK critical-section structures. These are focused build observations, not a
proof that every potential consumer is semantically equivalent.

The current registered subsets stay at their original relative paths,
including the existing private original-RTL binding. The three unused full
versions are preserved at commit `dc0d7e8f6` and identified by
[hash and comparison results](../../../artifacts/mvdm-audit/source-layout-flatten/).
No original guest is modified or rebuilt. Runtime acceptance and publication
remain pending until the final candidate passes the required tests.

A source-tree comparison also relocated the seven RAP files and the two RTL
assembly subsets to their actual OpenNT relative paths. Project-authored
facades moved to compat or WOW32 ownership. Two selected headers have no
OpenNT counterpart and retain their original OpenNT 4.5 `private` paths:
`dlcio.h` is used by VDMREDIR, and `pmvdm.h` by the SoftPC configuration
source. The unrelated, unreferenced JPEG `x.h` was removed. The updated
host-cohort identity inventory records 172 byte-identical files, two intended
changes and no missing selected files; it does not count the separate ABI
cohort. Exact runtime validation remains recorded separately.

Final delivery: cached x86 and x64 builds passed, including explicit VDMREDIR
and 32-bit Hook targets. The final ten images differed from the previously
verified candidate only in four PE link timestamp bytes each. The Full selected
product gates passed with PowerShell 7; the DOS4GW/Nesticle external-fixture
gate was skipped because no fixture root was supplied. A PowerShell 5 attempt
failed in test harness process argument handling and is not counted as product
evidence. Documentation governance passed. The ten packaged images were
published and SHA-256 verified in both `assets/release` and `O:\winnt\system32`.
See the [final build, comparison and gate records](../../../artifacts/mvdm-audit/source-layout-flatten/).
