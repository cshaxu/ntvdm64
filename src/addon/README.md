# Independently owned add-ons

These components are outside the OpenNT mirrors. Source and licenses live
here; installation and recovery scripts live under tools. Build outputs,
objects, listings and probe results belong under repository build/. Original
Microsoft guest sources, installed systems and media are not build targets.

| Component | Sources and output | Build entry |
| --- | --- | --- |
| compat95 | compat95.asm plus bootlogo.inc/root.inc → compat95.sys | tools/build-addon.cmd compat95 |
| insignia-386 | insignia.asm → insignia.386 | tools/build-addon.cmd insignia |
| ntvdm64-386 | ntvdm64.asm → ntvdm64.386 | tools/build-addon.cmd ntvdm64 |
| grp | grp.c → GRP.EXE | grp/BUILD.CMD |
| hash | hash.c → HASH.EXE | hash/BUILD.CMD |
| lnk | lnk.c → LNK.EXE | lnk/BUILD.CMD |
| mouse101 | mouse101.asm → MOUSE101.DRV | tools/build-addon.cmd mouse101 |
| mouse31 | mouse31.asm → MOUSE31.DRV | tools/build-addon.cmd mouse31 |
| patch31 | patch31.c → patch31.exe | patch31/BUILD.CMD |
| patch95 | patch95.c → patch95.exe | patch95/BUILD.CMD |
| path95 | path95.c → path95.exe | path95/BUILD.CMD |
| path31 | path31.c → path31.exe | path31/BUILD.CMD |
| pif | pif.c → PIF.EXE | pif/BUILD.CMD |
| szdd | szdd.c → SZDD.EXE | szdd/BUILD.CMD |

The nine native tools are AMD64 standalone programs. The two mouse drivers
are independent Windows NE add-ons; compat95 is an independent real-mode DOS
driver. They are not linked into the eleven host components. Driver source and
output identity alone does not establish guest runtime acceptance.

common/file-helpers.h is a shared source header for patch95 and path95, not
another executable or runtime library. Neither component depends on the
other's private directory or binary.

The canonical producer is `tools/build-addon.cmd [component]`; omit the
component to build all14 outputs. It uses MSVC AMD64, NASM and Open Watcom
WLINK through native CMake/Ninja targets in `build/.ninja/addon`. WLINK is
resolved from PATH or `build/.tools/openwatcom/binnt64`; the linker DLL and
configuration beside it are tracked inputs. No tools are downloaded.

The nine native `BUILD.CMD output-exe` entries delegate to this producer and
copy the selected result; objects stay in the stable cache. Driver outputs
carry `build.sha256` and `build.inputs.json`, binding output identity to source
and tool hashes. The configure-time `source-inputs.json` is not a build receipt.
The `compat95-control` target builds the no-logo control separately.

`mouse101-tests` and `mouse31-tests` build16 independently authored probe
programs, separately from release targets. Existing observation runners invoke
these targets and check NE structure; their assertion languages are not build
dependencies. No original Microsoft guest is rebuilt.

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
