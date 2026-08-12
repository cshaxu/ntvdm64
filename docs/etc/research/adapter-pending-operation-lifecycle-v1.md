# Adapter Pending-Operation Lifecycle v1

## Decision

An adapter session owns exactly one pending operation. A write transaction and
a future ordinary-RAM observation transaction use the same slot; neither has a
parallel queue or a bypass into Bochs memory.

## States

```text
none
  -> pending write          (only after admitted host-service preparation)
  -> pending observation    (only through same-island profile-owned request)
  -> none                   (single successful take, mismatch, reset or teardown)
```

Each pending operation stores a copied exception event and copied CPU state at
the #UD boundary that created it. A take operation must present byte-identical
validated copies. A mismatch cancels the operation before exposing a payload
or observation output. A second take always fails.

## Write Path

The existing source-derived `50:11` NTDOS transaction remains the only runtime
write producer. It queues the transaction and its adapter-owned NTDOS payload
with the boundary that caused its prepare result. Bochs passes its current
copied event/state back to the same-island adapter before calling the generic
ordinary-RAM commit primitive. Thus a prepared write cannot be consumed after
a different exception boundary.

## Observation Path

The same slot can hold `bx_ntvdm_observation_transaction_v1` and an
adapter-owned output span. Its queue/take functions are currently confined to
`bx_ntvdm_host_session_v1`; neither the adapter runtime public API nor Bochs
calls them yet. They provide the cancellation and output-lifetime rules needed
before a profile can request a stopped machine observation.

## Reset and Lifetime

`bx_ntvdm_adapter_runtime_v1_reset()` releases BYOB images and zeroes the
entire session, thereby cancelling any operation and removing every private
payload/output pointer. No pending record, range or pointer crosses session
teardown.

## Verification

`bx-ntvdm-host-session-test` verifies:

1. an admitted `50:11` write is available only at its exact original boundary;
2. a mismatched CPU snapshot cancels that write;
3. a re-created write can be consumed once, then is absent;
4. an observation queues only into an empty slot, cancels on mismatch, and
   transfers its output span once on a matching boundary.

`bx-ntvdm-adapter-runtime-test` retains the independent BYOB identity-to-write
transaction path. Both targets passed in
`artifacts/build/current/adapter-cli-r1`.

The r12 explicit native Bochs link was refreshed with the revised
`cpu/exception.o`, all adapter objects and the observation ABI object. The
existing adapter runtime fixture again completed reset -> `50:11` commit ->
controlled stop. This proves the tightened write-consumption boundary; it does
not make observation callable, boot NTIO, or run COMMAND.
