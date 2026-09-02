# M0 T384 closure — CPU40 illegal-operation startup recovery

## Closed result

T384 closes at its changed-owner stop condition. Its source audit proved that
the reached CPU40 opcode `0x63` path is the original and correct downstream
reporter of a pre-CPU40 guest-memory ownership failure; it is not a missing
ARPL implementation.

- Original COMMAND obtains its initial environment segment through DOS
  `INT 21h/AH=48h`, puts that returned segment in `ES`, then invokes `54:0F`.
- `54:0F` retains its original `ES:0` destination contract. It does not choose,
  alter or own the destination segment.
- The fixed observation found inherited `codex` environment text at
  `03f4:2128`, within resident COMMAND memory, before the first `54:01`.
- Original CPU40 correctly routes the resulting real-mode `0x63` byte through
  `illegal_op_int` and the original error path.

## Successor boundary

M0 T385 owns the original DOS arena/MCB initialization and SoftPC guest-memory
preconditions. Its runtime priority is pure DOS first, followed by DOS-initiated
native 32-bit execution. WOW16/Win16 activation remains deferred.

## Evidence

- [COMMAND environment/resident-overwrite ledger](../etc/evidence/m0-t384-s1-command-environment-resident-overwrite-ledger-001.md)
