# M0 T364 — COMMAND command delivery and built-in dispatch closure

T364 closes the owner package transferred by the T363 later `54:01`.

- S1 followed original `CmdDispatch`/`cmdGetNextCmd`, `CMDINFO`/`VDMINFO`, the
  imported BaseClient/BaseSrv source, app declaration and the local transport.
- S2 proved the selected one-session copied-record lifecycle with the formal
  x86 fixture: first delivery, consumption, pending/wake/retry, explicit
  no-command, DOS/WOW selection and teardown.
- S3 proved original COMMAND, BaseClient-shaped binding, session and broker
  are final CPU40/x86 process inputs.
- S4 made the one frozen run.  The initial original `54:01` returned; after
  the `/C EXIT` bootstrap record was consumed, a later ordinary `54:01`
  entered the source-defined no-next-command wait and timed out under the
  observer.

No command parser, virtual command source, BOP result, guest/media mutation,
CPU/device change, CPU30, Bochs or CSRSS recreation was introduced.  T364
does not claim interactive COMMAND, a DOS child, COM/MZ EXEC, native child,
WOW or a completed user command session.  The next queue owner is original
DOS `.COM`/MZ `.EXE` execution and parent return.
