# M0 T450 S5 — Run16 Console entry normalization

## Outcome

`run16.exe` remains a Console-subsystem launcher, but now uses the matching
standard `wmain` C runtime entry.  It gets the full command line from
`GetCommandLineW`, extracts only the executable token, and gives the
remaining raw tail to the existing launcher-prefix parser.  It never rebuilds
the target command from `argv`.

This preserves Console inheritance for CMD callers and keeps shell syntax
(`>`, `>>`, `|`), quotes and target arguments untouched.  No GUI subsystem,
RPC, worker, frontend, guest image or guest configuration behavior changed.

## Evidence

| Check | Result | Evidence |
| --- | --- | --- |
| Raw-tail parser | Pass, 22 existing/prefix cases plus full-line cases for `cmd /c ver`, `>`, `>>`, `|` and quoted targets | `build/t450-s5/focused.stdout.log` |
| Generated entry contract | Pass: x86 and x64 generated Run16 links use Console + `wmainCRTStartup` | `build/.ninja/msvc-x86-release/worker/build.ninja`; `build/.ninja/msvc-x64-release/launcher/build.ninja` |
| Coherent build | Pass: all ten images, x86/x64, 1,476 intermediates unchanged on second increment | `build/t450-s5/component-build-r2/result.json` |
| Stage and publication | Pass: ten staged images, then exact SHA-256 agreement among candidate, `assets/release` and `O:\winnt\system32` | `build/t450-s5/stage-r1/product-system32-manifest.json`; `build/t450-s5/publish-r1-result.json` |

## Runtime observation limitation

The installed `run16 --wait cmd /c ver` process printed the expected Windows
version and its Run16 process exited.  The ad-hoc observation wrapper used
redirected pipes; the launched child chain inherited those pipes, preventing
that wrapper from receiving EOF.  It was stopped with its own spawned NTSRV
test session.  This is not recorded as a runtime pass or product failure;
the deterministic tail contract is instead covered by the focused unit test.
