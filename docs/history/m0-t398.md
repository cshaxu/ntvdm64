# m0-t398 historical record

Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.

## m0-t398-errorpanel-copied-request-reply-closure-20260909.md

# M0 T398 closure — ERRORPANEL copied request/reply boundary

## Outcome

T398 closes the approved, finite original SoftPC `ERRORPANEL` boundary. It
does **not** claim a completed Win16 application lifecycle, an organically
reached host-error workload, or any guest-dialog migration.

The only product and acceptance executable is Win32/x86 `ntvdm32.exe`.
Native x64 is not built, linked, staged, run, accepted, or repaired by this
closure except for a demonstrated architecture-neutral mapping-manager defect.

## Recovered boundary

The original `softpc.new/host/src/nt_error.c:ErrorDialogBoxThread` retains its
source-defined request and termination ownership.  It now first offers the
finite `ERRORPANEL` request to the `softpc` adapter as copied data; the app
owns one public-Win32 dialog and returns only the selected source `RMB_*`
reply.  It neither retains the original HWND/message/edit pointer nor owns
termination.

The retained source details are:

- `RMB_ABORT`, `RMB_RETRY`, `RMB_IGNORE`, `RMB_EDIT` and all four source icon
  choices;
- the source high-word edit capacity and its edit-request predicate;
- source empty-edit copyback for Abort/Retry/Ignore, rather than fabricated
  text; and
- the original post-reply WOW notification and caller/session termination
  path.

If the copied presenter is unavailable or rejects a request, the original
`ERRORPANEL` resource and `ErrorDialogEvents` fallback remain selected.
`RcMessageBox`, guest application dialogs, app preflight and private
`WOWpSysErrorBox`/CSRSS families remain outside T398.

## Evidence

The [S2 evidence](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t398-s2-errorpanel-copied-request-reply-001.md)
records three x86 focused passes: copied adapter contract, actual app-dialog
`WM_CLOSE → RMB_IGNORE`, and a source-layout `ERRORDIALOGINFO` call through
the original `ErrorDialogBoxThread`. The latter returns `RMB_IGNORE` after
the original worker trims its title, proving the selected request/reply
carrier rather than a synthetic adapter caller.

The complete x86 product graph completes all 451 commands without unresolved
external. A diagnostic-free Console-owner run loads WOW32 for `WOWEXEC`; the
ordinary `WRITE.EXE` run retains its original visible low-memory guest dialog.
That is the required ownership non-regression. The fixture's test-only
translation-unit isolation is explicitly not a substitute for the normal
product link.

## Explicit limits and successor

No bounded ordinary workload constructed an `ERRORPANEL` during observation.
T398 does not manufacture a guest failure just to obtain one. Any future
host-error reachability investigation requires new packet admission and must
retain the separation between host `ERRORPANEL`, guest dialogs, `RcMessageBox`
and private WOW hard-error families.

With T398 closed, no numeric M/T/S packet is active. The next work must be
selected from the ordered unnumbered candidate queue and admitted before any
implementation.

