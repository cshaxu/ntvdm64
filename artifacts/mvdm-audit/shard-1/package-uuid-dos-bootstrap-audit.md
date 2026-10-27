# Package-identity DOS bootstrap: read-only evidence review

This review did not modify or clean `O:/tmp/pqa`, `O:/tmp/pqb`,
`build/package-uuid`, or their running processes. Stage-copy PATH experiments
prepared separately under `build/t473-uuid-observe` did not start: first
preflight saw a project process and the next isolated-scope preflight detected
a live `O:/tmp/pqa/system32/ntsrv.exe` PID 29032 owned by the other session.
Only the reviewer-owned empty stage was cleaned.

Both retained `dos-tmp3` observer command lines are complete: each package's
`run16.exe`, its `system32/COMMAND.COM`, `/c exit`. Both timeout-live files
capture an NTVDM `16 bit MS-DOS Subsystem` modal dialog whose text reads
`config.nt. The system file is not suitable for running MS-DOS and Microsoft
Windows applications.` Thus the timeouts are accompanied by an actual blocked
error dialog; neither image or CPU behavior can be inferred merely from the
observer timeout. Native `cmd.exe /c exit` in the same package has an exited
zero result. The direct DOS dialog alone does not distinguish host raw-config
open failure from guest bootstrap failure, nor does it identify a UUID bug.

The two COMMAND.COM images and S9's package-r10 image share SHA-256
`908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`;
NTIO.SYS and NTDOS.SYS hashes also match between PQA and S9 package-r10.

`dos-tmp3-test.ps1` and `dos-tmp4-test.ps1` retain the same PATH
`C:/Windows/System32;C:/Windows`, same saved environment whitelist and same
program command. Besides output file names, their difference is that TEMP/TMP
changes from `O:/repos.hobby/ntvdm64/build/package-uuid` to `O:/tmp`. Both
tmp3 cases report timeout with that dialog; both tmp4 observer reports say
`result=exited` and `exit=0x00000000`. Tmp4's wrapper reports failure despite
these success records because it did not retain the Process handle before
later inspecting ExitCode; the later checked-in
`tests/broker/verify-package-session-isolation.ps1` explicitly retains it and
its `session-dos-r2` and `session-native-r2` report two concurrent exited-zero
sessions per kind. This evidence does not support the initial hypothesis that
adding package system32 to PATH is necessary; changing TEMP is strongly
associated, not a demonstrated exact defect mechanism.

Source path: `cmdconf.c::ExpandConfigFiles` reads selected CONFIG.NT and
`cmdredir.c::cmdCreateTempFile` obtains a TEMP path, uses GetTempFileName,
and creates the transient file. `cmdconf.c` normalizes the generated path via
GetShortPathNameA and checks its length at most 63, then
`cmdGetConfigSys` converts it to OEM and copies it into the DOS-provided
64-byte buffer. `sysinit1.asm::ProcessConfig` requests it via
SVC_GETCONFIGSYS; `sysconf.asm::doconf` opens that path through DOS INT21 and
returns without parsing if open fails. Later `sysinit1.asm::comerr/stall` can
invoke `SVC_DEMEXITVDM`, and `demmisc.c::demExitVDM` emits the fixed
`config.nt` modal error. COMMAND initialization version mismatch can also
invoke DEMEXITVDM. This is a plausible chain, **not an observed failure at any
particular edge**; no guest register, DEM-open status, or file path was
captured in tmp3.

Read-only `cmd` short-name expansion on this host returned
`O:\\repos.hobby\\ntvdm64\\build\\package-uuid` unchanged, including a
component longer than 8.3, while `O:\\tmp` remained DOS-shaped. The path plus
typical `scsXXXX.tmp` name is around 53 characters, so this is not evidence of
crossing the explicit 63-character bound. Historical
`docs/history/m0-t245-consolidated-history.md` documents a different original
DOS bootstrap failure when a host component lacks an 8.3 alias. The matching
symptom and path spelling justify a focused guest CONFIG/DEM-open trace, but
are insufficient to conclude this is the same failure. Capture actual
returned temporary path, INT21 open result, later shell path and the exact
caller of DEMEXITVDM before attributing the root cause.

Evidence SHA-256: tmp3 script
`2DF7F3932C36790CDCB368FF196507BAA9A84D0DEE4991107B167E0191AE733F`;
tmp4 script `B565E267AB1CACFBD567C2D23B881CA00592235E16FCC8978D6A1BE5F964C998`;
selected session test script
`7B786A4EE8FD0E5EFD65159C39C13852D5352D2F4039B404D3D3FCE00E2D467E`;
DOS session reports A/B
`74120AD5159690C4F18331DF49B19790502845DA03B860CA780CB85E9E9EA3B8` /
`6171E4EE83B9DD21EE2A58D295B565E4CA25240B862BDFEB206BFDF55F696F77`;
tmp3 timeout-live A/B
`5E841F0054523C26E27BC9F515519A19DC0734A8E5248150832A8C871FFB4087` /
`3E720710BBB9159A2E94D4FEB72AEAF00C1A9CC5CCA277C51160A6CE4269BB78`.
