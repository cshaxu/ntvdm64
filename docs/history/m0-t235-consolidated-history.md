# m0-t235 historical record

Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.

## m0-t235-closure-20260821.md

# M0 T235 Closure — Original COMMAND Initial-Input Admission

## Outcome

M0 T235 closes the requested CLI admission boundary.  The fixed runner is
`build/output/ntvdm64-0235.exe`; it reads only sibling `ntvdmcfg.yaml`, which
has no schema or version selector.  The first non-option argument remains the
target path.

## Requirement Audit

| Owner requirement | Evidence and result |
| --- | --- |
| CLI accepts executable input without a `run` subcommand. | Runner target-first policy passes; PE stays a direct host handoff while DOS/COMMAND inputs require the explicit engine. |
| No project profile-version protocol in the runner/native path. | Runner, real native CLI, and DEM startup composition share `ntdos64_config` flat YAML validation.  The existing copied engine descriptor ABI now transports the YAML pathname, not JSON profile meaning. |
| COM, DOS EXE, BAT, and PIF reach the original COMMAND boundary. | `byob_launch_plan_v2` carries all four kinds; the focused original `cmdGetNextCmd` fixture verifies copied input delivery.  The linked native engine's `--validate-only` path accepted identity-checked YAML bundles for all four target suffixes. |
| PIF behavior remains original OpenNT code. | `cmdpif.c` remains the late PIF owner and original `nt_pif.c` remains the parser.  The CLI does no BAT/PIF content parsing. |
| PIF CONFIG/AUTOEXEC timing is correct. | Before session binding exposes COMMAND `54:0C/0D`, the wrapper invokes original `GetPIFData` and original `GetPIFConfigFiles`; the PIF fixture observes selected paths. |
| Fixed output/config contract is verified. | Fresh runner r3 built nine MSVC x64 `/MT` actions and passed runner YAML, duplicate/unknown-key, target-format, and engine-argv policy checks. |
| Native composition is source-built. | Fresh full r5 corrected Bochs/OpenNT include-root ordering, built 137 actions, and linked `bin/ntdos64-native.exe`; its YAML validation was run for COM, EXE, BAT, and PIF. |
| Governance and source hygiene. | Documentation inventory/governance and `git diff --check` pass at closure. |

## Boundaries And Transfer

T235 does not claim a completed guest program run, guest loader, child/parent
lifecycle, redirection lifecycle, or WOW execution.  Those remain the first
queued whole COMMAND launch/PIF/lifecycle package.  This is intentional: the
requested CLI acceptance boundary hands the initial input to original OpenNT
owners without replacing their later runtime semantics.

## Delivered Revisions

- `4674e50d feat(t235): add fixed yaml command input seam`
- `13c90ee5 feat(t235): route native startup through yaml`
- `f273209e build(t235): prioritize bochs headers in native graph`


## m0-t235-s1-p1-closure-20260821.md

# M0 T235 S1 P1 Closure

M0 T235 S1 P1 closes only CLI target-format admission. The target-first grammar
is retained; `.bat` and `.pif` are classified without opening their contents
and fail explicitly until S2's session-owned initial-command transport exists.
No PIF parser, BAT parser, guest write, COMMAND handoff, or PIF startup
configuration behavior entered the CLI.

The isolated MSVC x64 `/MT` Ninja graph in
`D:\tmp\ntdos64-M0-T235-S1-formal-r3` built the runner and engine probe in
nine actions. The existing runner policy fixture and the S1 format fixture
passed. The v3/v4 single-target compatibility projection was restored so a
source-validated legacy target reaches the v2 launch-plan seam; the fixture
now supplies its required paired CONFIG/AUTOEXEC sources.

Detailed source-recovery ledger, format matrix, exact commands, and transfer
to P2--P9 are in [the S1 result](../../artifacts/documentation-archive/20260910/etc/evidence/t235-s1-cli-target-format-admission-result-001.md).


## m0-t235-s4-p1-closure-20260821.md

# M0 T235 S4 P1 closure — fixed YAML cleanup

T235 S4 is closed. The owner directed that fixed YAML consolidation and the
retired JSON `profile.json` mechanism finish within T235.

The accepted product result is one sibling `ntvdmcfg.yaml` configuration:
the JSON parser, old profile CTest, old Bochs argv relay and its policy
fixture, and the JSON-only runtime fixture target are removed. The formal
native module graph no longer compiles `byob_profile.c`. The product-facing
YAML output record is `ntdos64_startup_selection`.

Verification used the final source state: the MSVC `/MT` formal graph linked,
the original OpenNT PIF fixture passed, and the fixed-YAML runner policy and
format-policy scripts passed. See
[the S4 result](../../artifacts/documentation-archive/20260910/etc/evidence/t235-s4-yaml-profile-cleanup-result-001.md).

Historical evidence that literally contains a `profile.json` fixture remains
indexed and non-product; it is neither a product target nor a formal runtime
dependency. This closure does not expand T235's PIF/startup result into a
claim of COMMAND child lifecycle, guest execution, redirection, or WOW
completion.


## m0-t235-s5-closure-20260821.md

# M0 T235 S5 Closure — 2026-08-21

M0 T235 S5 is closed at commit `dafc7e60`.

The delivered executable-relative package is `build/output/ntvdm64-0235.exe`
with sibling `dos/` and `wow16/` directories. It has no JSON or YAML runtime
configuration. Its 115-file origin manifest records 87 OpenNT originals, 26
OpenNT-4.5 originals, and reproducible original-toolchain builds of
`HIMEM.SYS` and `REDIR.EXE`. The final bundle verifier, strict runner COM/PIF
fixture, formal Ninja graph, original PIF fixture, and documentation-governance
verification all passed. Details are retained in
[the S5 bundle result](../../artifacts/documentation-archive/20260910/etc/evidence/t235-s5-guest-bundle-result-001.md).

The package does not claim DOSX/DPMI execution, WOW16 runtime loading, or
redistribution approval; it only supplies the requested currently available
guest binary inventory and its provenance.

