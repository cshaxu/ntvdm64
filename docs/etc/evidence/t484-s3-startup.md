# T484 S3 P77 startup diagnostic producers

B0103 has two diagnostic modes: failure of the second allocation in original
xxxInitTask, and an exception after the original Shell callback returns. Both
now use native one-object WOW diagnostic targets and preserve their original
wrapper source, fault identity, normal DLL hash and nonpublishable manifest.
The runner no longer parses Ninja command text or emits CL/link commands.

Startup uses the real queue owner profile. Shell replaces only the leaf
wow_shell_callback object, but its diagnostic observes original TD/VDM stack
state through precomp.h. The leaf's modern-only flags do not provide these
headers. The old script selected the registration bridge compile profile;
that current bridge is also modern-only after earlier layering work. Two
failed trials (PID18300 and18796) demonstrate missing precomp.h. The final
explicit diagnostic profile is original_wshell, the actual original Shell
caller with these internal headers. It does not broaden the product leaf's
profile, copy algorithms or alter its ABI. Both modes compile/link at PID30780.

The obsolete optional Ninja/VsDevCmd overrides are removed from this test
builder; no checked-in caller supplies them. Tool selection now follows the
shared CMD build entry/environment, as other native diagnostics do. Existing
ProviderBuildRoot, BuildRoot and ShellException selections remain.

[Build receipt](../../../artifacts/build-migration/t484/s3-startup/diagnostic-build-result.json)
records the positive native build; raw attempts are in build/t484-s3-startup.
No original guest or product implementation changed. Fresh guest startup/Shell
fault execution is not claimed by these producer tests. Final cache, object
order, architecture rejection, callback-frame and publication checks are
recorded with delivery, separately from the historical guest behavior.

Cached full build PID32856 and final verification PID25568 exit0. Each mode
keeps229 linked objects with exactly one expected replacement; both cached
DLL hashes/mtimes remain stable, x64 is rejected, and the frame fixture passes.
All11 raw host inputs and22 published hashes remain unchanged; dual-location
publication is verified, no diagnostic image is published. B0103 is complete.
Fixed355 inventory:152 complete,203 remaining (51 pending producer items,
152 awaiting reconciliation). B0101 historical positive verification remains open.
