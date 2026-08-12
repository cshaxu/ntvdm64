# M0 T181 finite declared target sequence plan

## Packet purpose

Design one whole, immutable replacement for the unavailable historical
COMMAND host broker.  The provider supplies a finite profile-declared guest
target sequence through the existing `54:01` ABI and terminates with the
already source-built `QUIT.COM` controlled-stop contract.  This is an
admission/design packet, not permission to implement a command broker.

| S | Scope | Initial disposition |
| --- | --- | --- |
| S1 | Define the source-derived sequence state machine: slot identity, materialized guest file, command tail, first/repeated `54:01` delivery, commit boundary, and final `QUIT.COM` stop. | Complete; the two-slot contract and owner/rejection map are recorded in `t181-s1-finite-declared-target-sequence-contract-001.md`. |
| S2 | Audit profile, CLI launch declaration, namespace and current provider ownership; define the minimal immutable schema and rejection rules for undeclared, duplicate, out-of-order and exhausted requests. | Complete; the v5 proposal, replacement map and rejection matrix are recorded in `t181-s2-immutable-sequence-ownership-audit-001.md`. |
| S3 | Admit or reject a contained implementation/verification slice after S1/S2 prove a stable object and ABI closure. | Active in Status. |

## Invariants

- No `GetNextVDMCommand`, Win32 COMMAND broker, host process creation, ambient
  queue, or ambient namespace.
- Each `54:01` response is an existing bounded copied transaction, not a new
  Bochs or DOS mechanism.
- Every guest file is source-built or otherwise already provenance-classified
  and is explicitly named by the immutable profile.
- `QUIT.COM` is a terminal guest slot only.  Its `C4 C4 FE` controlled stop is
  not evidence of `54:11`, normal DOS result transport, or host process exit.
- No adapter/Bochs/CLI code, build or runtime observation is admitted until S3.
