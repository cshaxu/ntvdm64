# M0 T310 S24 P1 — host-communications source boundary

## Question

Which complete original host-communications forms can remain directly composed
under the selected Win32/x86 CCPU40 profile, and which forms are owned by the
separate WOW, printer/MONITOR or later integration packages?

## Inputs and procedure

Read `softpc.new/host/src/nt_com.c`, `nt_wcom.c`, `nt_lpt.c` and the reached
`nt_ntfun.c` fast communications helpers together with `host_com.h`, the
original `nt_bop.c` WOW dynamic-import sequence, S23 controller callers and
the selected formal build manifest.  Classified every reached host API,
thread/event/handle state and cross-owner callback by original caller, state
owner, public availability and selected preprocessor condition.

## Observations

### Serial endpoint: direct original host algorithm

- `nt_com.c` owns the original serial endpoint lifecycle.  `host_com_open`
  validates the adapter, reads original configuration, opens the configured
  endpoint with `CreateFile`, captures/restores its DCB, creates its original
  events and RX worker, synchronizes line settings, and on every failure
  closes/reset-retries through the original `host_com_close` order.
- Its live endpoint operations use public Win32 serial/device APIs
  (`CreateFile`, `CreateEvent`, `CreateThread`, `ReadFile`, `WriteFile`,
  `GetOverlappedResult`, `SetCommState`, `SetCommTimeouts`,
  `EscapeCommFunction`, `ClearCommError`, waits and `CloseHandle`).  These
  retain the historical handle/event order without a substitute serial
  provider.
- The reached fast forms in `nt_ntfun.c` preserve `FastSetCommMask`,
  `FastGetCommModemStatus` and `FastWaitCommsOrCpuEvent`.  They use the
  original `NtDeviceIoControlFile` / `NtWaitFor*` calling shape for serial
  IOCTLs.  The selected Win32/x86 ABI is pointer-width matched; these are
  direct original host calls, not guest-pointer publication.
- Every `HANDLE`, event, `OVERLAPPED`, `DCB`, RX buffer and thread record
  stays in the private original `HOST_COM` state.  S24 does not publish it to
  guest state or another component.  Therefore no mapping-manager identity is
  required by the selected serial path.  A future fixed-width publication
  would use the session host-resource mapping-manager instance, not a new
  map.
- The only relevant mirror divergence is already registered
  `MVDM-HOST-DIV-068`: `host_com_ioctl` has native-width private transport.
  It retains source request values and output-pointer ordering; it is neither
  a device replacement nor a guest ABI change.

### WOW and printer boundaries

- `nt_wcom.c` is the original WOW direct-UART companion.  Its `GetCommHandle`
  and `GetCommShadowMSR` function pointers are assigned only by the original
  `nt_bop.c` dynamic lookup from the historical WOW DLL.  When the pointers
  are absent it falls back to the ordinary `com_inb`/`com_outb` controller
  path.  The WOW DLL/provider and its exported handle accessors belong to the
  later WOW owner package; S24 must not invent a standalone callback provider.
- `nt_lpt.c` is the original LPT/printer host provider, but its whole runtime
  body is selected only by `PRINTER`.  Its direct-access paths additionally
  call MONITOR forms.  The selected CCPU40 profile defines neither `PRINTER`
  nor `MONITOR`; thus it remains an exact condition-unselected unavailable
  path, not a candidate for synthetic printer status or monitor recovery.

## P1 disposition

| ID | Original contract | Disposition | Next owner |
| --- | --- | --- | --- |
| S24-001 | `nt_com.c` serial endpoint state, RX worker and lifecycle | direct original body with public Win32 and existing OpenNT-shaped NT calls | S24 P2 formal assembly / S49 integration |
| S24-002 | `nt_ntfun.c` Fast serial IOCTL/wait helpers | direct original same-process helper body | S24 P2 formal assembly |
| S24-003 | `nt_wcom.c` WOW direct-UART callback forms | binding-only; no callback provider until original WOW package is recovered | S39 |
| S24-004 | `nt_lpt.c` LPT/printer endpoint and MONITOR direct access | exact `PRINTER`/`MONITOR`-unselected unavailable disposition | S24 P2; future profile only by admission |

## Interpretation, confidence and follow-up

The serial endpoint has a finite original public-API closure and needs no new
adapter.  Its actual device open is deliberately not exercised during this
source-boundary pass because it can alter a real host serial device's DCB and
line state.  S24 P2 must verify fresh formal x86 CCPU40 archive/forced-link
ownership and use only a safe public invalid-device negative operation.  It
must record that this proves failure direction and API availability, not live
serial I/O.  WOW and printer integration remain explicitly outside S24.
