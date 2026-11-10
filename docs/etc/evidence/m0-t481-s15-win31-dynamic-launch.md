# T481 S15 - dynamic Windows 3.1 setup launcher

## Scope and contract

Owner-approved replacement of install-time DOSONLY selection with one installed
PATCH/WIN.CMD. Setup always copies the two released .386 addons and PIF.EXE;
it writes two commented device entries through WIN.CMD's internal --install
operation. Runtime choices select Standard/Enhanced, DOSONLY0/1, and (Enhanced
only) the addon pair. Standard comments both entries. Each launch creates
CONFIG.NT, AUTOEXEC.NT and WIN.PIF using the existing PIF utility.

Only the two matching device entries in [386Enh] are toggled, in place after
installation. Other INI lines are preserved. There is no template PIF,
MULTITASK.CMD dependency or new executable. A short-lived WINCFG directory
holds prepared files; failure stops before run16 and cleanup removes owned
scratch. No recovery framework or runtime timeout is introduced. Original
guest executables and host runtime code are unchanged. PIF legacy path-size
limits still apply. A SYSTEM.INI containing ! is rejected, retaining the
existing CMD editor boundary rather than corrupting delayed-expansion input.
Do not change a running installation's configuration.

The pre-existing standalone win-multitask tool remains for its other callers;
this setup package no longer distributes or invokes it. All executable inputs
come from assets/release. Public patch execution uses CMD and existing addon
utilities only; development tests may use PowerShell.

## Verification method

win31_setup_profile_generation_test.ps1 runs real SETUP.CMD and WIN.CMD with
a run16 shim: six choices, repeated driver toggles, exact generated PIF fields,
NT profiles, unchanged unrelated INI and guest files, failure without stale
launch and child exit-code propagation. This is script-contract evidence.

verify-win31-setup-modes.ps1 invokes the real WIN.CMD selection with a launch
shim, then the existing private-desktop runtime matrix executes the generated
WIN.PIF against real NTVDM. PreparedProfile verifies the selected mode and
DOSONLY without replacing that selection. Only the ordinary guest witness is
appended to AUTOEXEC for observation. This distinguishes configuration testing
from a fully automated interactive installer run. Existing six scheduling
contracts, two Standard controls, fullscreen and paired-prompt cases remain.

## Preliminary results and corrections

The first profile fixture used a repository path exceeding the original PIF
path capacity; it correctly failed creation before launch. Short build/s15a
and build/s15b fixtures passed. No PIF format or product workaround was added.
The first runtime invocation had a PowerShell one-element nested-array
flattening mistake and attempted to inspect PATCH/W; this was a test error,
not a product failure. It was corrected before runtime acceptance.

Package tests in background PID4092 passed: six-profile contract, incomplete
installation rejection, media APPLY/reapply/UNAPPLY with compressed-original
hash restoration, unknown PIF preservation, and original Setup exit-code37
propagation. The media source was read-only O:/ewin31; tests copied only the
needed files and original .BAK identities into the task build directory.
No original media or installed guest was changed by these tests.

## Deferred boundary

The owner-directed S14 EDIT graphical-return mouse issue is explicitly deferred
in [its handoff](m0-t481-s14-win95-launch.md#s14-final-scheduling-verification-and-owner-directed-closure).
This launcher does not change VMD routing or install the pair as a workaround.
Win95 profiles and remaining mirror audit are outside S15.

## Delivery checks

Cached MSVC x86 and x64 builds both returned 'ninja: no work to do'.
Package-contract runner PID22472 passed the developer-produced media payload
and installed self-contained file checks. Profile test build/s15d also proved
old WINSTD/WIN386 CMD/PIF cleanup and correct child exit37 propagation.
Governance and all 11 host hashes at assets/release, O:/winnt/system32 and
the tested candidate passed in background PID22672. The first combined gate
wrapper misread LASTEXITCODE left by an expected negative test; the independent
governance process returned success. This was not a documentation failure.
No host or addon binary rebuild/replacement is needed; published identities
and the preceding Full result remain applicable to the unchanged runtime.
The patch tools are delivered in this repository's tools/win31-setup, not
in winnt.zip or host system32.

## Final integrated result

The first final matrix produced 14 passing runtime records, but its final
source-preservation guard detected O:/w31/DOSAPP.INI changing during the run.
The user-owned formal NTVDM PID5492 (started 12:34:18) remained alive; the changed
entry named the user's DOSPRMPT.PIF, while test prompts were DOSCHECK/DOSPAIR.
This supports a live-source write explanation but does not prove the writer.
The guard was not waived and that overall run was not recorded as a pass.

The established New-Win31ProbeCopy helper then made an independently owned,
rebased static source at O:/tmp/s15base. The unchanged final matrix ran from
that source in background PID14720, started 2026-11-10T21:39:07.9619472Z;
script SHA-256 D092BB6152CD6CE687CC4E2EBBF26F47679BB30EBACB9E61A1EC22690FEC554D.
Run output: build/t481-s15/runtime-sealed and job/sealed.out. Overall exit0,
all 14 matrix records passed and normalExit=true; all six scheduling records
passed their blocking/nonblocking contract. The static source's full file
inventory and hashes remained unchanged. O:/tmp/q31 and O:/tmp/s15base were
removed by their ownership-aware helpers. No user process was stopped.

Final production scripts also passed the build/s15d generation/cleanup and
exit-code tests. The only late installer change removes the four obsolete
generated launchers after successful configuration; that behavior was tested
with seeded legacy files. No original graphical Setup rerun is claimed:
its invocation/error propagation is unchanged and tested with a shim, while
the generated profiles were exercised by real Win3.1 in the private desktop.
The physical-RDP mouse path and the deferred S14 graphical EDIT issue are not
certified by these results. Publication consists of the updated repository
patch tools; all 11 host images retain their existing verified identities.
S15 remains open for owner acceptance, not automatically closed.