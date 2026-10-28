# S10 shard 3: selected printer-setup load failure ownership

Read-only follow-up to `wow-gdi-print-audit.md` for original `wow32/wgprnset.c`, SHA-256 `7E53074328ACE93AE8CE39FB218FA780FF868999B40F7904FDBBFF45FBD7B990`. The cached WOW32 map attributes all three printer setup entry points and `LoadLibraryAndGetProcAddresses` to `wgprnset.obj`; source table dispatch is in `wgtbl2.h`. The prior `wow-generic-thunk-published-binding.md` establishes the cached WOW32 code section is byte-identical to the current release and installed WOW32; this binds the cached map's selected machine-code section to the published code, but not any observed execution of these specific setup entries.

`WG32DeviceMode`, `WG32ExtDeviceMode`, and `WG32DeviceCapabilities` first copy the 16-bit string arguments into host `malloc_w` allocations to survive callbacks that may move guest memory. They release transient mappings and lazily call `LoadLibraryAndGetProcAddresses` for `WINSPOOL.DRV`; actual service belongs to native spooler/driver, and WOW owns its temporary copies and structure conversions. Normal paths free the copies and, where applicable, converted `DEVMODE` memory.

Three specific source-level exceptional contracts deserve follow-up:

1. If `SafeLoadLibrary` fails *after at least one successful non-null string copy*, these three callers return immediately at lines 90-91, 175-176 and 306-309, bypassing their respective `free_w` sites (99-102, 247-253, 513-516). The leak on that stated source path follows directly; no assertion is made that modern WINSPOOL load has failed or that the leak was reproduced in product. Repeated failure has no demonstrated task-level cleanup in this audit.
2. `LoadLibraryAndGetProcAddresses` returns FALSE for module-load failure but after successful loading it loops over exports and returns TRUE even if an individual `GetProcAddress` is NULL; `WOW32ASSERTMSGF` is not a return-value check. An absent requested export followed by an unchecked indirect call is therefore a failure case to test; do not conflate this with the historical DeviceCapabilities observation, whose cause remained unknown.
3. Partial string allocation failures pass NULL to provider in these original paths. The expected return and task/callback cleanup require tests with bounded, controlled allocation failure; allocation exhaustion was not reproduced here.

Both the loader helper and `spoolerapis` have additional consumers in `wspool.c`, `wgdi.c` and `wow32fax.c`; any fix or cleanup contract must cover them as a group, preserve original callback semantics and register original-code diffs. Keep `wgprnset.c` **pending**; existing PDF printing acceptance does not cover the above failures or complete device configuration. No new regression, guest build, or product repair is claimed.

Reproduction (read-only):

```powershell
Get-FileHash src/mvdm/wow32/wgprnset.c -Algorithm SHA256
rg -n 'malloc_w|free_w|LoadLibraryAndGetProcAddresses|return \(0\)' src/mvdm/wow32/wgprnset.c
rg -n 'LoadLibraryAndGetProcAddresses\(' src/mvdm/wow32
rg -n 'wgprnset.obj' build/.ninja/msvc-x86-release/wow32/wow32.dll.map
$env:PYTHONDONTWRITEBYTECODE='1'; python artifacts/mvdm-audit/shard-3/generic-thunk-package-observer.py
```
