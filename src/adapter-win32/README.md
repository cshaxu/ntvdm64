# adapter-win32

Same-shaped recovery of unavailable historical Win32/NTDLL interfaces using
modern public Win32 APIs. A facade retains the original reached name,
parameters, calling convention and observable failure semantics as far as the
current platform permits.

It owns neither BOP/provider policy nor Bochs interaction. Its public headers
are the declared historical API facades consumed by `opennt-host` and
`opennt-bop`; the future static library name is `adapter-win32`.

## T260/S6 component register

`facade/` contains the pre-existing `opennt_rtl_compat` and
`opennt_error_dialog_facade` implementation. Their source spelling and
observable contract remain the historical OpenNT-facing surface; T260/S6 only
relocates the files out of the former generic BOP shim directory.
