# T225 S48 Source-Built Guest Lifecycle Ledger Result

## Question

Can the formal x64 /MT closure retain every reached accepted original lifecycle
identity (50:36, 50:3C, 54:0B) in arrival order without changing BOP, guest,
machine, or mutation-profile behavior?

## Inputs and procedure

The fixed-capacity (16-record) default-off bx-vdm ledger copies only the
existing typed generic-#UD event and its already selected outcome. It accepts
only vector 6, C4 C4, the three named family/service pairs, and the existing
checked resume (fault_rip + 4). It does not inspect guest RAM or classify a
target.

A fresh formal graph was generated at build/M0-T225-S48/lifecycle-r3; manifest
SHA-256 is 62e1718937f871f3da9e8ad5e20577d5ef67957d75a5544dbaf679b2e50e8b41.
Ninja completed all 344 declared outputs. The focused ledger fixture passed,
including default-off, repeated ordered records, rejected non-resume, capacity,
and overflow behavior. The existing four-image fixture passed for Direct and
Readonly (and explicitly rejected deferred Overlay and retired Virtual).

The fresh inputs-r1 stage revalidated the four original-toolchain artifacts:
NTIO cfc8be16576bb6acd16bb8fc9b2d9a080f544bbfdd2d2d2df07ed908b3ab4937,
NTDOS 957662320654ad5251c3a8b228a5dadec28aa65dddbcba38c3658a6e7f93bc84,
COMMAND 908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43,
and TARGET.EXE (byte-identical source-built SHARE)
69dabbdb754b358ac4fe4b22de04c0e4c93076816f14bb0730caa9fd223996fc.

One bounded 1,000,000-tick native invocation was run in each supported mode
with the same staged inputs and only the lifecycle-ledger option. Both returned
the established execution-budget result (exit 4), and their logs were
byte-identical: 4f0153fc7fc69555dc02450ad3e7c07e9f094fe7465ea3d15c6815d70674f407.

## Observations

Both modes recorded exactly one ledger item: phase ENTRY, cs=0041,
eip=00005a70, ds=0c41, esi=00000100, disposition RESUME, resume=00005a74;
overflow was zero. Neither 50:3C nor 54:0B was reached inside this bounded run.

## Interpretation and limits

This proves the ledger is default-off, non-mutating, bounded, ordered, and
profile-neutral for the reached event. It does not prove that the entry is the
selected MZ SHARE target, that a child ran, or that termination/parent return
works. The continued budget result is an observation, not authority for a
trace-selected repair.