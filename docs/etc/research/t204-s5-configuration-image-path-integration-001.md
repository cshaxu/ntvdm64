# T204 S5 P1 — configuration image/path integration

## Delivered composition seam

The startup configuration provider is now the unique owner of generated
CONFIG/AUTOEXEC image bytes whenever it is bound:

- `bx_ntvdm_boot_namespace_provider_v1_bind_startup_configuration` builds one
  ready provider, then makes the two namespace entries borrow its two image
  arrays.  No second copy of either image is created.
- The existing COMMAND boot-input record is rebuilt from the provider-owned
  namespace paths and copied into the COMMAND package through one explicit
  session setter before bind.
- `bx_ntvdm_boot_namespace_composition_v1_set_startup_configuration` is the
  only composition admission seam.  It rejects a bound composition, a second
  configuration bind, invalid input and every non-ready provider result.
- The existing `54:0C/0D` helper remains purely mechanical: it writes the
  copied paired paths only.  It neither knows transform semantics nor owns the
  generated image bytes.

The composition regression binds a ready provider, checks that both namespace
entries point directly at the provider image arrays, deliberately mutates the
mutable namespace path strings after composition bind, then verifies the
COMMAND copy still supplies the selected CONFIG and AUTOEXEC paths.  This
preserves COMMAND's historical copied-path lifetime while eliminating the
provider-data duplication.

## Build evidence

`tools/Invoke-T198S16BootNamespaceVdmX64Probe.ps1` was updated to include the
new provider/policy sources and the pre-existing normal-return observer needed
by the current generic bridge.  The first r1 attempt is retained negative
evidence: it compiled but did not link because that existing observer source
was absent from the tool's source closure.  r2 produced
`artifacts/build/t204-s5-configuration-integration-r2/t198-s16-bx-vdm-boot-namespace.exe`.
Direct execution of that fresh MSVC x64 `/MT` executable returned zero.

The fixture includes the complete current boot-namespace composition closure;
it does not execute Bochs or a native guest trace.

## Remaining S5 condition

This P deliberately does **not** claim startup execution closure. The later
S5 adapter-installation path supplies the admitted paired provider input, and
namespace initialization now retains only CONFIG/AUTOEXEC path declarations
and metadata rather than legacy synthetic bytes. The migrated
`bx_ntvdm_boot_namespace_provider_v1_test` binds a contained provider before
opening CONFIG, and its strict MSVC x64 `/MT` build/run passes.

Therefore S5 remains active pending the complete affected-fixture sweep and
the full runtime closure. It must not regress by hard-coding new bytes, adding
a BOP special case, or making a host temporary file.
