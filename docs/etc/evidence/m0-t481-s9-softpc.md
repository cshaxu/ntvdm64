# T481 S9: SoftPC import evidence (in progress)

S8 was closed by the owner; its remaining observed-test boundaries are recorded
in [the S8 closure](m0-t481-s8-closure.md). S9 is not yet complete or published.

Three candidate mirror changes are registered as MVDM-HOST-DIV-894 through
896. They are selected by the x86 product build; source selection does not
establish that a guest workload hit each path.

- BIOS word and DWORD virtualization fallback: a fixture extracts both
  actual function bodies and uses stubbed address translation and nested
  execution. The prior word cast fails; corrected protected-mode and software
  V86 fallback and successful translation cases pass. This is not proof of a
  real guest fallback path.
- CHAIN2 Copy fill: the actual-provider C-VID fixture failed its full-plane
  comparison before the SoftPC-derived fix. It passes after the fix across
  odd/even addresses, four values and counts including zero.
- Unchained DWORD read: the actual-provider comparison against four consecutive
  byte reads failed before the offset fix and passes afterward.

Cached x86/x64 CMake builds and the separately linked x86 NTVDM worker passed.
An 11-image candidate was staged with a single package identity. Full selected
product gates passed against that package; DOS4GW/Nesticle was skipped because
the external fixture was not supplied. The first Full invocation stopped at
the control tests because the assembled cache lacked four test executables;
the complete second invocation used current built fixtures. The x86 and x64
non-desktop CTest selections each passed 46/46. An isolated Win3.1 matrix
passed standard/enhanced mode with DOSONLY 0/1, including normal exits.

An isolated Win95 test using the candidate host and released NTVDM64.386 passed
two concurrent VM identities, 320 callback checks and four native commands;
the source installation's file hashes were unchanged. The observer reported
guest exit code 1, so normal Win95 shutdown is not established. Mouse/editors,
the owner's repeated Win3.1 switch sequence and full S10 acceptance remain
unverified on this candidate.

SoftPC `1ec0d551` virtual text aperture changes are already registered under
DIV-545; `8c27a3b6` CHAIN2 reads are already DIV-889. CPL access, slave EOI
and virtual text height are already covered by DIV-891, DIV-883 and DIV-890.
SoftPC `42533c7d` matches the S8 mouse fix; its later `421b2fcb` removes the
driver in SoftPC's different runtime, while this project still selects it.
SoftPC `ecf263a8` concerns snapshot interfaces not present by name here; no
claim of equivalent snapshot behavior follows from that observation.

The tested 11-image package `53589968-545f-4d1d-8951-db38d0131d13` was
published to both `assets/release` and `O:/winnt/system32`, with manifest
hashes checked. The two released drivers already match `O:/w31/PATCH`, and
`O:/w31/SYSTEM.INI` was verified unchanged. This is a manual-test candidate;
S9 and the Win95 acceptance work remain open.
