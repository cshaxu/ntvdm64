# M0 T184 bounded v5 finite-sequence observation plan

## Packet purpose

Observe once, through the completed CLI shim/bundle boundary, the already
implemented two-slot `TARGET` then `QUIT.COM` composition. The packet is an
evidence operation: it cannot repair or extend Bochs, adapter, BOP or guest
semantics.

| S | Scope | Initial disposition |
| --- | --- | --- |
| S1 | Inventory exact v5 profile/root, runner/shim/bundle, target identities and durable controller. | Active in Status. |
| S2 | Define one fixed watchdog envelope and its success/unfinished classification rules. | Pending S1. |
| S3 | Run exactly one watchdog-bounded observation and classify its retained evidence. | Pending S2. |

## Invariants

- Only the runner passes BYOB metadata; the shim remains the sole native
  Bochs launcher.
- The v5 profile has exactly target then source-built `QUIT.COM`; it contains
  no host filesystem, queue or result-transport capability.
- One process-tree launch is the whole observation budget. Timeout, launch
  failure or incomplete evidence consumes it and is recorded, not retried.
- No observation outcome establishes normal DOS return or CLI exit semantics.
