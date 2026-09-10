# M0 T366 S1 — Declared DOS workload record owner ledger

## Question

Why did T365's fixed `/C EXIT` container reach an original later `54:01`
no-next-command wait before `$Exec`, and what original owner package must be
recovered before another DOS workload observation?

## Inputs

- T365 S4 frozen observation and its unchanged `/C EXIT` declaration.
- Original `opennt-host/base/win32/client/vdm.c::GetNextVDMCommand`.
- Original `opennt-host/base/win32/server/srvvdm.c::BaseSrvGetNextVDMCommand`.
- Original `mvdm-host/dos/command/cmdmisc.c::cmdGetNextCmd`.
- Current `app/launch_declaration.c`,
  `adapter-mvdm-host-out/basesrv/source/base_vdm_broker.c`, and
  `adapter-mvdm-host-out/basesrv/source/base_vdm_local.c`.

## Observations

1. Original COMMAND requests a copied `VDMINFO` through
   `GetNextVDMCommand`; it does not parse a new CLI command language.
2. Original BaseSrv distinguishes a DOS record from a WOW request, copies the
   record's command/application/environment/current-directory fields, and
   returns a wait/no-next-command form only after its DOS record search.
3. Current app already has the right product-level boundary: `--command X`
   stores one immutable text input, then emits `COMMAND.COM` as application
   and `/C X\r\n\0` as the copied command record. It does not load guest
   bytes or parse DOS syntax.
4. `base_vdm_broker_publish` correctly accepts only
   `BASE_VDM_COMMAND_DOS`. Its `base_vdm_broker_deliver` reconstructs that
   owner before calling the same-shaped local BaseSrv record.
5. `app_launch_declaration_publish` initializes every other selected record
   field but omits `command.command_owner`; zero initialization therefore
   makes the existing broker reject the record before it can reach the local
   BaseClient/BaseSrv path.

## Source-first disposition

- **Original source reuse:** retain original COMMAND `VDMINFO` consumption,
  BaseClient capture/copy ordering and BaseSrv DOS-versus-WOW distinction.
- **Smallest same-shaped binding:** retain the existing copied BaseVDM broker;
  set the existing DOS record discriminant at its sole app composition point.
  This is one field of an already defined record, not a new provider.
- **External intrusion:** none.
- **New behavior:** none. CLI target selection remains the admitted app input;
  no DOS parser, loader, guest pointer or synthetic command is introduced.

## Selected S2 cohort

`app/launch_declaration.c` record construction, the existing BaseVDM broker,
and the existing local record fixture form one complete recovery cohort. S2
must set the DOS owner field, test a declared `.COM` text record through
publish/deliver/consume, retain malformed/oversize rejection, and formally
link the affected x86 product. It must not execute a guest workload; that is
S4 after S3 closure.

## Confidence and follow-up

High confidence: the omission and rejection predicate are adjacent, explicit
source facts. T365's `/C EXIT` sidecar remains runtime evidence only and is
not used to infer a BOP or `$Exec` repair. S2 owns the one source-shaped
record-field correction.
