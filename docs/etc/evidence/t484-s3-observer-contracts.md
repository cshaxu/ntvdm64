# T484 S3 P206: native observer regression producers

Five consecutive direct-command candidates are resolved. Four remain useful:
late Console API observation, startup Console tracing, restricted installation
writes and palette tracing. Their C sources and actual runtime assertions are
unchanged. One native CMake module shares the existing startup-hook provider
composition. Late observation reuses the existing normal/fault native DLLs and
keeps the required ntcon image basename through a native copy target. It no
longer depends on an old task's prebuilt probe directory. Runtime assertions
remain PowerShell; no compiler recipe or source generation remains in them.

The fifth candidate, verify-dos-style-relay.ps1, referenced a nonexistent
integration C fixture and obsolete worker archives, with no current caller.
Delete that builder rather than migrating it. T445 historical evidence remains
historical; this deletion does not claim fresh DOS style acceptance.

Owned runs live under build/t484-s3-observer-contracts. PID16008 failed before
compilation because old runners duplicated an absolute output path. Resolve
relative/absolute inputs against the script's repository root. PID14076 then
passed late Console normal and injected-failure cases in four private-desktop
processes each, including preserved API errors, rollback, thread progress and
module lifetime. It subsequently failed compiling Console tracing because the
old forced include process_hook_msvc.h was removed in T479 S2 (967ef3d31).
Current startup_hooks.c already uses _MSC_VER/__assume; remove the stale include
rather than reintroducing the shim. No library algorithm change.

Run-complete.ps1 PID19084 preserves those completed late-observer results and
runs Console tracing on x86/x64, both x86 installation guards with outside-root
write rejection and target rejection, and palette tracing on x86/x64. It also
requires cached full host build and11 raw/22 published identity checks. Real
native calls and errors are checked; none is a new guest installation or Full
acceptance result. Unchanged product bytes do not need redundant publication.
Governance, parser and staged diff checks precede commit/push.

Eight direct-command review candidates remain. Frozen316/355 and23 supplemental
preparation/staging chains are unchanged and cannot be summed. S3 is active;
S4/S5 remain pending.
