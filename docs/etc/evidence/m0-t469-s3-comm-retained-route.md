# M0 T469 S3 — COMM retained-route acceptance

## Question

Does the current source-identical COMM provider retain both its original
success-side lifecycle and its source-defined unavailable-host behavior,
without introducing a project serial provider or changing guest media?

## Inputs and procedure

- `Build-T420S30CommsGuestTest.ps1` rebuilt the checked-in `C30.COM` and
  `C31.COM` fixtures under `build/t469-s3/fixtures`.  Their SHA-256 values
  are respectively `5bc3f89161c1c7542859e6c5b51e15a091f0cc0a6fd61f96667756bca49fa67e`
  and `2629dc806130fa4de7b83588497219a03dce608a3aa54b65b1b671b6f209ee49`;
  they match the installed immutable test inputs.
- The checked-in `Verify-T420S30ComMock.ps1` compiled and ran the current,
  unchanged `softpc.new/host/src/nt_com.c` in its test-only CTS-capable
  boundary.  It emitted `T420_S30_NT_COM_MOCK_OPEN_WRITE_READ_CLOSE_OK` from
  `build/t469-s3/nt-com-mock-r2/nt-com-mock-fixture.log`.
- A noninteractive ConPTY run submitted `--wait O:/winnt/tests/C31.COM`
  to the current installed `run16.exe`.  It returned guest status 1 before
  the loopback marker.  The initial run without `--wait` is excluded because
  the observer could close after launcher return; it is retained only as an
  invalid invocation record.
- The S1/S2 source audit and the existing
  [external COM boundary](m0-t420-s30-external-com-boundary.md) were
  rechecked.  The sole real host COM3 remains a USB serial endpoint without
  a CTS/DSR-capable peer.

## Observation and interpretation

The direct provider fixture proves the original open, write, receive, read
and close sequence when the host contract supplies CTS and one received byte.
It is deliberately not a runtime serial provider and is not linked into any
product image.

The current installed C31 run is not a negative result against that provider:
the one-shot launch has no recorded COM3-enabling PIF/profile, and its status-1
result before the marker cannot establish that the guest reached the original
controller-local loopback contract.
Creating a PIF profile or a virtual serial endpoint solely to turn that into a
pass would add an unadmitted configuration/provider policy.  Separately, a
real external transmit still lacks the physical CTS-capable peer documented
by the retained boundary record.  Both are explicit environment non-passes,
not evidence of a project implementation defect.

The prior source conclusion is unchanged: `FreeCommSupportResources` only
releases the first same-task `PortTab` entry, and original close retains the
1.5-second modem and 5-second writer waits.  No matching correction exists in
the inspected original trees, and no autonomous cleanup rewrite is admitted.

## Result and handoff

S3 adds only a test-fixture linkage repair: the direct-source fixture now
supplies an inert implementation of the production-only setup diagnostic that
the current original provider references.  No production source, runtime
image, release asset or guest input changed.  S4 owns final package/hash and
governance reconciliation, with the two serial-environment limitations kept
as non-passes.
