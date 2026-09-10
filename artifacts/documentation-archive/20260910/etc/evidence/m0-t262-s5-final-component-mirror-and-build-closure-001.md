# M0 T262 S5 — final component-mirror and formal-build closure

## Question

After the owner-authorized original guest binary/object import, do
`bx-core`, `opennt-guest` and `opennt-host` still contain only allowed mirror
classes, and does the formal component graph remain buildable?

## Inputs and procedure

- Compare every non-README `bx-core` path and SHA-256 to
  `O:\repos.external\bochs-2.6-compat\bochs-2.6`.
- Compare every non-README guest path under `dos-v86` by same relative path
  and SHA-256 to either `O:\repos.external\OpenNT\base\mvdm\dos\v86` or
  `O:\repos.external\OpenNT-4.5\nt\private\mvdm\dos\v86`; compare `wow16`
  to `refs/opennt/base/mvdm/wow16`.
- Compare the eleven declared host paths to their named OpenNT origins; scan
  every retained C/header body for `HOST-DIV-*` and compare that set to the
  component README register.
- Generate a fresh formal graph at `build/M0-T262-S5/001` with
  `New-T260S8FullNinjaGraph.ps1`, then run `ninja -C` outside the sandbox.

## Observations

- `bx-core`: 182 source files total; 164 byte-identical Bochs files and 18
  minimally edited Bochs files. All 18 have a local `DIVERGENCE(...)` marker
  and a corresponding `bx-core/README.md` index entry. No project-authored or
  non-mirror source remains.
- `opennt-guest`: 678 input files total: 676 DOS/V86 and two WOW16 files.
  Every file matches a same-relative-path original peer. The 99 retained
  DOS/V86 package/build inputs are individually recorded in
  `m0-t262-s3-opennt-guest-binary-import-001.md`; no guest divergence exists.
- `opennt-host`: 11 production files total: seven exact originals and four
  named reachable subsets/minimal edits (`nt_error.c`, `vrinit.c`,
  `vrnmpipe.c`, `vrputil.c`). The code and README sets both contain exactly
  `HOST-DIV-012` through `HOST-DIV-024`; there is no extra host production
  file.
- The fresh x64 `/MT` CPU5 formal graph completed its 469 compile/archive/link
  edges and emitted 79 fixtures plus one `ntdos64-native` target.
- `Test-BochsUdInterceptBoundary.ps1 -RepositoryRoot O:\repos.hobby\ntdos64`
  passes, and `t237-s5-bop-nosupport-fixture.exe` exits zero.
- `t251-s3-redir-ingress-fixture.exe` exits `8` at its mailslot regression.
  The same executable in the earlier build root `build/M0-T261-S12/002`
  returns the same `8` in this environment. This is a pre-existing
  runtime/environment limitation of the Redirector fixture, not a change in
  the imported host mirror or a failed link closure.

## Interpretation

The three components satisfy T262's mirror standard: exact original, named
original subset/minimal edit with local/indexed divergence, and no
project-authored fourth category. The build regression proves current source
composition, not guest runtime completion. The environment-sensitive
Redirector runtime result remains owned by that fixture/provider package and
does not authorize a source rewrite in this mirror-only task.
