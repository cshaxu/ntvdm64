# Current ten-build-input candidate

The latest x86 worker relink made both VDMREDIR and nthook32 links stale in
their generated Ninja graph; they were relinked. The selected executable or
DLL targets for x64 service, frontend, monitor, worker, launcher and hook,
and x86 worker, redirector, hook and WOW32 then had no remaining dry-run
actions. Ten unsealed inputs were copied directly from those cache graphs and
sealed together. Each manifest input hash matches the copied build input.
The ten-image manifest SHA-256 is
`53E1E57F50DF5712400A39BEAE2F5B52505AFB0B4651BD2976AE3FFA6DCC7320`.
The immutable guest manifest passed for 59 files, excluding configuration.
This checks selected target freshness, not every default-target output or
unrecorded source dependency.

The exact package passed the existing Console-17 and Window-17 exit-status
matrices: each summary has 17 cases and no expected/actual mismatch. The
runner exited zero and cleaned its isolated stage. All 12 selected Control
gates subsequently passed; their runtime hashes were independently matched
against all ten sealed images in this manifest. The first Control attempt
failed before running a gate because the verification cache mistakenly held
unsealed inputs instead of sealed package bytes; the corrected attempt used
the exact sealed copies. Summaries and manifests are retained here.

The same manifest passed three SOL Options interactions and one each of
Notepad Find, Write Find and Program Manager New Group; the probes checked
dialog close, parent recovery and launcher exit. SOL opening took
1.844–3.468 seconds and closing 1.906–2.235 seconds; the other application
timings and the four separate reports are retained in their subdirectories.
These bounded cases do not resolve the multi-window performance issue,
intermittent 1400, all WOW workloads or the Full gate. No product image has
been released from this candidate.

SOL gameplay subsequently passed stock draw, Deal with a nonempty waste,
redeal, another draw and normal close/launcher exit. Its ten tested host
image hashes match the sealed package exactly. The shared-WOW idle/dialog
witness passed with two tasks, 17 dialogs, 16 explicit closes, later idle
dispatch and both task exits. That witness uses a fresh package identity;
all ten recorded pre-seal input hashes match this candidate's inputs. Their
separate manifests, reports and guest checkpoints are in `sol-gameplay/`
and `idle-dialog/`. These focused observations do not cover Winmine, resolve
modal latency or replace Full.

The current cached WOW32 bridge object also passed the selected BOOLEAN
return/cleanup fixture and eight controlled Windows loader attach/unload
cycles (`bridge-result.json`, `bridge-loader-result.json`). The relinked
selected CCPU far-transfer fixture passed trace-off, matching/nonmatching
trace and valid-descriptor controls, with owner and object hashes in
`far-exception-result.json`. The latter retains its existing `/FORCE:multiple`
test-link warnings and checks a controlled fault boundary, not whole guest
exception delivery. The loader uses a controlled provider, not a complete
real WOW guest DLL lifecycle.

The current selected-object `native-nested` fixture also passed against the
current worker/provider objects. It covered nested callback ownership, native
wait unwind, exceptions with and without the USER lock, window destruction
during native return, a dead target and receiver retirement (including
expected error 1400), with execution ownership restored. Output, exit status
and source/binary hashes are retained in `native-nested/`. This is controlled
coverage; it does not identify the separate intermittent SOL failure or prove
every native call/teardown interleaving.

The final ten-input candidate also passed the real-worker VDD/MEM export
witness: query, allocate, reallocate and free succeeded, and a deliberately
exhausted reallocation preserved the original allocation and free-space
state. The unchanged guest and launcher exited zero. The test reseals a
fresh isolated identity; all ten underlying input hashes match this
candidate. Receipts are in `export-live/`; this is not an exhaustive VDD,
guest, or failure-path test.
