# M0 T470 S3 — WOW debugger producer and retirement disposition

## Original control graph

The selected original graph has one debugger owner and no NTSRV replacement:

1. `wow16/kernel31/wow16cal.asm::WOWNotifyTHHOOK` sends `DBG_TOOLHELP` with
   the original `THHOOK` address and 386 flag, then calls `WOWNotifyWOW32` to
   publish the original WOW kernel/task addresses.
2. `wow32/wkman.c::WK32WOWNotifyWOW32` stores those original guest addresses
   and invokes `IsDebuggerAttached`.  `W32Thread` calls `DBGNotifyNewTask`
   only while the original debugger is attached.
3. `mvdm/dbg/dbg.c::DBGDispatch` consumes `DBG_TOOLHELP` by recording its
   THHOOK payload.  It does not publish a synthetic `STATUS_VDM_EVENT` for
   that initialization notification.  `DBGNotifyNewTask` uses the retained
   public `STATUS_VDM_EVENT` route for `DBG_TASKSTART`.
4. The guest's original module/task paths issue their corresponding debug
   notices; `W32DestroyTask` completes original cleanup, then the already
   reviewed `wow_task_termination_retire` removes only the project management
   binding.  The latter neither selects nor changes the original exit path.

`W32RemoteThread`/`WK32KillRemoteTask` are an original debugger-only
ToolHelp-callback transport.  Their saved context and dedicated TD are not an
application task, an NTSRV request, or a general callback mechanism.  Reusing
them would create a second lifecycle/control plane and is rejected.

## Evidence and result

The source graph is unchanged (`git diff` is empty for `dbg.c`, `wkman.c` and
the ToolHelp thunk table).  The established MSVC-initialized cached Ninja
route rebuilt `debugger-event-tests`; both retained native checks passed:

* `S43_DEBUGGER_EVENT_TRANSPORT_OK` proves handled and original SEH fallback
  continuation behavior;
* `S43_DEBUGGER_MODULE_EVENT_TRANSPORT_OK` proves the selected module event
  layout and continuation behavior.

The historical real-worker observer was also reconstructed.  Its old command
expects a flat root containing both `run16.exe` and `MEM.EXE`; the former
`O:\winnt` root no longer has either.  Pointing it at the current
`O:\winnt\system32` obtains the files but `CreateProcess(..., DEBUG_PROCESS)`
returns Win32 error 50 before a worker begins.  This is an obsolete observer
placement/launch contract, not evidence that a WOW debug producer ran or
failed.  It is not used as a product pass or failure.

No safe new runtime test can turn the current immutable ToolHelp/KERNEL query
non-pass into a WOW debug notification pass without changing guest code or
adding a synthetic debugger/provider.  The direct original control graph and
the already accepted non-WOW event transport therefore remain the finite
disposition.  No production source, guest media, ABI, release asset or
installed binary changed.
