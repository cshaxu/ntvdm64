# T194 S14 v6 runner/shim/bundle invocation audit 001

Date: 2026-08-12  
Packet: M0 T194 S14  
Disposition: binary-identity blocker recorded; no process was started.

## Question

Can one existing source-built CLI runner, thin shim, and T194 S10 r2 engine
bundle accept the S13 v6 BYOB root while preserving the declared child
environment and fixed engine boundary?

## Inputs

- Current source contracts: `src/cli/ntdos64_run.c`,
  `src/cli/ntdos64_bochs_engine.c`, and `src/cli/byob_profile.c`.
- Retained runner: `artifacts/build/current/t181-local-mingw/ntdos64-run.exe`
  (SHA-256 `D5BE9BDC4E5510ED1585B1827131559285A5147A8611F7185B556226E1A24DC8`).
- Retained shim:
  `artifacts/build/current/t183-cli-bundle-mingw-r1/ntdos64-bochs-engine.exe`
  (SHA-256 `4D8750B9D9C6FFFAF39643EFCF4668F8DDF83EC114166DFCE6B6435C07D80B59`).
- S10 bundle engine:
  `artifacts/build/current/t194-s10-v6-observation-bundle-r1/ntdos64-bochs.exe`
  (SHA-256 `D9060D2201BE0DD8F650D44ADFD1F503D9549C1699952F6ACE2F84AC623E758C`),
  with manifest SHA-256
  `557B21A5C6409B57727DDC8870E079E853FD72C076708F8820D55A6A13F19F7A`.
- The S13 v6 root and its `ntdos64-byob-profile-v6` profile identity.

## Procedure and observations

1. Reviewed the runner source. It validates a profile before launching the
   shim, serializes the immutable launch plan, and places
   `NTDOS64_ADAPTER_PROFILE`, `NTDOS64_ADAPTER_ROOT`, and
   `NTDOS64_ADAPTER_LAUNCH_PLAN` in the child-only environment. It invokes the
   shim with its fixed eight-argument ABI and does not pass a Bochs option or
   BYOB path directly to Bochs.
2. Reviewed the shim source. It accepts only that eight-argument runner ABI,
   verifies the named engine and three ROM hashes from the fixed bundle
   manifest, creates its private run directory, and invokes the bundled native
   engine with only its derived fixed Bochs command line. It does not parse a
   BYOB profile and its source has no change after the T183 source baseline.
3. Compared current source against that baseline. `ntdos64_run.c` has since
   changed by 33 lines; `ntdos64_bochs_engine.c` has no source delta. The
   runner's profile parser now supports v6, but this is statically linked into
   the runner executable.
4. Searched the retained runner and the T183 runner as binary text. Each
   contains `ntdos64-byob-profile-v1` through `v5` and contains no
   `ntdos64-byob-profile-v6`. Therefore neither binary can admit the S13 v6
   profile. This is an ABI/input-identity failure before any child process or
   guest execution, not a runtime result.
5. Checked the S10 bundle engine hash against its manifest. It matches exactly.
   The retained shim's fixed process ABI remains source-compatible, but no
   existing v6-capable runner completes a runnable triplet.

## Interpretation and confidence

Confidence is high. The v6 profile is deliberately rejected by every retained
runner binary, while the shim and r2 bundle are not implicated. Running the
S10 controller with a v5 runner would be a stale-input experiment and is
prohibited.

## Follow-up

T194 S15 may build the current MinGW CLI runner and the unchanged thin shim as
one source-built, hash-recorded CLI-only pair. It must not build Bochs, alter
the bundle, start the engine, or run guest code. A bounded observation remains
a later, separately admitted S.
