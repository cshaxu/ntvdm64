# T420 retained prior status records

Moved verbatim from Current during the S42 handoff to keep the status control
within its size limit. These are historical observations; their linked later
corrections and the current proposal govern acceptance and receiver assignments.

## S30 Closure Context

The S37 comparison supersedes pure-profile acceptance below: the old marker
proved no DOSX, not the intended CONFIG. See the corrected
[S30 evidence](m0-t420-s30-dos-profile-host-medium-closure.md#post-closure-pure-profile-evidence-correction).
Current low-DOS failure remains the accepted original guest debt, not a pass.

The owner-expanded S30 scope is delivered at `91a5be61a`: task inputs enabled
`EMM=RAM`, and a real DOS guest completed INT 67 allocation, map/remap,
unmap and free on the formal product. The complete 17-route product matrix
also passed. The closure record below is the current disposition; it was not
withdrawn.

S30 now audits the original COM/LPT configuration and host providers against
actual Windows endpoints, then adds the default DOS, HIMEM/DOSX and explicit
EMS profile matrix. No S31/debug work is admitted.

The first real matrix results are: default startup reaches both public XMS
and DPMI queries (`S30_HIMEM_DOSX_OK`); the task PIF override reaches XMS
without DOSX and completes original EMS map/remap/unmap/free
(`S30_HIMEM_ONLY_OK`, `S30_EMS_*`); a task PIF with no DOSX also completes
normally (`S30_PURE_DOS_OK`); and guest COM3 opens the discovered Windows USB
serial endpoint through unchanged original `nt_com.c` (`S30_COM3_OPEN_OK`).
The first pure-DOS probe incorrectly set the original PIF editor's default
EMS reservation to zero, shrinking the worker address-space allocation and
causing the initial stall. Its subsequent assumption that XMS absence proves
HIMEM absence was also false: selected worker XMS handling is a process
baseline. The corrected profile proves the actual PIF-owned distinction,
which is absence of DOSX/DPMI.

The original UART controller itself is real-guest proven: after the COM3
host open, `C31.COM` writes the original COM3 8250 MCR loopback bit and
round-trips `5A` through direct TX/RX port I/O
(`S30_COM3_LOOPBACK_TX_RX_OK`). This verifies selected SoftPC serial TX/RX
without transmitting to an unknown external protocol.

The discovered `COM3` is a Windows `usbser` USB-C Billboard endpoint. Its
real-open probe passes, but original INT 14h transmit correctly times out:
the unchanged `rs232_io.c` waits for UART CTS before it calls the original
host write path, and this endpoint does not assert CTS. This is a real
medium capability condition, not a missing NTVDM provider or a reason to
weaken the original CTS rule. Complete COM transmit/receive acceptance needs
a CTS-capable loopback or paired serial endpoint. The host has no persistent
physical/network LPT endpoint, but the original LPT `CreateFile("LPT1")` path
is separately verified below through a temporary Windows device mapping that
is removed after the test.

The host inventory confirms that this is not an unsearched Windows facility:
`HKLM\\HARDWARE\\DEVICEMAP\\SERIALCOMM` contains only
`\\Device\\USBSER000 = COM3`; PnP reports only the started `usbser.inf`
USB Serial Device (COM3). A native non-writing open reports `cts=False`,
`dsr=False` and no received bytes. Thus no installed physical or virtual
paired COM medium can complete the original CTS-gated external transmit or
host-driven receive acceptance.

The deferred prerequisite is registered in the [debt ledger](../../states/TODO.md) with
its original caller/provider, zero retained product diff and the exact retest
condition.  The supporting [external COM boundary record](m0-t420-s30-external-com-boundary.md)
retains the full current-host audit; the unavailable medium is not a pass.

To avoid leaving the provider's success route untested merely because this
host lacks a peer, a test-only direct-source mock now compiles unchanged
`nt_com.c` and supplies a controlled CTS-capable Win32 serial contract. Its
`open → write → receive queue → read → close` lifecycle passes with
`T420_S30_NT_COM_MOCK_OPEN_WRITE_READ_CLOSE_OK`. This fixture is not linked
into the product and does not close the real external-medium debt.

The original LPT write path is now independently end-to-end proven without a
product substitute. The test observer temporarily mapped the Windows `LPT1`
DOS-device name to a private named pipe, then removed that mapping after the
run. `L30.COM` invoked INT 17h byte output; unchanged `nt_lpt.c` opened
`LPT1`, buffered it, and flushed during worker close. The pipe received `5A`
and the guest reported `S30_LPT1_WRITE_OK`; no mapping, pipe, or product
process remained afterward.

An independent post-closure `C31.COM` ConPTY rerun also completed normally:
the launcher returned `exit=0`, emitted `S30_COM3_LOOPBACK_TX_RX_OK`, and
had no guest command-resolution failure. The earlier aggregate-run timeout
is therefore not a reproducible worker-exit defect.

The owner's clarification applies to the entire T420 capability sequence:
every capability that the selected standalone NTVDM product itself owns must
be verified through its original guest-to-host path before its owning S can
close. A missing external medium is recorded as a platform limitation, never
silently converted into a pass; the remaining S31--S43 rows retain the same
per-package acceptance rule.

## S30 Reopened Closure

The owner returned to S30 and its reopened matrix passed again: DOSX/HIMEM,
pure-DOS, EMS, COM3 controller
loopback, LPT1 byte output and the isolated original `nt_com.c` success mock.
The Windows serial map remains only `\Device\USBSER000 = COM3`, so the
recorded absence of a CTS-capable external peer is unchanged and may not be
turned into a product-side substitute. See the S30 closure evidence's
2026-09-18 reopened-verification record.
