# T481 S9: owner-reported direct Win3.1 startup failure

The owner reported `run16 O:/w31/patch/win386.cmd` terminating before the
desktop with illegal instruction bytes `63 6f 64 65 78`. S8 had not received
owner acceptance. The selected matrix previously launched the batch from an
existing CMD and replaced PATH, so it did not cover this direct inherited-PATH
case. `win31_runtime_matrix_test.ps1 -DirectRun16` now provides that route;
`-DirectShortPath` is its isolated short-PATH control.

On independent Win3.1 copies, the current published S9 candidate and the S8
package both timed out at the same illegal-instruction dialog, CS:04D3,
IP:2032 and bytes `63 6f 64 65 78`. Thus this reproduction predates S9. The
same S9 image and direct invocation passed desktop, mode and normal exit
when only its isolated test process PATH was shortened. The first attempted
long-name test copy instead produced a `config.nt` error and is not evidence
of the owner's instruction failure. The original W31 installation was checked
unchanged; neither environment control changes the user's system environment.

A separately linked diagnostic host wraps the original environment copy and
only reads guest state. In the matching inherited-PATH failure, the COMMAND
environment block (ES=057E, 018C paragraphs) overlapped DS:203C, the released
INIT `EnvSiz` location. Its value changed from 018C before copying to 7261
afterward; the run then produced the same illegal bytes. In the short-PATH
control the allocated 010A-paragraph block did not contain DS:203C, the value
remained 010A, and Windows started and exited normally. These observations
match the previously retained original guest defect in TODO, without proving
the guest state of the owner's already-closed process.

No product fix has been made for the guest defect: current authority explicitly
prohibits modifying the original guest, automatic environment truncation,
injected `/E:` or allocator special cases. Shortening PATH is a per-invocation
manual workaround, not a fix or a general safe environment-size threshold.
Do not claim the reported acceptance passed. The S9 image's graphic fixes and
the direct-start environment failure have distinct evidence and disposition.
