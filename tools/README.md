# Tools

Tool source and operator scripts live here. Generated objects, listings,
logs and probes belong under repository build/. Independently compiled
add-on utilities and drivers are published once under assets/release, with
source and output identity in addon-manifest.json. Vendored original tools
are provenance-bound inputs, not project-generated release copies.

## Owner-designated operator directories

| Directory | Responsibility |
| --- | --- |
| opennt | Retained original tools requested by the owner. |
| win31-path | Discover and reversibly repair installed Win3.1 paths. |
| win31-setup | Prepare a recoverable media copy and generate launch profiles. |
| win95-launch | Offline Win95 preparation and journaled restoration. |
| win95-path | Offline Win95 path migration and restoration. |
| win101-setup | Win1.01 media-copy preparation and launch profiles. |

The project-owned native/driver sources are indexed in
[src/addon](../src/addon/README.md). Scripts consume released components;
PIF/HASH documentation belongs with those source components. The original
SETVER.EXE supplied to win101-setup remains a separately retained input.
These operator workflows do not authorize changes to original supplied media
or recompilation of original Microsoft guests; apply the
[source policy](../docs/etc/operations/policy/source-policy.md).

## Engineering tools

| Directory | Responsibility |
| --- | --- |
| build | Actual configure/build/package/publication owners. |
| audit | Source/composition observers and current product verification. |
| governance | Current documentation, manifest and execution checks. |
| probe | Reusable diagnostic or read-only inspection entrypoints. |

Do not classify a script as obsolete merely from a T-number in its name:
CMake still invokes New-T310OriginalSoftpcNinja.ps1. Current call chains,
declared manual entrypoints and evidence dependencies determine retention.

The retained historical/, import/, observation/ and research/ directories,
and dated one-off probe scripts, are assessed in the
[directory review](../artifacts/mvdm-audit/tool-directory-review.md).
Archive obsolete tool closures with their old consumers, manifests and
provenance; never move a producer alone and leave live callers behind.
They do not grant an exception to the current guest-immutability policy.
