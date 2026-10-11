# M0 T443 S6 — Hook CreateProcess Boundary

## Question

Can the two Hook widths select a DOS/Win16 candidate from the original
`CreateProcess(application, command)` pair without taking over Run16's
single-string command-line compatibility contract or changing native creates?

## Inputs and source basis

- `src/nthook32-dll/create_process.cpp` is the shared Hook implementation for
  the two widths.
- `src/run16-exe/main.c` remains the sole owner of `run16 <target> <args>`:
  CWD/PATH and COM/EXE/BAT/PIF search, error distinction, and the compact
  `command/c` normalization are unchanged.
- `src/opennt-host/base/win32/client/vdm.c`, `BaseCheckVDM`, is the retained
  mirror source for the three-branch DOS-tail shape. No new OpenNT source was
  imported and no mirror file changed.

## Disposition

The Hook no longer calls `common_resolve_application()`. That resolver has a
Run16-only one-field input contract and must not decide a CreateProcess pair.

For a Hook request, an explicit application selects only that application
field. With no application field, the Hook tests the public bounded
CreateProcess command-line candidates solely to classify a discovered image.
If the discovered image is DOS/Win16, it launches Run16 with the selected
image and the tail selected by the existing OpenNT `BaseCheckVDM` rule. If it
is not DOS/Win16, the original application, command, environment, directory
and flags reach native CreateProcess unchanged; Hook propagation is retained.

This deliberately does **not** make the Hook a second Run16 resolver. In
particular, `run16 command/c ver` continues through unmodified Run16 logic:
its one-field resolver finds `command`, separates the attached `/c`, normalizes
the command to `command /c ver`, and only then calls `BaseCheckVDM`.

## Focused observations

Build root: `build/M0-T443/S6/r001/`.

| Check | Result |
| --- | --- |
| x86 `nthook32.dll` and installation fixture rebuild | pass |
| AMD64 `nthook64.dll` and installation fixture rebuild | pass |
| x86 Hook installation fixture | pass, 104 assertions |
| AMD64 Hook installation fixture | pass, 104 assertions |
| Run16 launch-options fixture | pass, 18 cases, including `command/c ver` preservation |
| extended DOS image path (> `MAX_PATH`) in both Hook fixtures | pass |
| formatting gate | `git diff --check` pass |

The Hook fixture creates a temporary two-byte DOS image and verifies each
width for: (1) application-only selection with an empty tail; (2) both fields
where the explicit application wins despite a different `argv[0]`; (3)
command-only quoted selection; (4) ANSI both-field conversion; and (5) an
extended absolute DOS image path longer than `MAX_PATH`. Its private Run16
stand-in observes the selected absolute image and expected tail.

The final focused rerun rebuilt the x86 fixture and ran both widths: x86
`nthook-install-test.exe` passed with 104 assertions, AMD64
`nthook-install-test.exe` passed with 104 assertions, and the x86 Run16
launch-options fixture passed its 18 cases. The latter contains the literal
`command/c ver` request, so this Hook-only split has an explicit guard against
changing Run16's compact-option CLI behavior.

The first AMD64 test execution initially returned `ERROR_FILE_NOT_FOUND`: the
Hook-only build did not stage the ordinary x86 `run16.exe` beside the fixture,
which the existing Hook context identifies as its launcher. The Ninja graph
now stages that same task-owned build input; a clean re-run passed. This was
test-package assembly, not Hook redirection behavior.

## Limits and follow-up

This is a focused Hook boundary proof, not a new implementation of all
Kernel32/CreateProcess parsing or a full interactive product regression. The
existing Run16 integration suite remains the owner of real DOS command launch
and service/worker lifecycle coverage. The Hook now uses a bounded
32768-character temporary selection buffer and still rejects a redirect whose
complete Run16 command would exceed CreateProcess's 32767-character command
line contract; it does not alter a guest path structure.

## Delivery gate status

`assets/winnt.zip` is a reproducible host-package baseline, not a substitute
for the mutable installed runtime.  The WOW guest fixtures (`WINMINE.EXE`,
`SOL.EXE`, and `WRITE.EXE`) are external inputs and must be referenced directly
from `O:\winnt`; they must not be copied into a candidate `System32` directory
or used to justify copying the installation tree.  `NTVDM.REG` is not present
in the current installed package and is not an S6 fixture.

The earlier candidate matrix used a nested temporary SystemRoot.  A real-root
control showed `run16 COMMAND.COM` returns the same `ERROR_INVALID_PARAMETER`
there with both the old and the candidate Hook pair, so that observation cannot
attribute a failure to S6.

The authoritative runtime check instead used the real `O:\winnt` root.  It
passed both of these requests with the selected candidate Hooks:

1. `run16 command /c ver` printed `MS-DOS Version 5.00.500` and returned.
2. `run16 cmd.exe /d /c "command /c ver"` printed the same result and returned.
   This second route starts native CMD and exercises its injected Hook redirect
   into the selected COMMAND image.

The coherent ten-image set in `r024-candidate` was then copied, after a
recoverable ten-file backup at `build/M0-T443/S6/r035-publish-recovery`, to
both `assets/release` and `O:\winnt\system32`.  Its exact ten-image SHA-256
manifest is `build/M0-T443/S6/r035-published-manifest.json`; the post-publish
native-CMD-to-COMMAND smoke repeated successfully.  No guest fixture was
copied.  Publication is complete; documentation/governance review, commit and
push remain before S6 closure.
