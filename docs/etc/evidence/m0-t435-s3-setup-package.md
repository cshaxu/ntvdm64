# T435 S3 — Owner-run Windows 1.01 Setup package

Owner requests original Setup installation at O:/win101, then takes over the
interactive installation. Later requests create O:/win101-setup with complete
media/new mouse/scripts; generate PIF/config/autoexec/win.cmd/start.cmd and
resolve run16 through PATH. Automatic installation remains stopped.

Published S2 APP435/RPC45/I/O25 is unchanged. The private Setup reaches real
mouse/graphics pages and exits on Q with1. O:/win101 is created but empty,
not a completed installation. Its initial detached-desktop controller fails
before observable Setup; retaining the desktop until process exit fixes that
controller lifetime. The manual wrapper's trailing root separator makes the
guard reject legitimate TMP writes; normalization fixes that boundary. These
are installer/test-tool failures, not an ACL or product fix claim.

The portable directory contains100 original merged loose files from O:/Windows,
including five DISK markers, SETUP/BUILD/UTILITY labels, KERNEL/USER/GDI,
drivers/fonts and applications. It is not presented as original floppy images.
Only MEDIA/MOUSE.DRV is replaced by the authored r008 driver; DRIVER contains
its unchanged binary/source. Original media hashes and all delivered file
hashes are in build/M0-T435/S3/r007-package/package-manifest.json. Original
configured SETVER is included under SUPPORT. No diagnostic runtime is shipped.

package-win101-setup.ps1 verifies required original inputs, checks original
hashes and refuses to overwrite an existing package unless every prior manifest
file remains unchanged. It does not delete additional user files. Build stages
retain recoverable prior copies. r007 publication matches all recorded hashes.

SETUP.CMD generates its PIF/profile, temporarily maps only Z:, invokes PATH
run16 and preserves original Setup's actual exit code. After success and
WIN.COM/WIN100.BIN/WIN100.OVL existence, configure-launch.ps1 generates installed
WIN101.PIF, PROFILE/CONFIG.NT/AUTOEXEC.NT/SETVER.EXE and win.cmd/start.cmd.
win.cmd sets CWD/TEMP, calls PATH run16 on the PIF and preserves its exit code;
start.cmd is an alias. SETVER is local, not dependent on a fixed run16 location.

PIF generation copies the accepted Win31 template and uses the original pif.h
packed STDPIF/PIFEXTHDR/W386PIF30/PROPNT31 fields. It validates spans/cycles,
preserves unrelated flags/extension links, clears both /S argument fields and
sets its own executable/CWD/profile paths. No NT/WOW DOSX or global config edit.

Actual packaging checks verify target/config/autoexec strings, absence of /S
and Win31 paths, driver binary identity and PATH-only invocation. Z: is unmapped.
This proves package preparation, not successful Setup or installed launch;
owner-run installation and actual O:/win101 mouse/exit checks remain open.

## Superseding H: flat-media rebuild

Owner supplies H:/ as the original blueprint and explicitly requests rebuilding
O:/win101-setup from it. r008-h-package supersedes r007: all96 read-only H: files
are copied into the package root. Only MOUSE.DRV differs, matching the verified
independent r008 driver. Unlike the earlier mixed installation input, no old
WIN.COM/WIN100.BIN/WIN100.OVL is introduced; original Setup must generate them.
No MEDIA/DRIVER/SUPPORT/PROFILE directories are created. Additional scripts,
PIF/template, config/autoexec, source and SETVER live at the root; TEMP is made
only when the owner runs Setup, not part of original media layout.

Existing installer is identity-checked, its exact non-reparse absolute path is
verified, then the whole directory is moved to r008-h-package/previous-package.
It is recoverable, not deleted. Publication failure restores that directory.
Final comparison verifies all96 names and hashes against H:, except MOUSE.DRV;
flat SETUP.PIF points at Z:/SETUP.EXE, Z:/CONFIG.NT and Z:/AUTOEXEC.NT without /S.
The installed generator likewise emits root-level config/PIF/SETVER/win/start.
No installation runs, no H: file changes and no Z: mapping remains.

