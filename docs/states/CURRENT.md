# Project Status

## Current Work

**No active M/T/S packet.**

M0 T413 S4 completes the owner-requested autonomous-binding cleanup and
subsequently approved IP-wrap repair. The [closure audit](../history/m0-t413-accessor-restoration-closure.md#s4-cleanup-and-owner-expanded-ip-repair-closure)
and [S4 evidence](../etc/evidence/m0-t413-s4-binding-cleanup.md) qualify the
earlier S1-S3 closure. No next candidate is admitted; Queue order is unchanged.

## Current Technical Baseline

- O:\winnt contains the three formally built x86 /MT CCPU40 programs,
  APP_VERSION 0.0.413 / protocol 3. Published hashes match S4 formal products.
- All 76 field-access bodies are unchanged original cpu/src/evid/vglob.c,
  called through original egacpu.h declarations. Autonomous accessor table
  rebinding and all 20 typed conversion wrappers are removed. A registered
  seven-line header seam preserves the full original latch/table layout.
- The original IP-update and page-advance expressions are restored, including
  masking the complete USE16 sum. The retired x64 helper is removed.
- Full accessor/layout/concurrency fixture, 168 IP-expression cases and 11
  functional native instruction cases pass. All 17 command cases, four ConPTY
  sizes, mouse, live resize, native resize contracts and five prefilled 80x5
  EDIT runs pass, including 20 post-EDIT MEM completions.
- Real-mode divide-by-zero still times out on both the prior and current
  worker through unchanged original Int0/NTVDM continuation. It is explicit
  [Debt](TODO.md), not a functional test pass or a repaired fault-delivery claim.
- Mirror diff distance: 417 versus pre-task 506. Four-overlay cohort: 125,
  back to its pre-task size. Generated runtime binding include loses 176 lines.
  No algorithm is moved into an adapter to disguise its footprint.
- Formal products: build/M0-T413/S4/formal. Fixtures and recoverable previous
  products: build/M0-T413/S4/integration. Runtime logs: O:\winnt\logs only.
  User Terminal/RDP visual acceptance is separate and not asserted as received.

## Recent M0 Closures

T413 S4 supersedes the earlier accessor-binding and IP-exclusion dispositions;
see its requirement/evidence closure above. T412 Broker remains closed within
its recorded architecture. T411/T410/T409/T407/T406 retain their own scopes.

## Recent Governance

S4 admission is 65aa091f6. Source changes, explicit retained boundaries,
adverse test evidence, build identities and source-footprint measurements are
reviewed together for S4 P2. The next candidate remains x87 host-layout
restoration; no new task number is allocated.
