# T484 S4 device test input preparation

P9 extends the existing CMD physical-stage owner with repeatable TestFile input.
Each input must be an existing file below this repository build tree. The stage
copies its bytes into tests before completing the ownership marker. Sources,
product images and original guest files remain unchanged. Failed preparation
uses the same owned-directory cleanup as P6. The PowerShell facade only passes
arguments. There is no separate fixture copy framework or second runtime owner.

PIC now hashes its canonical authored COM output and passes that file directly
to CMD staging, removing the redundant evidence copy and interpreted runtime
copy. Serial discovery likewise passes its canonical COM to CMD staging. Its
native host oracle still queries the current COM1-COM9 namespace; the runner
validates the numeric mask and supplies it to the existing CMD compiler entry.
That observation is not a reimplementation of the oracle or a script generator.
All device assertions, timeouts and cleanup remain unchanged.

## Verification

Evidence: build/t484-s4-device-inputs.

- PID18144,2026-11-24T16:44:27Z-16:46:34Z: cached full build and expanded package
  contracts pass. Tests include byte-identical independent input staging and
  rejection of missing files, directories and outside-build inputs, together
  with all previous identity/copy/failure contracts. The serial direct and
  double-COMMAND routes pass; actual host mask is0. No positive physical-port
  claim is made. This process returns1 because its PIC invocation supplied an
  absolute OutputRoot to a runner requiring a repository-relative path. That
  rejection occurred before native/guest assertions and is retained as failure.
- Corrected PIC PID14820,16:47:03Z-16:47:29Z,exit0: unchanged final source,
  corrected caller argument. The fixture checks real production map providers
  and passes24 assertions; actual DOS IRQ0 control and reinitialization both
  report success and normal exit. No product workaround was introduced.
- publication.json verifies11 raw host inputs unchanged and22 matching hashes
  across assets/release and O:/winnt/system32. No changed host image required
  copying. Tests run on private desktops in the complete native Userenv block.
  No new Full runtime pass, original guest rebuild or winnt.zip change.

## Accounting

The PIC and serial frozen entries become migrated-delivered. Census334/355
disposed,21 pending:20 producer entries and1 reconciliation entry. The24
supplemental preparation chains overlap this census and remain separately
pending. S4 continues; S5 clean interpreter-free acceptance is not complete.
