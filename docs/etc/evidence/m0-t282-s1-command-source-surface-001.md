# M0 T282 S1 — original COMMAND source-surface evidence

Both MSVC x64 and x86 static-only Ninja probes reached the same first
declaration boundary: original `cmd.h` includes `vdmapi.h`, but the current
adapter-win32 carrier is an empty prior placeholder. The exact original
definition is `public/internal/base/inc/vdmapi.h`; it supplies `VDMINFO`,
`MAXIMUM_VDM_CURRENT_DIR` and `GetNextVDMCommand`. `cmdpif.c` additionally
selects the exact `public/internal/windows/inc/pif.h`, not the unrelated
standalone forcedos tool header.

No COMMAND source was edited, no provider or selector was enabled, and no
Bochs input entered either graph. The two S1 ledgers cover all eleven original
translation units and all seventeen `54:00..54:10` services. Their first
recovery group is the source-shaped vdmapi/PIF declaration and session-monitor
binding boundary; subsequent child, stream, environment and Redirector work
remains explicitly owned as recorded per row.
