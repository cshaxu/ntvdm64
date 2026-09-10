# Adapter Host Session v1

## Purpose

This is the single adapter-owned runtime coordination object for admitted CLI
BYOB resources, a copied CPU event, and a preflighted host-service result. It
prevents a per-service pile of callbacks: every future host service enters
through the same session, catalogue, transaction, and commit path.

It is not a guest ABI and is not part of Bochs. Its payload pointers are
same-process staging references, never fields of the fixed-width Bochs/adapter
records.

## v1 path

`#UD event + snapshot + 15-byte observation` → exact catalogue lookup →
source-derived plane prepare → one pending transaction → one generic Bochs
ordinary-RAM commit → resume.

At this revision only historical DEM `0x50/0x11` is admitted. It prepares the
existing `demLoadDos` contract: copy the identity-checked NTDOS payload to
real-mode `DI:0000`, then advance over the three marker bytes and one service
byte. The session does not perform the write and has no Bochs memory access.

## Atomicity and failure

Before a commit owner can consume a pending item, the transaction is
preflighted again against the declared memory aperture and payload length. A
second consume fails. An unknown selector/service, a non-#UD event, malformed
records, an unavailable session, or an unimplemented service produces only
pass-through; it neither writes memory nor advances guest state.

The pending payload reference is an internal hand-off to the future generic
Bochs commit seam. It has no guest address translation, device access, DOS
filesystem behavior, or host I/O.