## Final PATCH-only placement and source rename

Owner requires every non-original file under PATCH, and renames driver source
to src/addon/win101-mouse-drv while assigning installer implementation to
src/addon/win101-setup. Installer/templates and binary-copy installer move there;
test build/caller references are updated. Unrelated assets/release changes are
preserved and not included in this work.

H: is unmounted. r010-frozen-h/source recovers all96 original bytes from the
retained package plus backup MOUSE.DRV matching the original H: SHA
E8C0A3D6B9860A7AA87D41C829DE1C18DC306F76F2AEB04CFA6CAFF3EB8AE107.
r011 fails on the concurrently renamed source before publication; it is not a
pass. r012-patch-package uses new source owners and verifies every original
root hash, including original MOUSE.DRV. Only PATCH is an added root directory.
PATCH/MOUSE.DRV and PATCH/WORK/MOUSE.DRV match the verified new driver. Original
Setup/media are copied into WORK; its generated PIF/config use Z: mapped to
that work directory. Installed additions likewise use O:/win101/PATCH. No
actual Setup is run by this repackaging, and no Z: mapping remains.

## Superseding physical-path and parameter-only installer

Owner prohibits drive mappings, fixed drive letters and hardcoded machine paths.
Earlier mapped-drive procedures above are historical evidence, not current
instructions. All scripts and both readmes in src/addon/win101-setup now have
no fixed drive paths or mapping command. Package location derives from the
script; SETUP.CMD requires an installation-directory argument, also selected
in original Setup. Packaging requires explicit original media, driver build,
destination, SETVER and reference PIF inputs. Relative PATCH/WORK layout and
standard SystemRoot-derived HIMEM remain, not machine-location constants.
PIF/config contain the actual supplied paths: this is necessary generated data,
not a hardcoded default; relocation requires regenerating the profiles.
ASCII/path-field validation rejects unsuitable physical paths before writes.

The owner's actual package is found at O:/w1setup, after their path relocation.
Read-only inspection finds WIN.COM/BIN/OVL at O:/win101 but no installed launch
profiles. Actual Setup exit status is unknown. A nonzero launch return or CMD
wrapper invoked without CALL could explain skipped postconfiguration; neither
is established as the cause of this particular owner run. SETUP.CMD now uses
CALL and writes setup-result.txt, preserving real failure and showing recovery.
r013-postinstall creates only installed PATCH profiles/SETVER/launchers;
all prior root-file hashes remain unchanged. No original Setup is rerun.
r014-no-subst backs up and updates owner package scripts to physical paths.

Reproducible fixture:
`tests/component-integration/win101_launch_profile_test.ps1 -BuildRoot <fresh-short-build-root> -PifTemplate <reference-PIF> -SetverPath <SETVER>`.
r015 exposes a fixture array-expression precedence error, not a product pass;
parenthesizing the composed invalid path fixes the harness. r016 verifies
supplied destination PIF/profile strings, PATH-relative launcher, three
invalid-path refusals without changing the valid PIF, and the entire
component's fixed-drive/mapping audit. Inert file-existence fixtures are never
executed. This does not prove installed Windows startup or mouse acceptance.
Host package APP435/RPC45/I/O25 remains unchanged.

## No-argument interactive Setup and temporary work cleanup

Owner corrects the unnecessary destination-before-Setup restriction.
PATCH/SETUP.CMD starts original Setup without arguments; after success,
run-setup.ps1 asks for the actual installation directory and generates profiles.
It owns fresh temporary WORK, restores CWD/TEMP/TMP and removes WORK in finally
on success/failure. Preexisting WORK is preserved, not silently overwritten;
unsafe reparse cleanup is refused. Forced termination can bypass cleanup and
is documented. Installed files/original media are never cleanup targets.
r018 inert PATH-launcher fixtures verify success cleanup, exit37 result/log/
cleanup and unchanged installed PIF on failure, plus path/PIF refusal checks.
No original Setup is run. r019 backs up and synchronizes the four changed
package files at O:/w1setup/PATCH; WORK is already absent, so none is deleted.
Actual interactive Setup/Windows startup remains owner verification.

