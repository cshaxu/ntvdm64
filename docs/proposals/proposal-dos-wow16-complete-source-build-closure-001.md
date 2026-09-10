# DOS/WOW16 Complete Source-Build Closure Proposal

## Rebootstrap owner note

The canonical selected DOS and WOW16 trees are complete load-only guest mirrors
under `opennt-guest-dos` and `opennt-guest-wow16`. Their objects and libraries
never enter either x86 or x64 host link; source-build results are alternative
manifested guest products only.

## Decision Requested

Admit one future owner package which establishes an independently reproducible
original-source build route for every artifact in the fixed 115-file guest
bundle (`dos/` and `wow16/`).  Original OpenNT/OpenNT-4.5 prebuilt binaries
remain the preferred packaging inputs where present; this package closes the
separate provenance requirement that every packaged artifact can also be
rebuilt from its original source and declared historical toolchain.

This is an unnumbered candidate until selected and admitted to `STATUS.md`.

## Baseline And Scope

T235 S5's manifest has 115 staged files: 87 `opennt-prebuilt`, 26
`opennt-4.5-prebuilt`, and two `source-built` fallbacks (`HIMEM.SYS` and
`REDIR.EXE`).  The retained DOS/WOW source-closure inventory currently holds
27 historical `source-built` records.  Twenty-six correspond to a current
package artifact; `QUIT.COM` is a historical source-build record but not a
member of the 115-file package.

The 26 package artifacts with prior source-build evidence are:

```text
ANSI.SYS       APPEND.EXE      COMMAND.COM     COUNTRY.SYS
DEBUG.EXE      DOSX.EXE        EDLIN.EXE       EXE2BIN.EXE
FASTOPEN.EXE   GRAPHICS.COM    HIMEM.SYS       KB16.COM
KEYBOARD.SYS   LOADFIX.COM     MEM.EXE         NLSFUNC.EXE
NTDOS.SYS      NTIO.SYS        REDIR.EXE       SETVER.EXE
SHARE.EXE      COMM.DRV        VGA.DRV         KEYBOARD.DRV
MOUSE.DRV      SOUND.DRV
```

The remaining 89 artifacts currently have a verified original-prebuilt route
only.  That classification means no reproducible source-build result is
currently recorded; it is not evidence that the original source is absent or
unbuildable.

## Boundary

This is guest-artifact provenance and historical-build recovery work.  It may
recover original build descriptors, build shims and toolchain invocations, but
it must not substitute new DOS, WOW16, COMMAND, DPMI, or driver behavior.
The final artifact must remain owned by its original OpenNT component.

`refs/opennt`, the admitted `src/opennt` tree, and the approved local
OpenNT-4.5 tree are evidence/input roots under source policy.  No later
repository, downloaded binary, Windows installation file, or untracked tool
may become an acceptance input.

## Required Ledger Per Artifact

For each of the 115 destinations, record:

1. bundle destination, original product/prebuilt provenance and SHA-256;
2. original source owner, exact source and makefile/descriptor paths and
   source revision;
3. historical toolchain/packer/resource dependencies and their approved local
   provenance;
4. a fresh, disposable build recipe under `build/<admitted-task>/<run>/`;
5. resulting file SHA-256, size and PE/NE/DOS/static-data identity facts;
6. byte-identical status to the original product file, or a bounded
   source-derived reason for an expected difference; and
7. positive build and negative missing-input/tool failures.

The source-recovery ledger applies to every missing build route: directly
reusable original makefile/source first, then the smallest build shim retaining
that source contract, then a registered exceptional external-code intrusion,
and no authored replacement behavior unless the earlier rungs are proven
unavailable.  A binary copy, a decompilation, or a modern compiler rewrite is
not source-build closure.

## Proposed Sequential Deliveries After Admission

### P1 — Complete Inventory Reconciliation

Reconcile the T235 manifest against the full OpenNT and OpenNT-4.5 descriptor
census.  Partition all 115 artifacts by source owner and declared build graph,
including static data and files historically placed by `binplace`.

**Acceptance:** a one-to-one destination ledger has no unclassified artifact
and distinguishes source absence, descriptor absence, unavailable historical
tool and attempted-but-failed build.

### P2 — DOS/NTDOS Build Closure

Recover all remaining DOS system, command/tool, driver, font, keyboard/NLS and
redirector build routes with the original source and the smallest documented
historical build shims.

**Acceptance:** every `dos/` artifact source-builds in a fresh disposable
root; its result is compared to the packaged original when one exists.

### P3 — WOW16 Build Closure

Recover every staged WOW16 driver, runtime, static resource and support-file
route using original WOW16 source/descriptors and the required original
16-bit resource/link/placement tools.

**Acceptance:** every `wow16/` artifact source-builds in a fresh disposable
root; descriptor-only placement is explicitly resolved rather than copied.

### P4 — Full Reproducibility Matrix

Run clean-room sequential rebuilds of all 115 artifacts, regenerate the
per-artifact origin/build manifest and verify that the package staging tool can
select original prebuilt material while the ledger proves a source-build route
for every file.

**Acceptance:** 115/115 source-build success records, source/tool hashes,
positive/negative build checks, staging-manifest verification, documentation
governance and focused format identity checks all pass.

## Exit Rule

The package may claim complete guest-binary source-build closure only if all
115 packaged artifact destinations have a reproducible original-source build
route and a recorded fresh result.  Any remaining prebuilt-only artifact,
undocumented tool dependency, untracked tool output, or newly authored
functional replacement keeps the package open and is reported by exact
destination and first blocker.

## Non-Goals And Stop Conditions

- Do not change runtime artifact selection merely to prefer new builds;
  original prebuilt packaging precedence remains unless separately approved.
- Do not broaden guest runtime behavior or use this build work as evidence of
  COMMAND, WOW16, DPMI, Redirector, XMS, or BOP semantic closure.
- Pause and re-admit if an artifact needs a source tree or historical tool not
  already approved by source policy, if a license/distribution decision is
  required, or if the proposed workaround would replace original guest logic.
