# T484 S3 P14: original library build consolidation

Six historical generators were inspected for actual source membership,
consumers and independent test assertions. Their disposition is:

| Generator | Disposition |
| --- | --- |
| New-T281OriginalDemCommonControlNinja.ps1 | Four DEM objects already in the product; retire duplicate archive. |
| New-T281OriginalDemProvidersNinja.ps1 | Twelve further DEM objects already in the product; retire duplicate archive. |
| New-T281OriginalDemPackageNinja.ps1 | Union of those16 objects; retire the third overlapping archive. |
| New-T282OriginalCommandSurfaceNinja.ps1 | Eleven COMMAND objects already in the product; retire duplicate archive. |
| New-T287OriginalXmsStaticNinja.ps1 | Seven XMS objects already in the product; retire duplicate archive. |
| New-T282OriginalCommandInitializationNinja.ps1 | Preserve its real initialization assertions, migrate to a native worker target using actual cmd/cmddata objects. |

The five duplicate producers have no executable consumers or runtime assertions.
Their34 distinct sources, source hashes and existing production object hashes
are recorded in [coverage](../../../artifacts/build-migration/t484/s3-original-libraries/coverage.json).
This retains production compilation, not obsolete standalone x64 syntax islands
or archives with historical header roots. Current source-owner inventories
continue to select the product inputs. No original file or algorithm is removed.

`tools/build-fixtures.cmd x86 command-initialization` builds and runs through
the existing worker graph. The fixture checks initial globals and actual
CMDInit behavior and passes. x64 selection is explicitly rejected with exit2.
No old generator references remain in executable tools/test callers.

Final cached full host build PID20780 passes. All11 raw product images and22
publication hashes are unchanged, so the current dual-published package remains
d8983d2b-3fe1-4d18-8ea9-64613c84346c without redundant copying. Original guest,
product sources and non-error mirror differences are unchanged. The
[result](../../../artifacts/build-migration/t484/s3-original-libraries/result.json)
records the actual test and publication checks; adjacent product-build receipt
records the build. The containing commit is P14's delivery identity.

A refreshed literal inventory contains24 remaining Ninja generators,6 with
missing literal inputs. This is not the total remaining S3 workload: inline
compilation and dynamic consumers still require audit. S3 remains active,
followed by S4 release migration and S5 final clean/interpreter-free acceptance.
