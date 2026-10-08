# M0 T437 S1 — installed Windows 3.1 tools

## Scope

This record covers the two owner-requested tools for an already installed
Windows 3.1 tree:

- `tools/win31-launch`: recoverable installation of the two approved guest
  candidates, released `MOUSE31.DRV`, and explicit standard/386 profiles.
- `tools/win31-path`: relocation of proven textual and Program Manager paths.

It does not claim that original Setup completes its protected-mode transition,
or that Windows 3.1 enhanced mode is reliable.

## Inputs and boundaries

The launch finalizer accepts only the owner-approved exact identities:

| Image | Retail SHA-256 | Candidate SHA-256 |
| --- | --- | --- |
| `KRNL386.EXE` | `FBEAF672EE9E917318CE8FCD1D560DB805DBFADF4777039ABBAAFF1A9C75B980` | `88E095A7C39C6294E8E3BE71CBD2EDC1E4DCECBB7101D1581021EA7BE7B33181` |
| `WIN386.EXE` | `6006860AE1003114583D70A1C6447247E1280A99633202912092A5F06A0C22A5` | `C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8` |

It also requires the released `MOUSE31.DRV` hash
`D6BA5380A2EDCDB33E0C36850FB6BDD0542D78EC03E982DBD5114A462B0A2F1A`.
All root, file and driver checks run before `PATCH` creation.  A candidate
input must already have the matching retail recovery copy, so an incomplete
prior mutation is not silently adopted.

`win31-path` first derives one old root from existing `PROGMAN.INI` group
entries.  It replaces a textual occurrence only if its suffix resolves under
the selected root.  For `PMCC` `.GRP` files it validates the group container,
item count/table, item executable-path offsets and NUL boundaries; it appends
new command strings, retargets only the corresponding pointers, updates the
group payload length and recomputes the 16-bit zero-sum checksum.  An invalid
or unsupported group rejects before any INI or `PATCH` write.

## User entrypoints

All three add-on packages now use the same batch interaction shape:

1. open `APPLY.CMD`;
2. enter one target path;
3. perform package-specific validation and preparation;
4. show the actual exit code and pause.

`win101-setup` labels its target accurately as original installation media;
its legacy no-argument `apply-setup.cmd` delegates to `APPLY.CMD`.  Explicit
PowerShell automation arguments remain available for packaging only.

## Build and focused evidence

The two independently authored native x64 finalizers were compiled with MSVC
`/std:c11 /W4 /WX /MT`; `WIN31LAUNCH.EXE` additionally links `bcrypt.lib` and
the retained OpenNT ABI `pif.h` declarations.  Final build outputs were under
`build/M0-T437/S1/r002-final/`.

Fresh fixtures were copied from the existing recoverable Win31 input only:

- a short-path external fixture was restored to the retail `KRNL386` hash, then launch
  preparation produced candidate hashes, `KRNL386.ORIG`/`WIN386.ORIG`, the
  released driver, `CONFIG.NT`, `AUTOEXEC.NT`, and only
  `WINSTD.CMD/.PIF` plus `WIN386.CMD/.PIF`;
- `tests/component-integration/win31_launch_profile_test.ps1` passed before
  and after an idempotent second launch application;
- a candidate-without-recovery fixture returned `1` and did not create
  `PATCH`;
- a fresh path fixture repaired four `PROGMAN.INI` group locations and
  twenty-two validated PMCC command targets (26 total), retained an unrelated
  stale-looking nonexistent path, preserved a zero group checksum, and was
  unchanged on its second run;
- a malformed PMCC fixture returned `3`, left `PROGMAN.INI` unchanged, and did
  not create `PATCH`.

The actual shipped `APPLY.CMD` files were exercised against that short-path fixture and
returned zero for both Win31 tools.  Empty target input returns `64` and
leaves the prompt window open for all three add-on entrypoints.

## Remaining owner validation

The tools prepare standard (`/S`) and enhanced (`/3`) profiles.  Standard and
enhanced guest launches still require owner runtime confirmation on a selected
installed tree; enhanced preparation is not an enhanced-mode reliability
claim.  The separate queue-tail original Setup transition remains untouched.
