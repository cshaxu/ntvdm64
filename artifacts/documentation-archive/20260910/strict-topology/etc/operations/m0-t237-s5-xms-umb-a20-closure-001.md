# M0 T237 S5 — XMS UMB/A20 checked-state closure

## Question

Can the imported OpenNT `52:06` `xmsInitUMB` path retain its original A20
ordering without carrying `GetVDMAddr`'s raw host pointer across the modern
Bochs/VDM boundary, while `52:07` and `52:08` keep their original empty-UMB
failure results?

## Inputs

- OpenNT owners: `src/opennt/base/mvdm/xms.486/xmsumb.c` and `xmsa20.c`.
- Active mirrors: `src/bx-vdm/bop/opennt/xms/{xmsumb.c,xmsa20.c}`.
- Compatibility seam: `src/bx-vdm/bop/shim/xms_shim.{c,h}` and the existing
  selector-blind checked-RAM/A20 mantle interfaces.
- Formal membership: `tools/build/t225-s7-full-module-manifest.json`.

## Procedure

1. Compare the OpenNT `xmsInitUMB`, `xmsEnableA20Wrapping`, and
   `xmsDisableA20Wrapping` ordering with the active mirror.
2. Bind the original `AX:BX` HIMEM state-byte address through one checked
   guest read before dispatch; retain only that fixed-width address in the
   scoped XMS call state.
3. Publish the existing source A20 transitions by checked guest writes.  No
   raw VDM pointer, UMB policy, allocation table, or selector/service term
   enters bx-core or bx-mantle.
4. Generate and execute a fresh MSVC x64 `/MT`, CPU5/P-MMX formal Ninja graph:
   `New-T225S7FullNinjaGraph.ps1 ... build/M0-T237-S5/formal-r7`, followed by
   `ninja -C build/M0-T237-S5/formal-r7 -j 4`.
5. Execute `t237-s2-xms-source-mirror-fixture.exe`,
   `t237-s3-xms-a20-source-mirror-fixture.exe`,
   `Test-T237S3XmsV1Retirement.ps1`, and
   `Test-T237S4MachineSemanticBoundary.ps1`.

## Observations

- `52:06` reaches the imported `xmsInitUMB` body.  Its `AX:BX=0200:0010`
  state byte is initially published as `0`, then changes to `1` after the
  original A20-on branch and back to `0` after the original A20-off branch.
- The source query branch retains both writes: it reports `AX=0` and clears
  `BL`; the fixture verifies the corresponding AX/BX result mask.
- The source fixture preserves empty-map `52:07`/`52:08` results `B1`/`B2`.
- The fresh formal graph compiled all 349 declared edges.  The two fixtures
  and both governance gates returned zero; the machine-boundary gate confirms
  that formal core/mantle sources contain no BOP/service vocabulary.

## Interpretation and confidence

This closes T237's source/ABI scope.  The source bodies, ordering, and empty
map failures are active, and the only necessary divergence is explicit:
OpenNT's retained host pointer becomes a checked, fixed-width guest address.
Confidence is high for the local source route and its formal build.  It does
not prove continuous guest execution or successful UMB allocation.

## Follow-up

`52:07`/`52:08` success still requires a separately admitted selector-blind
physical-span/reservation capability from the bx machine/BIOS owner. `52:09`
still requires the keyboard/IVT `UpdateKbdInt15` owner.  Neither is a reason
to add XMS/UMB/DOS policy to bx-core or bx-mantle.
