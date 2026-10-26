# M0 T460 S5 — Win95 Start Menu PIF-template provenance

## Question

Does the build-only Start Menu plan truthfully record the file that supplies
the MS-DOS Prompt candidate, and can the released PIF editor update dispatch
fields without losing the template's Win95 memory profile?

## Scope and inputs

This S changes only the project-owned `tools/win95-launch` producer, its
fixture test, README, and the adjacent released-utility manifest assertion.
`books/` is expressly excluded. No installed tree, guest media, runtime image
or release binary was changed.

The plan's menu PIF is still recorded as the original Start Menu source for
backup/recovery provenance. It now separately records `templateSource` and
`templateSha256` for `DOSPRMPT.PIF`, which is the file copied to create the
candidate. The candidate hash remains the installation authority.

`STDPIF::defpath` is a 64-byte OEM field. The released editor's original
strict NUL-fit check leaves at most 62 encoded bytes for a selected Windows
root. Preparation now rejects an overlong root before it creates a candidate;
this is a PIF-format boundary, not an arbitrary host-path restriction.

## Procedure and results

The isolated `win95_start_menu_template_test.ps1` creates a test-only extended
PIF through the released editor, supplies the documented Win95 standard and
Windows-386 memory values, and invokes the real plan producer against a
non-default Windows root. It verifies original menu-source identity, template
identity, candidate target/directory, every unchanged non-dispatch PIF field,
and the overlong-root refusal.

`build/t460s5-regressions-r2` passed:

- `WIN95_START_MENU_TEMPLATE_OK menu_source=1 template_provenance=1 profile_preserved=1 pif_directory_limit=1 installation=0 fixture_only=1`
- `WIN95_LAUNCH_COMMAND_SAFETY_OK ...`
- `WIN95_LAUNCH_INSTALL_OK ... fixture_only=1`
- `PASS PIF create/show/update and HASH single-file output`

The PIF/HASH test no longer hard-codes the historical utility count. It now
requires the named `pif` and `hash` entries and verifies the architecture,
existence and SHA-256 of every manifest-listed utility, so adding a legitimate
utility cannot turn a passing release manifest into a false failure.

## Disposition

S5 is a source/tooling-only delivery. It publishes no new runtime package and
does not expand T460's Clipboard, device, guest or Win95-runtime claims.
