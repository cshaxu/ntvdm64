# M0 T477 host frontend foundation closure

The approved host foundation is complete through S4 P1. Its independent
frontend groups follow the owner's revised UI: CONSOLE with worker chain,
VM with worker chain, WOW/Win16, native observations, then unassociated
workers. VM PID is hostPID-VMID and TASK is empty; VM rows are read-only.
This closure does not admit the separately queued guest bridge.

## Required scope and evidence

| Requirement | Delivered owner and evidence |
| --- | --- |
| Reusable frontend mechanics, native presentation retained | [S1](../etc/evidence/m0-t477-s1-frontend-base.md): common/frontend-base and NTCON adapter; channel/service isolation and product regression. |
| Backend/frontend dependency direction | [S3](../etc/evidence/m0-t477-s3-host-composition.md): NTVDM direct composition; worker-base has no frontend-base dependency; ownership gate passes. |
| Scoped payload/VM registry and adapter lifecycle | S3 registry, handle reuse, retirement, malformed identity and failure tests; 171 assertions and seven exit cases. |
| Authenticated independent frontend chains | [S2](../etc/evidence/m0-t477-s2-frontend-instances.md) and S3: real RPC admission plus x86/x64 service fixtures for nesting, completion orders, parent loss and sibling preservation. |
| Management projection and coherent acceptance | [S4](../etc/evidence/m0-t477-s4-vm-groups.md): actual service/RPC/renderer checks, missing groups, stable identities, read-only VM actions and retained worker operations. |

NTSRV retains control and handoff authority; I/O stays directly between the
active worker and its frontend. Upstream backend ownership is not consumed
by downstream VM frontend activity. Immutable retained association metadata
supports missing-owner display without a second mutable relationship registry.

## Delivery and limits

Cached x86/x64 builds passed. The final S4 runner exited zero; Full passed all
16 selected gates in 400331 ms. The coherent ten-image package
7b89c5d6-c8e0-472b-8aa8-b7f174c71ff8 is hash-verified at assets/release and
O:/winnt/system32. Detailed corrective attempts, tests and source accounting
are in S4 evidence. The delivery commit contains this closure and manifest.

No guest or mirror source was changed or rebuilt. Host fixtures establish the
foundation, not an operational guest VxD, guest VM discovery or guest copied
I/O. The guest driver/NTVDM bridge remains in the existing queue. WOW startup
witnesses do not prove gameplay; DOS4GW/NESticle remains the pre-existing
explicit exclusion. Earlier mirror-audit evidence limitations remain with
their recorded follow-up and are not relabeled as proved by these tests.
