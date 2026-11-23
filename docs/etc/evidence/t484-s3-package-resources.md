# T484 S3 P17: package and resource producers

The firmware-resource and package-layout PowerShell generators are replaced
by `tools/build-fixtures.cmd x86|x64 package-resources`. Four native targets
retain the current source bodies and their input closures. Runtime media
staging is explicit test setup rather than a configure-time side effect.
No production, mirror or original guest sources change.

`test/observation/verify-package-resources.ps1` passes ten runtime cases:
package root derivation and rejection, unknown embedded-ROM rejection, WOW
initial environment, media lookup/shadow reuse and cleanup, and flat-media
rejection, each on x86/x64. Unknown-ROM rejection does not prove successful
embedded-ROM loading. The media input comes read-only from O:/winnt and its
hashes are recorded; no original guest is rebuilt.

The first flat-media negative setup incorrectly shared an executable package
with a valid system32/NTIO.SYS, so the product correctly found that file. The
final negative case places its executable in a separate package containing
only the flat NTIO.SYS. It passes without a product change. The final
[runtime result](../../../artifacts/build-migration/t484/s3-package-resources/result.json)
records all ten cases and input identities.

Cached full host build and raw/published identity checks are recorded beside
the runtime result. The publication package remains
d8983d2b-3fe1-4d18-8ea9-64613c84346c:11 raw host images and22 published hashes
are unchanged, so no changed product image requires republication.
This targeted delivery is not a Full runtime rerun or S3 closure. The containing
commit is P17; remaining build producers and S4/S5 acceptance remain active work.
