# M0 T275 S1 P2 historical interface ledger

## Source-first result

The ledger records eleven original-caller interface surfaces rather than using
the old adapter filenames as an architecture. Every row names an original
caller/declaration, historical dependency, observable contract, old candidate
scope, recovery disposition, failure boundary and one successor subtask.

## Decisions

- BOP transport is the sole S2 recovery surface. It remains selector-blind and
  consists only of copied frame ingress, checked transfer and typed completion.
- The only generic SoftPC S3 surface is synchronous CCPU-register/SAS-span
  access through the session mapping lease. Bochs execution itself is not a
  SoftPC facade: the old machine shell is retired and `adapter-bochs` remains
  the only caller of `bochs-core`.
- XMS A20/UMB and device-specific callbacks remain source-owned later package
  work. They cannot justify generic adapter policy.
- The eight project-only trace/observation candidates are retired from
  production adapter recovery; retained historical evidence belongs in the
  documentation archive or a separately admitted diagnostic facility.
- Win32 recovery is limited to named original callers. Public Win32 can replace
  historical product exports only while preserving name/layout/order/failure;
  it cannot silently recreate `NtVdmControl`, CSR or BaseSrv.
- VDM monitor is a complete same-shaped disposition family, not an ad hoc
  `NtVdmControl` stub. Broker is explicitly project-owned public IPC, and may
  carry only copied fixed-width values.

## Limitation

This P2 freezes family scope and owner decisions. P3 still maps every one of
the 61 implementation candidates to one of these ledger IDs (recover, split,
defer, retire) and independently verifies declaration candidates needed by a
retained source surface. No adapter source becomes production code in P2.
