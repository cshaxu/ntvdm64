# T484 S3 P145 hook ownership audit reconciliation

## Scope and changes

B0227-B0230 are standalone read-only audits. They invoke Ninja inspection
tools or read maps; none owns a compiler, generator or release path. Preserve
their current boundary contracts, not the historical object/cache spelling.

Context/installer checks now share Get-ProductCompilerDependencies with the
memory audit. The shared helper selects a compiler producer inside the actual
product closure and requires VALID dependency evidence. This replaces copied
old object-name logic; the memory audit is rerun after extraction.

The nonworker audit now understands native path separators, CMake implicit
static inputs and compiler producers behind object-copy nodes. Image import
libraries do not recursively acquire executable implementation sources.
Stale dependency blocks are not merged into the previous valid block, and
zero compiler coverage is rejected. The negative control selects the real
compiled hook entry, not a retired object alias.

The product-map audit now defaults to the current native cache instead of a
T475 experimental tree. All five payload/startup-provider requirements and
Detours negative controls are retained. No live tool/test callers of these
four standalone entrypoints required adjustment.

## Verification

Raw runs: build/t484-s3-hook-audits. Initial context check failed on its old
object path (PID32300); final shared-helper run PID33616 exits0: launcher
context, both worker installers and memory regression pass, including six
private-header negative controls. PID25416 exits0:104 DLL compiler-object
instances plus injected private header control, five actual product maps and
five Detours controls pass. These are static contract checks, not a Full run.

[Receipts](../../../artifacts/build-migration/t484/s3-hook-audits/result.json).
No production source, build graph, guest or release image changed. No rebuild
or republication is claimed for this audit-only batch. S3 continues at B0231.
