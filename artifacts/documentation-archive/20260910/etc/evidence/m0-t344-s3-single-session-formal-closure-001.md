# M0 T344 S3 — Single-session reentrancy formal closure

## Result

T344 closes with an explicit **one active SoftPC machine/session per process**
profile. This is an evidence-backed boundary, not a claim that the original
NT4 product lacked all multi-VDM facilities.

The source-owner ledger distinguishes project state that is already scoped
per session or per caller thread from original state that remains one machine
image:

- `session` owns three mapping-manager instances and a TLS caller binding;
  `base_vdm_local` owns one copied BaseVDM record, lock and wake event per
  binding.
- Original CCPU40 still owns one process TLS allocation id, `potentialNewThread`
  and `tidlist`, with only `ThreadSimBuf` itself being per thread.
- Original SAS/M-area dispatch, video controller/latch state, ICA/event/timer
  state and host lifecycle state are mutable machine-wide carriers.
- `CurrentMonitorTeb` is an original process-global monitor/WOW carrier. It
  cannot become a session identifier merely by changing its storage class.

No complete original machine-instance lifecycle exists in the selected source
cohort. Copying these globals into a product scheduler would be a new machine
implementation, so S2 is intentionally not admitted.

## Formal verification

The unchanged formal CPU40 candidate graphs were invoked after the inventory:

- `ninja -C build/M0-T343/S3/record-x86 original-softpc-candidate`
- `ninja -C build/M0-T343/S3/record-x64 original-softpc-candidate`

Both reported `no work to do`. This verifies that the audit made no source or
build-closure change; it does not claim a second simultaneous SoftPC instance.

## Explicit transfers

- A future in-process multi-machine capability requires a newly admitted,
  whole original-machine lifecycle cohort covering CCPU, SAS, video,
  controller/timer, monitor and host endpoint ownership together.
- CSRSS/BaseSrv transport, kernel VDM, shared/separate WOW execution and
  cross-process coordination remain outside this package.
- The existing session and broker records remain bounded source-shaped
  compatibility bindings; they are not evidence that the original machine
  state is reentrant.
