# T484 S3 P2: retire the old worker generator

The five executable consumers of New-T310OriginalSoftpcNinja.ps1 were checked
in DIR order and handled as follows. Historical evidence mentioning that
filename remains historical; it is not a current build input.

| Consumer | Disposition and evidence |
| --- | --- |
| test/app/Verify-T410NativeConsoleRoute.ps1 | Removed obsolete source-shape check. It reads nonexistent src/app/entry.c and asserts retired Console implementation markers; it has no executable caller. Current Console tests are retained. This is not claimed as a fresh Console runtime pass. |
| test/mvdm-host/dos/Verify-T409FastIoFallback.ps1 | Reads the configured x86 source manifest, requires demdisp.obj, retains original dispatch and guest fallback assertions. Pass. |
| test/mvdm-host/dos/Verify-T409OriginalVddLifecycle.ps1 | Reads the configured x86 source manifest, requires demfile.obj, retains original SFT initialization/allocation/retrieval/release assertions. Pass. |
| test/observation/verify-dem-directory-reset.ps1 | Compilation/linking moved into the native worker graph using the real DEM profile and existing CCPU fixture providers. Runner retains original symbol-map ownership, timeout, filesystem checks and115 assertions. Pass. |
| test/observation/verify-redirector-oem-capacity.ps1 | Both variants use native targets and the current production redirector profile/archive. Reuses the native bounded source projection instead of invoking Python extraction. Keeps original provider symbol ownership, allocation/free/register/copy ordering and14/18 assertions. Both pass. |

The old worker generator is deleted after these consumers are resolved.
Current documentation links now point to native build owners. Worker link
targets expose their declared inputs for fixture reuse; no second copy of the
large provider list is introduced. Product algorithms, ABI, mirror and guest
bytes are unchanged. The source-only tests still distinguish their evidence
from runtime integration evidence.

Actual runs: static checks plus DEM PID372, redirector variants PID33208.
Final cached full build PID26632 passed after deletion of the old generator.
All11 raw host inputs and22 published host hashes remain the same as the
existing verified package. No changed host image requires republication.
Addon publication remains S3 P1; this batch changes no addon producer.

Evidence is in the [observation bundle](../../../artifacts/build-migration/t484/s3-observation-cmake/result.json).
The containing commit delivers this batch. This closes all five consumers of
that generator, not all S3 candidates. Other standalone compilation scripts,
including guest-memory extraction consumers and historical/manual generators,
still need individual disposition. S3, S4, S5 and T484 remain open.
