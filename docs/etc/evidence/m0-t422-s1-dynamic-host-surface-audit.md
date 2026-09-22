# T422 S1 WOW32 dynamic host-surface audit

## Question

Do the selected 32-bit host DLLs provide every dynamically resolved export
needed by the selected original WOW32 sources?

## Inputs and procedure

The audit reads the original dynamic tables in `wsext.c`, `wgprnset.c`,
`wuclip.c`, and all `GET_MULTIMEDIA_API` sites in `wmmedia*.c`.  The new
`verify-wow32-dynamic-host-surface.ps1` compares those source-derived entries
with x86 `dumpbin /exports` output for the SysWOW64 DLLs.  It deliberately
does not try to load an x86 DLL into the 64-bit PowerShell test process.

## Observation

| Original consumer | Host DLL | Form | Required | Missing |
| --- | --- | --- | ---: | ---: |
| `wsext.c` | `WSOCK32.DLL` | ordinal | 49 | 0 |
| printer callers | `WINSPOOL.DRV` | name | 15 | 0 |
| `wmmedia*.c` | `WINMM.DLL` | name | 78 | 0 |
| `wuclip.c` | `OLETHK32.DLL` | name | 1 | 0 |

The source's 49 Winsock entries are intentionally ordinal-valued `(char *)`
records, including legacy values above 100.  The current `WINMM.DLL` also has
both private WOW helpers consumed by the original multimedia layer:
`WOW32ResolveMultiMediaHandle` and `WOW32DriverCallback`.

## Interpretation and disposition

This host passes the complete static dynamic-import surface for these four
families.  It does **not** prove that a Win16 program reaches a thunk, that
marshalling is valid, or that callbacks and cleanup work.  S7 owns those
runtime contracts and must ensure table resolution fails before a null table
entry can be called: the original retail `LoadLibraryAndGetProcAddresses`
asserts a missing `GetProcAddress` only in debug builds but otherwise returns
success.

`VDMREDIR.DLL` is a product DLL, not a host DLL.  Its exact seven-export load
gate remains T420 S20 evidence and is outside this audit.

## Reproduction

```powershell
& .\tests\observation\verify-wow32-dynamic-host-surface.ps1 `
  -RepositoryRoot (Get-Location).Path
```
