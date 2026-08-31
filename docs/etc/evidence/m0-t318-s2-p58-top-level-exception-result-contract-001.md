# M0 T318 S2 P58 — Original top-level exception-result contract

## Question

Does the fixed-container `0xC0000005` describe a synthetic app/observer
outcome, or can the original SoftPC top-level exception path preserve it?

## Original path

The selected original `obj.vdm/ntvdm.c` places only `CpuEnvInit`, `nls_init`,
and `host_main` inside its `try/except` body.  Its `except` expression is the
original `nt_timer.c::VdmUnhandledExceptionFilter`.

That filter:

1. enters the original heartbeat suspend critical section;
2. calls public `UnhandledExceptionFilter` with the original exception record;
3. if Windows selects `EXCEPTION_EXECUTE_HANDLER`, calls original
   `NtTerminateProcess(NtCurrentProcess(),
   ExceptionInfo->ExceptionRecord->ExceptionCode)`.

It does not translate an access violation to a `DisplayErrorTerm`, `120`,
BaseVDM result, BOP result, or app-defined exit code.  `TimerInit` initializes
the critical sections before this top-level `try` is entered.

## Result

The p60 exit code is therefore consistent with an original primary access
violation escaping a selected host/CPU branch.  It does not locate that branch,
but it rules out interpreting `0xC0000005` as a synthetic command-declaration
or adapter terminal result.  The next audit must examine whole original owner
contracts that can execute inside `host_main`; no BOP implementation or guest
trace may be selected from this code alone.
