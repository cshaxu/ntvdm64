# T475 S3 — Worker Base and NTVDM dependency remediation

## P1: VDMREDIR Worker Base input

The owner directed sequential delivery of the S3 design. The x86 production
link for VDMREDIR no longer selects `worker-base.lib`. The real link succeeded;
the ownership verifier now checks VDMREDIR and Hook32, with a negative control
that injects the prohibited archive into VDMREDIR. NTVDM and NTVWM still select
all five Worker Base sources. No product source or ABI changed in P1.

The x86/x64 product build, source ownership and negative controls passed.
Redirector controlled async publication passed 24 checks; OEM capacity and
sequential aliased guest writes passed 15. The coherent ten-image package was
staged, 63 immutable guest files and 274 export ordinals verified, and all 12
selected Control gates passed. Real DOS async-read, async-write and NetAPI
probes passed in a helper-owned short physical stage, with a fresh package ID
and the established test-only bounded PATH. Local package
`fd19d65e-2eec-4bb8-a76a-697b9841c4d3` was published in this worktree with
ten matching hashes. Full was not run; DOS4GW/NESticle was absent.

The checked-in guest boundary runner previously used a global `subst` drive;
it now uses the existing marker-checked physical staging helper and restores
its test-only environment. The first candidate and prior-release baseline
both timed out at guest startup with inherited 2423-character PATH. The
baseline and candidate both passed all three probes with the established
bounded test PATH. This is a controlled result consistent with the known
long-PATH issue, not a claim that its product root cause was repaired.
Logs, commands, start/end times, owned PIDs, hashes and prior release backup
are under `build/t475-s3-p1`. No main-runtime deployment or push occurred.

## P2: shared guest-memory contracts

The guest-memory lease API and redirector copy declarations now reside in
`common/guest-memory`. WOW32 and VDMREDIR consume those declarations without
including the former NTVDM-private headers. The worker retains its single
session-owned memory provider and the existing export ordinals. Compiler-recorded
dependency checks passed, including injected private-header negative controls.

The x86/x64 build, guest-memory lease tests, original WOW callback-frame fixture,
redirector OEM and async fixtures, and 12 selected Control gates passed. Actual
DOS async-read, async-write and NetAPI probes passed in a freshly identified
physical stage with the bounded test PATH. Staging checked 63 immutable guest
files and 274 NTVDM export ordinals. Local package
`8ac27d1f-ff63-4b10-b8f8-2afd5f83d39b` was published with ten matching hashes.
Full was not run; the optional DOS4GW/NESticle fixture was absent. Logs and
the previous release backup are under `build/t475-s3-p2`.
