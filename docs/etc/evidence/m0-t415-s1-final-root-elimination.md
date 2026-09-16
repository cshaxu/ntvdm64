# T415 S1 final retired-root elimination

## Question

Did the owner-moved font and `v86/util` inputs, plus the remaining extracted
printer carrier, land at their correct physical owners without changing any
runtime source content or leaving an obsolete MVDM root?

## Inputs

- T414 closure baseline `622b71cda`.
- Pinned OpenNT comparison tree at `O:\repos.external\OpenNT`.
- Current formal generator `tools/build/New-T310OriginalSoftpcNinja.ps1`.

## Procedure

1. Compared all 42 `*.fon` files at
   `src/opennt-host/base/win32/winnls/fontsup/system` against OpenNT
   `base/win32/winnls/fontsup/system` by Git blob identity.
2. Compared `src/mvdm/v86/util/{makefile,quit.asm}` against
   `base/mvdm/v86/util` by Git blob identity.
3. Compared the relocated printer carrier to its T414-baseline blob, checked
   the three retired roots are absent, and generated a fresh formal x86 graph.
4. Checked the graph's direct printer object input and ran diff hygiene.

## Observations

- `Test-T415FinalRootElimination.ps1` passed: 42 fonts, two `v86/util` files,
  one preserved printer carrier, and zero retired roots.
- The printer carrier is byte-identical to the prior carrier, but is an
  extracted standalone fragment of OpenNT
  `base/mvdm/v86/monitor/i386/monitor.c`, not a complete original translation
  unit. It therefore belongs at
  `src/mvdm-overlay/v86/monitor/i386/monitor_printer.c`.
- The fresh graph selects that exact overlay path for
  `obj/kernel-vdm/monitor_printer.obj`, then `kernel-vdm-printer.lib`, and
  links the library into `ntvdm.exe`.
- The static dry-run resolves 465 worker actions. Actual Ninja invocations,
  including the one-target printer-library attempt, remained at zero CPU with
  no `cl.exe` child and no output before the first command; both were stopped.
  This is a local Defender/process-launch block, not a compiler or source
  diagnostic. Consequently, no actual T415 link, executable test, or runtime
  publication is claimed.
- `git diff --check` passes.

## Interpretation

The physical relocation is complete and mechanically proven. The sole direct
production consumer is repaired. Historical T-numbered generators and audits
still name their then-current pre-T414 roots as retained evidence; they are not
production generators and were deliberately not rewritten.

## Follow-up

After local compilation is permitted again, run the admitted formal x86
three-program build and its printer-containing worker link from
`build/M0-T415/S1/formal-x86`; only then may T415 close. No `O:\winnt`
publication is needed because this task changes only source placement.
