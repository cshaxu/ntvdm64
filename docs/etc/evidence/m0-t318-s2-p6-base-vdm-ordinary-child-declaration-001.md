# M0 T318 S2 P6 — Base VDM ordinary-child declaration

## Question

Can the product declare the admitted one ordinary DOS child through the
existing Base VDM source-shaped adapter, without making app a DOS loader,
command broker, or guest lifecycle owner?

## Source route

1. `app/entry.c` activates the one session, initializes the existing
   `base_vdm_local` record, then binds it before the original SoftPC entry.
2. The original `nt_msscs.c::scs_init` calls `GetNextVDMCommand(NULL)`.
   Existing `base_vdm_local_is_first` preserves the selected one-session
   query-and-clear result, so original `AddSystemFiles` retains its owner and
   order.
3. The explicit app option `--ordinary-child` is removed before original
   `ntvdm.c` receives its command line. App derives only a copied record from
   the already session-selected `dos` media root:
   `COMMAND.COM /C VER`, its same image application path, `COMSPEC`/`PATH`
   MULTI_SZ and current directory.
4. Existing `base_vdm_local_publish` owns copies, sizes, lock/event, retries,
   `VDMINFO` result layout and later re-entry. Original
   `dos/command/cmdmisc.c::cmdGetNextCmd` remains the consumer that asks for
   the DOS-binary payload and transfers its fields to guest COMMAND state.

## Verification

- The selected `CPU_40_STYLE` formal product links in both x86 and x64 after
  adding `app/launch_declaration.c`; no second command dispatcher or guest
  loader is linked.
- The two formal graphs are then incrementally clean (`ninja: no work to do`).
- A fresh x86 `runtime-r9` run with original `-f -o --ordinary-child` remains
  alive until a five-second owned-process watchdog. This proves only that the
  admitted declaration did not immediately reject the original process entry.
- The prior missing-`NTIO.SYS` counterfactual remains indistinguishable at this
  external watchdog boundary. It therefore cannot prove that `NTIO`, NTDOS,
  COMMAND, EXEC, or parent restoration was reached.

## Disposition

The Base VDM **declaration and binding** are source/build closed. The child is
not runtime-complete: a non-invasive observation boundary is still required to
distinguish the original media-load error path from subsequent guest execution.
No BOP, DOS service, child executor, parent restore, or host COMMAND provider
was authored or replaced.
