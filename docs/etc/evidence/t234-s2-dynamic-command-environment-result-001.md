# M0 T234 S2 — Dynamic COMMAND Environment Composition Result

## Question

Can the v2 OpenNT COMMAND mirror remove its fixed 1024-byte session-environment
limit while retaining the original `cmdenv.c` owner, merge order, and failure
rules?

## Inputs

- OpenNT mirror: `src/bx-vdm/bop/opennt/command/cmdenv.c`, specifically
  `cmdXformEnvironment`, `cmdCreateVDMEnvironment`, `cmdSetEnvironmentVariable`,
  `cmdExpandEnvironmentStrings`, and `cmdGetEnvironmentVariable`.
- Session seam: `src/bx-vdm/bop/shim/command_misc_shim.{h,c}`.
- Formal source membership: `tools/build/t225-s7-full-module-manifest.json`.

## Procedure

1. Admitted the existing original `cmdenv.c` dynamic bodies instead of the
   former shim-only memcpy implementation.
2. Replaced both fixed session multisz arrays with session-owned dynamically
   allocated copied multisz values, plus explicit dispose/reset ownership.
3. Replaced the separate `54:08/0A` 1 KiB guest environment probe with a
   bounded, exact double-NUL guest multisz copy (maximum `USHRT_MAX` bytes).
4. Preserved the original `DWORD` count fields and documented the required
   modern warning/length conversions inline.  The historical 512-byte growth
   increment now grows by at least one complete input string, because a valid
   modern PATH can exceed that historical per-string assumption.
5. **P2:** admitted the original `cmdXformEnvironment` body. Its historical
   process-environment snapshot is now a copied Unicode view of the active
   session multisz; its RTL environment calls use one private compatible shim
   while retaining the original filtering, replacement and sort algorithm.

## Observations

- `t234-s2-command-dynamic-environment-fixture.exe` exited `0`: a source
  environment above 1024 bytes is copied into session storage, the original
  merge returns a result above 1024 bytes, inserts the supplied COMSPEC,
  filters WINDIR in the DOS case, and expands an AUTOEXEC pair.
- Updated `t231-s7-command-get-next-direct-fixture-s2.exe` exited `0`: the
  original `cmdGetNextCmd` capacity/retry route requests an environment above
  1024 bytes, preserves it across repeat state, and writes the merged value
  back to guest memory.
- Updated `t231-s8-command-lifecycle-direct-fixture-s2.exe` exited `0` with a
  1306-byte guest environment.  The Direct child lifecycle keeps its existing
  result and standard-handle behavior.
- The changed `cmdenv.c`, `cmdmisc.c`, `command_misc_shim.c`, and the two
  fixtures compiled under MSVC x64 `/MT`, `/W4 /WX`.
- **P2:** `command_environment_shim.c`, the newly admitted transform body,
  and the focused dynamic-environment fixture compile under the same flags.
  Its link/run remains pending because the shared formal Ninja root is held
  by existing Ninja processes.

## Formal Build Limitation

A fresh formal Ninja graph was generated at `build/M0-T234-S2/formal-r2`.
In this session its Ninja executor starts the first object and then makes no
child-compiler progress, even when the parent MSVC environment is preloaded.
The disposable run was stopped after verifying no compiler/linker child was
active.  This is not treated as a source-build failure or as a formal build
pass.  The direct MSVC compilation/link commands above are only focused local
verification; formal Ninja closure remains required by the active packet.

## P3 — `cmdSetDirectories` binding closure

The imported OpenNT body in
`src/bx-vdm/bop/opennt/command/cmdmisc.c::cmdSetDirectories` was already
admitted. `command_misc_shim.c` nevertheless still exported an identically
named no-op, leaving the final archive/member selection ambiguous. The
duplicate definition was removed; no replacement implementation was authored.

This retains the first recovery rung: the original COMMAND owner, its
`CurDrive` / `CurDirectory` ordering, and its traversal of `=X:` multisz
entries. The existing COMMAND shim remains only the named compatibility seam
for the unavailable CCPU/SAS include closure. No Bochs intrusion, adapter
rewrite, or new BOP behavior was used.

Focused verification used a fresh `build/M0-T234-S2/formal-r5` graph. MSVC
x64 `/MT` compiled the changed imported translation unit, shim, and isolated
`t234_s2_command_set_directories_fixture.c`. The fixture then linked with the
already verified r4 `bx-vdm` dependency archive and the standard formal
platform libraries. Its link map resolves `cmdSetDirectories` specifically to
`formal-r5/focused/cmdmisc.obj`, not to an archive member. The fixture exited
zero after confirming both: (1) the selected drive's `=X:` value and current
directory are synchronized from `VDMINFO`; and (2) a distinct inherited
`=Y:` entry from the supplied multisz survives the original loop. The fixture
runs in its own process, so neither its current-directory nor environment
updates persist in the invoking shell.

