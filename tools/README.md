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

`build.cmd` builds the11 host images. `build-addon.cmd [component]` builds
all14 authored addons or one selected target; see the addon source index for
the separate test targets. Both use stable native CMake/Ninja caches.
`build-fixtures.cmd x86|x64 <group>` builds the migrated standalone groups:
guest-memory, redirector-async, guest-location, wow-task-frame,
wow-callback-frame, redirector-contract, dem-file-lifecycle,
redirector-guest-copy, and xms-mapped/xms-startup (x86 only).
The redirector-contract x86 target also compiles seven original integration
units. T280 and T282 shared one fixture and now share guest-location.
This does not imply that every test producer has already migrated.
Existing test runners execute and validate those binaries.

`build-fixtures.cmd x86 guest-probes` builds44 independently authored DOS COM
programs and2 authored SYS probes under
`build/.ninja/fixtures-x86/guest-probes/<group>`.
Individual groups are guest-redir, guest-video-int10, guest-video-direct,
guest-video-graphics, guest-video-handshake, guest-system, guest-bios,
guest-support, guest-disks, guest-comms, guest-comms-medium, guest-dem,
guest-command and guest-vdd. The variant directories prevent one profile
from overwriting another. These opt-in probes are not release inputs.
`guest-keymouse` builds KMTST.COM and its x86 console observer;
`command-tools` builds S34N.exe, S34E.exe and command-stream-observer.exe.
`system-fixtures` builds NT I/O facade and VDM control tests on x86/x64,
plus the original x86-only DOS find-layout fixture.
`observation-tools` builds five observers on x86/x64 and the x86 clean-console
integration helper. Select the architecture matching the frontend being observed.
`suballoc-lifecycle` builds the original x86 allocator lifecycle runtime test.
`oem-tests` builds17 x86 OEM/COMMAND fixtures in the existing worker graph.
Run the PIF caller in a short working directory; its separate
`--expect-long-startdir` case verifies the original64-byte rejection boundary.

`build-memory-probes.cmd ems <new-build-output> <runtime-test-root>` builds
the three EMS/DOS probes and writes their PIF/configuration files using the
existing native generators. The runtime test directory must already exist.
`build-memory-probes.cmd xms <new-build-output> <runtime-test-root>
[default|default-low|reserved] [normal|fault]` selects the XMS/INT15 and
startup-fault variants. Reserved INT15 keeps its original low-DOS behavior.
The output directory must be new and below this repository's build directory.
Runtime paths keep the caller's original spelling and must fit the original
PIF64-byte path fields. These commands do not launch the tests or edit guest
installation files. Native generator tools remain available through the
`memory-profiles` fixture group for their additional existing test options.

| Directory | Responsibility |
| --- | --- |
| build | Actual configure/build/package/publication owners. |
| audit | Source/composition observers and current product verification. |
| governance | Current documentation, manifest and execution checks. |
| probe | Reusable diagnostic or read-only inspection entrypoints. |

Do not classify a script as obsolete merely from a T-number in its name:
The former New-T310OriginalSoftpcNinja.ps1 was retired after its five remaining
test consumers were migrated or removed as obsolete. Current call chains,
declared manual entrypoints and evidence dependencies determine retention.

The retained historical/, import/, observation/ and research/ directories,
and dated one-off probe scripts, are assessed in the
[directory review](../artifacts/mvdm-audit/tool-directory-review.md).
Archive obsolete tool closures with their old consumers, manifests and
provenance; never move a producer alone and leave live callers behind.
They do not grant an exception to the current guest-immutability policy.
