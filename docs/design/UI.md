# Product UX

The current product is the x86 `ntvdm32.exe` command-line runtime. It directly
launches admitted DOS `.com`, `.exe`, `.bat`, and `.pif` targets through the
selected original CCPU40/MVDM path; bounded WOW16 workloads are a queued
recovery boundary, not a current compatibility claim. Unsupported inputs or
unavailable recovered services must fail explicitly at their original-owner
boundary rather than falling back to a substitute engine or private host shell.

The product is non-invasive: it does not install software, replace Windows
files, rebuild private subsystems, inject processes, or mutate host
configuration. Its selected deployed runtime set may be copied to the
`O:\winnt` package root after formal x86 staging; test harnesses and guest
probes belong only below `O:\winnt\tests\`, while diagnostics and observation
logs belong only below `O:\winnt\logs\`. `O:\winnt\builds\` is reserved for
deliberately retained historical runtime-package versions.
