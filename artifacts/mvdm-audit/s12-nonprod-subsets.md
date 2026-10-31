# S12 â€” non-PROD original host branch subsets

Owner-directed DIV-500 keeps original conditional directives and replaces
330 simple non-PROD bodies in 66 product-selected host C files with numbered
exclusion markers. Each body matched local OpenNT; a per-branch original
range and hash is recorded in [the compact ledger](s12-nonprod-subsets.jsonl).
The 442 selected MVDM C sources whose commands all define PROD were
identified from current ten-target Ninja closures; `accessfn.c` is explicitly
excluded because its command lacks PROD. Observed project source and 830
available dependency headers contain no PROD define/undef override; 31 stale
Ninja dependency paths refer to historical renamed files. This is a current
product-profile classification, not a statement about arbitrary other
configurations. Prior source test fixtures with textual includes need their
own build context verified.

66 files have no explicit later __LINE__ reference; six files containing that
reference and 29 candidate branches remain held. Other condition expressions,
headers, guest sources, unselected files and prior removed mirror structures
remain under audit. No guest source or image was changed. Cached ten-component x86/x64 builds passed. An independent reconstruction verified 66 files and 330 original body hashes without unrelated source-byte changes. Initial NTVDM comparison found two original diagnostic line constants shifted by one after removal in `nt_det.c`; `#line 150` restores their original values. After rebuilding, the ten images matched the published set exactly or only in PE link timestamps. The authored-profile check passed 428 commands. Runtime regression was not repeated or republished for this unchanged-code result.


The test-only `nt_com_mock_fixture.c` is built with `/DPROD`; the event and
keyboard observation objects use the product graph flags, and the exception
trace reuses its host flags. Other independent build recipes were not all
executed; this batch makes no claim about unsupported non-PROD builds.
