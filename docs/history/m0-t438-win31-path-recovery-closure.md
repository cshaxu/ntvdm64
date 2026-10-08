# M0 T438 — Win31 path recovery closure

T438 made the installed Win3.1 relocation repair explicitly recoverable, then
split its ownership: CMD now controls discovery presentation, file selection,
backup and rollback; `PIF.EXE` and new released `GRP.EXE` own their respective
binary formats; `WIN31PATH.EXE` is discovery-only. No guest media or host
product component changed.

The retained limitation is deliberate: CMD refuses INI constructs it cannot
preserve safely, and PIF fields retain their historical finite capacity. Those
are no-write or recoverable failure paths, not broadened rewrite behavior.
