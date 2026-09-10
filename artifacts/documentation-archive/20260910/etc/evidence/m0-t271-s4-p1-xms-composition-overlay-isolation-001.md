# M0 T271 S4 P1 — XMS composition overlay isolation

## Question

Can the project-authored XMS package configuration/call composition leave the
`opennt-bop` mirror without altering the original `xms.486` service table or
provider bodies?

## Inputs

- Pinned baseline: `O:\repos.external\opennt\base\mvdm\xms.486\{xms.c,xmsdisp.c,xmsa20.c,xmsblock.c,xmsmisc.c,xmsumb.c}`.
- Active component files before this part:
  `src/opennt-bop/xms/opennt_xms_composition.c` and the original-family XMS
  files named above.
- The source-first and mirror-component rules in
  `docs/etc/operations/policy/source-policy.md` and `docs/rules/CODING.md`.

## Procedure

1. Compared the active XMS source table and providers with the pinned package.
2. Classified `opennt_xms_composition.c` as project-authored lifecycle glue:
   it validates a copied call, owns configuration/reset state, binds scoped
   SoftPC mechanics and invokes `XMSDispatch`; it is not an OpenNT translation
   unit or true subset.
3. Moved that body to `src/opennt-bop-overlay/xms/` and made the original
   owner-family path a single registered include boundary.
4. Registered `BOP-DIV-107` in both component registers.
5. Generated formal Ninja root `build/M0-T271-S4/r001` and ran the two
   existing XMS source fixtures.

## Observations

- `xmsdisp.c` retains its original 12-entry `apfnXMSSvc` ordering and all
  providers remain in their original-family files.
- The only production caller of the new overlay body is
  `src/opennt-bop/xms/opennt_xms_composition.c`; the overlay has no public
  include or link surface.
- `t237-s2-xms-source-mirror-fixture.exe` and
  `t237-s3-xms-a20-source-mirror-fixture.exe` both returned zero from the
  formal MSVC x64 `/MT` root.

## Interpretation

This closes only the first structural correction: it removes the largest
unregistered project-authored XMS package body from the mirror root without
claiming that remaining XMS session/selector glue is already correctly owned.
The next S4 part must separately classify the copied-frame XMS route, the
session binding and each `GetVDMAddr`/A20/UMB mechanical call against the
shared mapping-manager and same-shaped `adapter-softpc` facades.