The r5 Ninja graph was generated successfully, but Ninja again retained an
idle parent without a compiler/linker child; it was stopped. This is a
formal-Ninja verification limitation, not a passing formal build. The source
and focused link/run facts above close the duplicate-symbol ambiguity only.

## P4 — Local-NTFS formal Ninja resolution

The owner approved a disposable local NTFS alternative after `cmd.exe` proved
that the repository-local `O:` build root could not execute even a one-edge
Ninja probe: Ninja remained idle without creating its `cmd.exe` child, while
that exact child command succeeded when entered directly. This is a
root-specific build-host execution observation, not an OpenNT or MSVC failure.
The observation does not establish that `O:` is a mapped drive.

`D:\tmp\ntdos64-M0-T234-S2-formal-r7` was generated from the same formal
manifest, with the Visual Studio supplied Ninja selected explicitly. From
`cmd.exe`, after `VsDevCmd.bat -arch=x64 -host_arch=x64`, Ninja completed all
79 edges needed by `bin/t234-s2-command-set-directories-fixture.exe`, including
the fresh `bx-vdm.lib` containing the imported `cmdmisc.c` and its revised
shim. The generated fixture then exited zero and printed:

```text
T234 S2 OpenNT cmdSetDirectories synchronization verified
```

This is the formal Ninja positive result for the P3 binding closure. The
local root is disposable build output only; source, manifests, evidence, and
release inputs remain in the repository. The prior `O:` root limitation is
retained as a superseded host-execution observation, not a product failure.

## P5 — `cmdUpdateCurrentDirectories` binding closure

The OpenNT `cmdmisc.c::cmdUpdateCurrentDirectories` body is now directly
admitted.  It retains the source algorithm exactly: selected-drive entry
first, physical-drive classification, per-drive `=X:` lookup, bounded growth,
the final extra NUL, and the original allocation failure exits.  The prior
same-named no-op in `command_misc_shim.c` was removed.

Its terminal `SetVDMCurrentDirectories` call cannot directly link: in OpenNT
it is a Win32 client wrapper that packages the double-NUL bytes for BaseSrv/CSR
and a console-bound VDM.  `command_misc_shim.c` therefore supplies the sole
source-derived seam.  It copies the published bytes into the active COMMAND
session and frees them on session disposal; it does not alter the imported
directory algorithm, inspect guest memory, or create a BaseSrv dependency.

The existing formal `t231-s8-command-lifecycle-direct-fixture` was extended
to seed the selected drive via the directly admitted `cmdSetDirectories`, then
take the original `54:0B` return path.  In
`D:\tmp\ntdos64-M0-T234-S2-formal-r7`, MSVC x64 `/MT` Ninja rebuilt the 18
affected edges, including `cmdmisc.c`, the COMMAND shim, `bx-vdm.lib`, and the
fixture.  The produced fixture exited zero and verified the session-owned
double-NUL output begins with the selected `=X:` entry and has its required
terminator.  This is a local COMMAND owner regression; it makes no native
guest-continuity claim.

## P6 — `cmdMapCodePage` source binding closure

The former shim was a direct `ULONG`-to-`USHORT` cast.  It omitted the sole
special case in OpenNT `cmdmisc.c::cmdMapCodePage`: the historical
US-only `1252` Windows code page must be returned to DOS as `437`.  The
original body is now independently admitted from the COMMAND mirror and the
same-named shim definition is removed.  No new mapping table or modern
locale policy was introduced; non-1252 inputs retain OpenNT's original
`USHORT` conversion.

The formal `t231-s7-command-get-next-direct-fixture` now passes `1252` through
the real `54:01`/`cmdGetNextCmd` handoff and verifies the guest `CMDINFO`
receives `437`.  Its environment retry/re-entry case supplies `932` and
verifies the guest receives `932`.  In
`D:\tmp\ntdos64-M0-T234-S2-formal-r7`, MSVC x64 `/MT` Ninja rebuilt all five
affected edges (`cmdmisc.c`, shim, `bx-vdm.lib`, fixture object and executable)
and the fixture exited zero.  This validates the original function's two
defined branches without making an unsupported broader code-page claim.

## Interpretation And Confidence

The product route no longer has a 1024-byte COMMAND environment storage or
child-launch limit. It now directly reuses the original OpenNT transform,
merge, lookup, set and expansion algorithms, with a bounded session/guest-
memory seam only where historical BaseSrv and SAS composition cannot link.
P2 source compilation is positive; its fixture link/run and formal graph are
still verification gates.

## Follow-Up

After existing Ninja processes release the formal root, run the P2 fixture and
affected formal graph, then the documentation gate before accepting T234 S2
closure.
