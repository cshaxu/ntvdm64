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
- Direct Ninja completion notification hangs in this Codex host after the
  first child exits, although its exported command is valid. The formal graph
  was therefore exported with `ninja -t commands` and executed sequentially
  in one `VsDevCmd -arch=x86 -host_arch=x64` command session. All 478 formal
  graph commands completed, including the printer library and final worker
  link. The link's `Verify-VdmTibStorage.mjs` gate passed with a 4,208-byte
  `mvdm_vdm_tib.obj` owner. The resulting x86 products are `run16.exe`
  (256,000 bytes), `basesrv.exe` (266,240 bytes), and `ntvdm.exe`
  (3,250,688 bytes).
- `git diff --check` passes.

## Interpretation

The physical relocation is complete and mechanically proven. The sole direct
production consumer is repaired. Historical T-numbered generators and audits
still name their then-current pre-T414 roots as retained evidence; they are not
production generators and were deliberately not rewritten.

## Follow-up

No runtime publication is needed because this task changes only source
placement. The direct-Ninja completion-notification defect is local build-host
debt; the formal graph and all of its actual commands have been executed.