## Copied temporary profile repair

Owner's screenshot is run16 setup, failing on installed SETUP.PIF's reference
to the removed package WORK/CONFIG.NT. Read-only inspection confirms installed
root SETUP.PIF and CONFIG.NT/AUTOEXEC.NT carry temporary working-directory paths.
Original Setup copied these loose files; only generating PATCH/WIN101.PIF did
not repair them. Installed-profile generation now also emits root WIN.PIF and
SETUP.PIF targeting installed executables and PATCH profiles, and rewrites root
CONFIG.NT/AUTOEXEC.NT using installed paths. Guest executable bytes are untouched.
r022 fixture simulates original Setup copying those three temporary files;
postconfiguration repairs every tested PIF/config reference before WORK cleanup.
Success/failure cleanup and failure-result preservation remain passing.
r023 backs up and updates the actual package generator/readme. Existing owner
installation is not repaired or rerun here; the owner requests a fresh install.
No dependency on fixed locations, drive mapping or extra launcher is introduced.

## Final owner acceptance, diagnosis and asset delivery

Owner confirms final installation/profile workflow works and Notepad runs inside
Windows 1.01. setup-result.txt showed run16_exit1 on the earlier failed
postconfiguration. The final script prompts after any returned exit code:
confirmed installed directory permits profile generation, while actual nonzero
status remains nonzero. Blank input skips configuration. r024 and final r034
exercise explicit confirmed-failure recovery, ordinary success/failure, copied
temporary-profile repair and cleanup. Final SETUP.CMD displays its saved status
and pauses without changing that status. Only win.cmd remains; its alias and
template are retired. No runtime host source changed in S3.

Direct run16 Notepad diagnosis returns50. Read-only NE header has target OS0 /
expected version0. Corrected r030/r035 SEC_IMAGE queries return C000011B;
original vdm.c's selected x86 classification branch maps this status to OS/2,
which run16 rejects before broker submission. Public GetBinaryType returns
DOS/type1. r028 lacked execute file rights; r029 had a probe uint literal error;
neither is a pass. r027 process inventory was denied and is not lifecycle proof.
The reproducible read-only probe is tests/observation/inspect-ne-classification.ps1
with explicit Image/BuildRoot. Classification is not changed; direct WOW
execution of this application remains unverified, not an installer/PIF failure.

Owner explicitly requests all supplied assets in this delivery. win101-setup.zip
has108 entries, no WORK or redundant alias; its five shipped script/readme
payload hashes match current src/addon/win101-setup exactly. Its setup-result.txt
is a retained owner-run record, not installation-success proof. ZIP SHA256:
669B33F04146E9E89591BDDBF02B297373A798777EC6C390EB13232DD3AABB73.
Owner winnt.zip SHA256:
51E1AC1F078DFDC9C535A6FEDAE0394FBC325A2A1EA5D970A9B0A6D6DCAE2BB7.
assets/release contains ten owner-supplied executable/DLL snapshots. They are
not all identical to published S2, are preserved as supplied, not redeployed or
claimed as a newly verified runtime package. S2 runtime evidence still belongs
to its exact published ten-image manifest.

The owner has removed the external package before final checks; r031 therefore
fails its missing PIF prerequisite, not product functionality. r034 uses retained
reference PIF/SETVER bytes and passes. Two unused new mapped-drive launch scripts
are retired to r033 build evidence rather than added to main; preparation now
requires explicit physical input/destination paths. No subst is executed.
Final delivery runs documentation governance, relative links and diff checks;
full host rebuild/product matrix is not repeated because this S changes only
add-on source ownership, installer scripts/docs/test-only diagnostics and assets.
No mirror/host production algorithm, wire or deployed runtime input changes.
Actual installed physical/RDP mouse and general application acceptance remain
outside these focused fixtures. T435 is not automatically closed by this P.
