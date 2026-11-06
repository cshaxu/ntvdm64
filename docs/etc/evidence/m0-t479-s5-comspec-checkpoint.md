# T479 S5 shared COMSPEC checkpoint

This checkpoint implements the approved host-interpreter correction and the
first shared command-preparation extraction. It does not close S5 or complete
the remaining execution/request consolidation.

## Source and implementation

Original COMMAND SVC_EXECCOMSPEC32 (54:0A) selects the native host COMSPEC.
Original cmdXformEnvironment deliberately excludes the guest COMSPEC. Hosting
the frontend in guest Windows does not change this interpreter contract.
The existing Run16 binding incorrectly selected COMMAND.COM by a hardcoded
`command` argument; it now forwards the captured host executable, quoted for
spaces. Both ordinary and VM paths use this binding.

COMSPEC lookup, length validation and `/c` composition now have one helper in
the existing NTVDM command component. Original live-CPU wrappers and saved-VM
capture call it. The helper also rejects the exact-full-buffer case before
the terminating NUL can overrun it. Original error classification is retained.
Mirror exception MVDM-HOST-DIV-196 covers this bounded extraction; its local
cmdexec delta is 13 additions and 22 deletions. No original guest is changed.

The [source audit](../operations/m0-t479-s5-cmdexec-reuse-audit.md) records
original owners, rejected singleton reuse and the remaining consolidation.
Ordinary COMMAND scheduling and VMM wait/wake are separate responsibilities.

## Evidence

[Hash-bound evidence](m0-t479-s5-comspec-checkpoint.json) retains source,
build and actual Win3.1 VM results.

- Command capture: 107 assertions; launch: 65 assertions, including captured
  interpreter paths with spaces and valid/rejected capacity boundaries.
- Run192: actual host CMD and MEM output in both Win3.1 VMs before EXIT;
  both 54:08 forms and 54:0A; 320 callback identity/stack/register/flags checks.
  Both VM continuations succeed and guest Windows exits normally.
- Failed runs180/182 are retained. The fixture embedded a repository PATH
  without a DOS-compatible alias. Root controls reproduce the same failure
  without guest Windows. A same-hash MEM copy in the short fixture path fixes
  the witness; no product command-name or arbitrary-path workaround is added.
- Cached complete x86/x64 build193 and shared-library suites pass.
- Release194 passes all 16 selected Full product gates. The external
  DOS4GW/Nesticle fixture is explicitly skipped, not counted as a pass.
- Package `2915fea5-c2ec-4242-8f6a-fbe152c8c4f2` is published coherently at
  `assets/release` and `O:/winnt/system32`; all ten images and the hook license
  match the verified package. The diagnostic worker is not a release input.
  The published independent addon remains unchanged.

## Retained boundaries

Run192 uses translated provider input, not physical keyboard acceptance. Its
DOS descendant follows the existing host worker chain; it does not prove
CF=1 same-guest reentry. Shared execution preparation and reentry ownership
remain S5 work. Complete I/O, fonts and physical input remain S6. Win95 work
is deferred until Win3.1 acceptance. No full multitasking completion is claimed.
