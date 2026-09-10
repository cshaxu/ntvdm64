# Product UX

The current product is the x86 `ntvdm32.exe` command-line runtime. It directly
launches admitted DOS `.com`, `.exe`, `.bat`, and `.pif` targets through the
selected original CCPU40/MVDM path; bounded WOW16 workloads are a queued
recovery boundary, not a current compatibility claim. Unsupported inputs or
unavailable recovered services must fail explicitly at their original-owner
boundary rather than falling back to a substitute engine or private host shell.

The product is non-invasive: it does not install software, replace Windows
files, rebuild private subsystems, inject processes, or mutate host
configuration. Its sole deployed executable may be copied to
`O:\ntvdm64\ntvdm32.exe` after formal x86 staging; diagnostics and observation
logs belong only below `O:\ntvdm64\logs\`.
