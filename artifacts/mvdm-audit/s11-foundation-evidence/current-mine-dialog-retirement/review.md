# Current shared-worker Winmine and dialog retirement

The published ten-component candidate ran Winmine and the unchanged prebuilt
`CDLPROBE.EXE` in the same real WOW worker. The probe created 17 common
dialogs, explicitly destroyed 16 and left one for task retirement. The
guest reported all expected hooks, no errors and a live final dialog; the
worker trace recorded exactly one successful retirement for its task.
Winmine remained usable afterwards: flag, counter, reset, open and close
checks passed, and the launcher returned zero. The input hashes and reports
are retained here. WOW32 SHA-256 was
`6EF2791521DB2B962BFB38E800143C840CBCC3AF70417FDF7BA4E83605C34741`;
the prebuilt guest was `406D84F9E4EA04CD269045B354E95C47A342C986E0400A2C4F6A58DC818D3A02`.

The observation runner now accepts the existing hash-verified prebuilt guest
for its retiring-sibling case, without assembling guest code. An all-zero
expected hash was rejected before launch. This passing run exercises one
real overlapping-task lifecycle; it does not force the historic concurrency
schedule or identify the unrelated earlier SOL 1400 failure. No product or
guest binary was changed.
