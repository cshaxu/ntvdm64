# T420 S30 external COM boundary

## Question

Can the current Windows host provide a real peer that completes the selected
original DOS `INT 14h` external transmit and host-driven receive paths without
adding a product serial provider or changing their historical flow-control
semantics?

## Inputs and procedure

- The selected callers are original `rs232_io.c` (`INT 14h` send/receive),
  `com.c` (8250 controller) and original host `nt_com.c` (`CreateFile`,
  overlapped `ReadFile`/`WriteFile`).  They have zero S30 mirror, overlay or
  adapter implementation changes.
- The serial device map was read from
  `HKLM\HARDWARE\DEVICEMAP\SERIALCOMM`; PnP Ports enumeration and installed
  port-class providers were inspected without changing drivers.
- The available COM device was opened natively without writing, then its CTS,
  DSR and receive state were queried.
- Real DOS fixtures ran against the formal x86 package: `C30.COM` proves the
  original COM3 open; `C31.COM` proves original 8250 TX/RX loopback.  Raw
  transcripts are `O:\winnt\logs\m0-t420-s30-com3-open-r1.raw` and
  `O:\winnt\logs\m0-t420-s30-com3-loopback-r2.raw`.

## Observation

The map contains only `\Device\USBSER000 = COM3`; PnP reports one started
`usbser.inf` USB Serial Device (COM3).  Native open succeeds, but reports
`CTS=False`, `DSR=False` and no received bytes.  `C30.COM` opens COM3 through
unchanged `nt_com.c`; its INT 14h transmit takes the original timeout branch.
`rs232_io.c` deliberately waits for CTS before sending its byte, so it does
not call the host write path when CTS remains low.

`C31.COM` separately completes original controller-local `5A` TX/RX loopback.
This proves controller operation, but its MCR loopback mode intentionally
bypasses the external host write/read path and therefore is not external-peer
evidence.

The test-only `nt_com_mock_fixture.c` directly compiles the unchanged selected
`nt_com.c` body while replacing only its Win32 serial-device calls with a
controlled CTS-capable medium contract.  Its verified route is
`host_com_open → host_com_write → GetCharsFromDriver → host_com_read →
host_com_close`; it emits
`T420_S30_NT_COM_MOCK_OPEN_WRITE_READ_CLOSE_OK`.  The source and runner are
`tests/mvdm-host/nt_com_mock_fixture.c` and
`tools/audit/Verify-T420S30ComMock.ps1`; the r1 transcript is under the
disposable build root.  This proves the original provider's success-side
lifecycle without adding a product provider, driver or hardware simulation.

## Interpretation and follow-up

The missing prerequisite is a CTS-capable serial peer, not an implementation
gap: the original provider, normal no-medium failure behavior and controller
state are present and unchanged.  No fake serial provider, virtual device or
flow-control relaxation was introduced.

Future acceptance requires a physical paired/null-modem or loopback endpoint
that asserts CTS and returns a known byte, or an explicitly admitted virtual
serial-pair provider.  The retest must prove real DOS `INT 14h` transmit,
host-driven receive, representative failure and worker teardown; it may not
replace those with controller-only loopback or this test-only mock.
