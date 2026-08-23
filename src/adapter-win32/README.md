# adapter-win32

Same-shaped recovery of unavailable historical Win32/NTDLL interfaces using
modern public Win32 APIs. A facade retains the original reached name,
parameters, calling convention and observable failure semantics as far as the
current platform permits.

It owns neither BOP/provider policy nor Bochs interaction. Its public headers
are the declared historical API facades consumed by `opennt-host` and
`opennt-bop`; the future static library name is `adapter-win32`.
