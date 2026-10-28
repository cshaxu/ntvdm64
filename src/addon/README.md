# Independently owned add-ons

These components are outside the OpenNT mirrors. Source and licenses live
here; installation and recovery scripts live under tools. Build outputs,
objects, listings and probe results belong under repository build/. Original
Microsoft guest sources, installed systems and media are not build targets.

| Component | Sources and output | Build entry |
| --- | --- | --- |
| compat95 | compat95.asm plus bootlogo.inc/root.inc → compat95.sys | compat95/build.ps1 |
| grp | grp.c → GRP.EXE | grp/BUILD.CMD |
| hash | hash.c → HASH.EXE | hash/BUILD.CMD |
| lnk | lnk.c → LNK.EXE | lnk/BUILD.CMD |
| mouse101 | mouse101.asm → MOUSE101.DRV | tests/observation/build-win101-mouse-driver.ps1 |
| mouse31 | mouse31.asm → MOUSE31.DRV | tests/observation/build-win31-mouse-driver.ps1 |
| patch31 | patch31.c → patch31.exe | patch31/BUILD.CMD |
| patch95 | patch95.c → patch95.exe | patch95/BUILD.CMD |
| path95 | path95.c → path95.exe | path95/BUILD.CMD |
| path31 | path31.c → path31.exe | path31/BUILD.CMD |
| pif | pif.c → PIF.EXE | pif/BUILD.CMD |
| szdd | szdd.c → SZDD.EXE | szdd/BUILD.CMD |

The nine native tools are AMD64 standalone programs. The two mouse drivers
are independent Windows NE add-ons; compat95 is an independent real-mode DOS
driver. They are not linked into the ten host components. Driver source and
output identity alone does not establish guest runtime acceptance.

common/file-helpers.h is a shared source header for patch95 and path95, not
another executable or runtime library. Neither component depends on the
other's private directory or binary.

Each native BUILD.CMD entry accepts one output path.
Create their output directories under build/ first. They select the existing
MSVC Build Tools and keep intermediate objects in an output directory.
Mouse-driver producers additionally assemble their independently authored
test harnesses and validate NE structure. This does not rebuild a Microsoft
guest. compat95/build.ps1 emits a separate source/output manifest.

Publish through tools/build/Stage-AddonUtilityRelease.ps1,
Stage-MouseDriverRelease.ps1 and Stage-Compat95Release.ps1. The canonical
assets/release/addon-manifest.json owns source paths, output hashes and driver
acceptance. User tools consume these release files; they do not carry a
second project-produced binary copy. Offline patchers act only on explicitly
selected recoverable copies. Their presence is not a general Win95 support
claim or a default runtime dependency.

The former win101-setup script directory is consolidated into
tools/win101-setup. PIF and HASH usage documentation lives with each component.
The patch95 and compat95 third-party notices remain with their source.
Apply the [source policy](../../docs/etc/operations/policy/source-policy.md)
before importing or deriving any additional source or guest material.
