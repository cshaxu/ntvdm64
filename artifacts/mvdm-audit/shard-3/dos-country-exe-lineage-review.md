# S10 shard 3: COUNTRY.EXE build intermediate and deployed data identity

Read-only follow-up of pending `dos/v86/dev/country/country.exe` (OpenNT-4.5-only inventory). `country-exe-lineage-observer.py` and `country-exe-lineage-observations.json` record byte-level identity; no guest build, execution or modification.

The historical `dos/v86/dev/country/makefile` builds `country.obj` from `country.asm` and `mkcntry.inc`, links it with `link16` to `country.exe`, then invokes `reloc country.exe country.sys` and binplaces the SYS. NLSFUNC's original `nlsfunc.asm:10-15` describes reading COUNTRY.SYS to serve extended country and code-page information; the CONFIG path also references it. Thus COUNTRY.EXE is a historical **guest data-build intermediate**, not a program required for ordinary runtime input.

The retained executable is 27,609 bytes, SHA-256 `1B5B114FC5442A596D654EE2E463CF9B250561AB9DE49C7BF15F79856FB96442`; its MZ header length is 512 bytes. Its remaining 27,097 bytes match, byte for byte, both retained `src/mvdm/dos/v86/dev/country/COUNTRY.SYS` and the currently installed `O:/winnt/system32/COUNTRY.SYS`. All payload hashes are `9DDF2B738307B5950699513E6B8221E2044BB686487A8C216AF7C6FD3F853B54`, also the approved guest manifest's COUNTRY.SYS hash. This is stronger than filename matching: the executable's retained payload is identical to that installed data file. It does **not** prove an execution of the original linker/reloc recipe, original source-to-EXE reproducibility, use in every configuration, nor per-codepage/country correctness.

Suggested disposition: classify `country.exe` as retained original **intermediate build artifact** (file-role exclusion from runtime), with the actual COUNTRY.SYS guest data and NLSFUNC behavior tracked independently. The earlier `dos-device-locale-role-review.md` noted no identical EXE archive image; this review establishes the separate embedded payload-to-deployed-SYS relation, not identical EXE-to-SYS full files.

Replay:

```powershell
$env:PYTHONDONTWRITEBYTECODE='1'
python artifacts/mvdm-audit/shard-3/country-exe-lineage-observer.py
Get-Content src/mvdm/dos/v86/dev/country/makefile
rg -n 'COUNTRY\.SYS|country\.sys' src/mvdm/dos/v86/cmd/nlsfunc/nlsfunc.asm src/mvdm/dos/v86/doskrnl/bios/sysinit2.asm assets/winnt-guest-manifest.json
```
