# M0 T396 S22 — WOW32 import-name loader audit 001

## Question and inputs

S21 proved that the ordinary fresh product reaches original
`SafeLoadLibrary("WOW32") == NULL`. S22 inspected the staged x86 provider
(`WOW32.DLL`, SHA-256
`4333e8d705b017af784c3855b1dbda87e17c6eb2ebfb11d82bd8d8b5ca58f9a6`),
the fresh parent product, and the current SysWOW64 export tables.

## Procedure and observations

A disposable x86 loader surrogate mapped the selected parent image with
`DONT_RESOLVE_DLL_REFERENCES`, then issued an ordinary
`LoadLibraryA("O:\\t396\\WOW32.DLL")`. It mapped the parent successfully and
returned `ERROR_PROC_NOT_FOUND (127)` for WOW32. This is not product runtime
evidence, but it isolates the Windows loader's dependency-resolution stage.

PE import/export comparison found:

| Dependency | Imports | Missing exact export spellings |
| --- | ---: | ---: |
| `ntvdm32.exe` | 81 | 0 |
| `ADVAPI32`, `GDI32`, `SHELL32`, `COMDLG32`, `VERSION` | 214 | 0 |
| `USER32` | 272 | 1: `SetCursorContents@8` |
| `KERNEL32` | 163 | 2: `RegisterWowExec@4`, `RegisterWowBaseHandlers@4` |
| `NTDLL` | 21 | 21 decorated `@N` spellings |

The affected SysWOW64 DLLs export the same historical APIs under undecorated
names (for example `SetCursorContents`, `RegisterWowExec`, and
`RegisterWowBaseHandlers`). The provider's import table instead asks the
loader for decorated x86 names. This proves the generated import libraries
preserve a source-compiler symbol form where the Windows loader requires the
DLL export spelling. It explains both S21's `loaded-wow32=no` result and
`SafeLoadLibrary`'s `NULL` result.

## Disposition

S22 closes with an exact build-binding owner: the S2 task-local x86 system
import-library generation for USER32, KERNEL32, and NTDLL. No original WOW32
source or loader behavior is wrong or changed. S23 must regenerate those
import libraries with loader-visible undecorated export names while retaining
each original source call's x86 ABI/calling convention, relink the complete
77-body provider, and prove ordinary product load before claiming `W32Init`
or `W32Dispatch`.
