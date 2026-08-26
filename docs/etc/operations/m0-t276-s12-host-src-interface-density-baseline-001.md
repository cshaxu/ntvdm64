# M0 T276 S12 P3: `host/src` interface coverage baseline

## Purpose

Measure every original `host/src` C translation unit against the existing
case-sensitive T276/S4 call ledger and T276/S5 definition/boundary ledger. The
result is an audit-coverage and ordering aid; lexical names never establish an
ABI, linkable provider, adapter owner or runtime reachability.

## Exact coverage

All 56 source units are represented. They contain 7,079 lexical call records.
The existing selected-definition correlation observes 380 original definition
names and 37 external-boundary candidates across the eight owner groups:

- SoftPC facade: 10 files, 851 calls, 22 selected-definition observations,
  eight external-boundary candidates.
- VDM monitor: four files, 447 calls, 27 observations, three candidates.
- Cross-owner startup composition: one file, 178 calls, 13 observations, four
  candidates.
  candidates. It is not a direct implementation priority merely because it
  sits in the historical `spchost` library.
- Win32 host capability: nine files, 655 calls, 11 observations, two
  candidates.
- VDD/debugger: two files, 246 calls, one observation, zero candidates.
- Device bridges: 18 files, 3,263 calls, 258 observations, eight candidates.
- BOP dispatcher: one file, 221 calls, 21 observations, zero candidates.
- Session/monitor worker: one file, 18 calls, two observations, one candidate.
- Literal-build-unobserved auxiliaries: ten files, 1,200 calls, 25
  observations and 11 candidates. These retain the strictest non-enable state.

The highest current boundary-candidate count belongs to unobserved
`nt_cprgs.c` (ten), so it is explicitly *not* an implementation priority. Of
the original literal `SOURCES` members, `nt_msscs.c`, `nt_umb.c`, `nt_eoi.c`
and `sim32.c` each have three or four candidates and form the first source
evidence set for later package-level ABI review.

## Limitation and ordering rule

The 7,079 call records include macros, local calls, historical preprocessor
forms and calls with declarations outside the selected source set. Therefore
the 37 boundary count is not a closure count. No task may implement a single
one of these calls merely because it ranks highly. A later owner package must
first extract its original declaration, calling convention/layout, source-form,
provider/failure path and exact adapter or Bochs prerequisite.

This gives the next audit order:

1. complete monitor/session group;
2. complete SoftPC/SAS facade group;
3. complete Win32 host-capability group;
4. device bridge groups only after their Bochs-versus-public-host ownership is
   separately proven;
5. unobserved auxiliaries only when an original build/profile description
   proves inclusion.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Audit-T276S12HostSrcInterfaces.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S12HostSrcInterfaces.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

Expected result: `T276 S12 host/src interface-density verification passed: 56 source rows.`
