# T481 S9 temporary-copy cleanup

The owner requested prompt cleanup of test-owned `O:/tmp` copies and priority
for the Win+R startup failure. A read-only inventory found 963 directories
with test ownership/staging markers (about 16.3 GiB), and 129 unmarked
directories (about 3.1 GiB). The unmarked directories include unrelated and
research material and were not selected. The ownership, paths, link/Git
checks and process observations are recorded in `build/t481-s9/tmp-audit.json`
and `tmp-clean-result.json` while that disposable task root exists.

The reviewed cleanup removed 862 marked test directories, totaling
16,130,183,050 inventoried bytes (15.02 GiB). It retained 95 stages whose
binary identity differed from their marker, three directories that failed
deletion due to file use, two directories with running `ORDER.EXE` processes,
and one guest copy whose owner marker did not match the selected task roots.
Some locked copies may have been partially deleted; no claim of intactness is
made. No process was killed for this cleanup. A subsequent free-space reading
was approximately 209 GiB; other disk activity occurred during this period,
so its change is not attributed entirely to this cleanup.

Test changes in progress make Win3.1/Win95 probe-copy owners preserve their
small result witnesses and remove their own marked `O:/tmp` copy in a finally
block. Preparation failures are also cleaned. The ownership helper refuses
wrong markers, links, Git data and active processes. `guest-cleanup-test5`
passed those negative checks, nested paths and both preparation-failure
paths. `win31-auto-cleanup` passed one enhanced-mode dual-VM test and removed
its copy; `win95-auto-cleanup` passed the installer and four-native-command
dual-VM test and removed its copy. Both verified source hashes unchanged.
Win95 guest exit 1 is not established as normal shutdown. Eight older runtime
stage runners have also been changed to retire their stages on failure after
retaining available diagnostics; their individual runtime failure paths have
not yet been rerun. One later combined negative runner failed before reaching
its intended assertion because its observer executable had disappeared during
concurrent build-directory activity; it is not a product result.

After the initial unavailable-observer attempt, `runner-failure-cleanup2`
used an existing build-owned executable solely as a resolved placeholder;
both Win3.1 and Win95 runners then reached their deliberately induced
post-copy failure and removed their respective copies. The placeholder was
not launched. Changed PowerShell scripts passed syntax and whitespace checks.

The owner then directed priority back to Win+R direct DOS startup. Eight
older runtime-stage runners have not had their individual failure paths run;
their available diagnostics are retained before cleanup and the stage helper
still refuses changed binaries or live processes. The direct startup failure
remains open and no product release was made by this cleanup work.
