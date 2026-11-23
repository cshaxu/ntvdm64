# T484 S3 P15: ABI and retired adapter producers

Seven historical generators have explicit dispositions:

| Generator | Actual disposition |
| --- | --- |
| New-T280S9DemDeclarationNinja.ps1 | Six unchanged layout assertions and the disk-space declaration compile under the native worker's actual x86 DEM profile through dem-declarations. This is compile-only evidence. |
| New-T283PlatformAbiCarrierNinja.ps1 | Its27 DEM/COMMAND objects duplicate the current product graph; no executable consumer or independent assertion. Retired. |
| New-T288S2MonitorDeclarationNinja.ps1 | Both its named declaration fixture and old sim32 header path are absent. No caller. Retired, without claiming that another sim32 header is the same interface. |
| New-T288S4SasNinja.ps1 | Its autonomous mvdm_sas implementation and header are absent. The sole remaining fixture supplies obsolete machine_facade memory functions and has no current provider. Generator and fixture retired together; no SAS runtime pass claimed. |
| New-T289S5DpmiInterruptAuditNinja.ps1 | Seven original DPMI objects are in the product graph; its extra old overlay input is absent. No caller or runtime assertion. Retired; this does not claim a new DPMI behavior test. |
| New-T308MonitorControlNinja.ps1 | Same vdm_control source/test pair already migrated in P10. Native system-fixtures supplies its running coverage; no second graph needed. |
| New-T309OriginalDispatcherNinja.ps1 | Its two dispatcher objects are already in the product graph. No caller or runtime assertion. Retired. |

[Coverage](../../../artifacts/build-migration/t484/s3-abi-cleanup/coverage.json)
records34 distinct current DEM/COMMAND/DPMI source and object identities.
The absent-input record distinguishes genuinely missing old inputs from moved
or currently used files. Historical standalone x64 syntax islands are not
claimed as preserved product configurations. No original source is deleted,
modified or rebuilt as guest.

Native dem-declarations compiles the unchanged six assertions, and x64
selection is rejected explicitly. Native vdm_control runs successfully on
both x86 and x64. Final cached full host build PID13908 passes. All11 raw host
images and22 dual-publication hashes remain unchanged; package
d8983d2b-3fe1-4d18-8ea9-64613c84346c therefore needs no redundant copy.
[Result](../../../artifacts/build-migration/t484/s3-abi-cleanup/result.json)
and adjacent build receipt record actual evidence. The containing commit is
P15 delivery; this is not a Full runtime rerun or S3 closure.

The remaining literal Ninja-generator inventory has17 entries,3 with missing
literal inputs. This excludes dynamic/inline compilation and is not the full
remaining-work count. S3 continues, with S4 publication and S5 final clean
interpreter-free validation still outstanding.
