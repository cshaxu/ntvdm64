# M0 T465 closure — selected classic WOW32 DDE

T465 closes the selected original classic `WM_DDE_*` transport.  It preserves
immutable guest media and makes no production-image change.

## Delivered sequence

- S1 separated the selected classic transport from the excluded private DDEML
  provider and mapped its original alias/callback authorities.
- S2 added a real guest/native INITIATE/atom-ACK, bidirectional EXECUTE/ACK,
  DATA/POKE/ADVISE and rejected-post witness.
- S3 proved the exact release boundary: rejected conversion rollback is
  original-owned, while task exit does not silently free application-owned DDE
  allocations.
- S4 reran the aggregate transport and task-loss witnesses plus the exact
  selected-body alias fixture.

The detailed records are [S1's contract](../etc/evidence/m0-t465-s1-dde-conversation-contract.md),
[S2's conversation acceptance](../etc/evidence/m0-t465-s2-classic-dde-conversation-acceptance.md),
[S3's loss/release ledger](../etc/evidence/m0-t465-s3-dde-loss-release-ledger.md)
and [S4's aggregate acceptance](../etc/evidence/m0-t465-s4-dde-acceptance.md).

## Boundary and package status

The closure covers only the selected original classic conversation and alias
bridge.  DDEML, cross-worker DDE, arbitrary peer/module-loss reclamation and
unselected malformed-peer recovery are not capabilities of this delivery.
They are explicit boundaries, not deferred hidden implementation.

Only tests and evidence changed.  No guest source/media, production source,
release asset or installed file changed; therefore the existing T464
`0.0.464` package remains the current published runtime identity.

## Successor

The next ordered candidate, WOW32 sound and multimedia, is automatically
admitted as **M0 T466 S1**.  Its first task is a source-first operation,
device, buffer, callback and cleanup inventory; it may reuse T465's public
message/callback evidence, but not treat classic DDE as a multimedia service.
