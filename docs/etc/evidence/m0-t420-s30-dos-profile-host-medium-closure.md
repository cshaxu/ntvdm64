# T420 S30 DOS profile and host-medium closure

## Scope and invariant

S30 closes the selected original DOS/EMM profile work and the reachable
original COM/LPT host-medium work.  It adds no product provider, serial
emulation, printer emulation, mirror source change, overlay, or adapter
algorithm.  The selected original owners remain `rs232_io.c`, `com.c`,
`printer*.c`, `nt_com.c`, and `nt_lpt.c`.

## Profile and medium matrix

| Route | Result | Witness |
| --- | --- | --- |
| Default startup: HIMEM plus DOSX/DPMI | Real guest pass | `D30.COM` emits `S30_HIMEM_DOSX_OK`. |
| PIF pure-DOS selection | Real guest pass | `P30.COM` emits `S30_PURE_DOS_OK`; the distinguishing profile property is no DOSX/DPMI, not absence of the worker's baseline XMS handling. |
| PIF EMS selection | Real guest pass | `E30.COM` emits `S30_HIMEM_ONLY_OK`, `S30_EMS_STATUS_OK`, `S30_EMS_MAP_OK`, `S30_EMS_UNMAP_OK`, and `S30_EMS_FREE_OK`. |
| COM3 open and controller-local loopback | Real guest pass | Unchanged original paths emit `S30_COM3_OPEN_OK` and `S30_COM3_LOOPBACK_TX_RX_OK`; loopback is explicitly not external-peer proof. |
| LPT1 byte output | Real guest pass | `L30.COM` emits `S30_LPT1_WRITE_OK`; the test observer temporarily maps only its `LPT1` test DOS-device name to a private named pipe, observes byte `5A`, then removes the mapping. |
| External COM transmit and host-driven receive | Real boundary, deferred | The only discovered endpoint is USB Billboard `COM3`, whose CTS/DSR are low.  The unchanged original CTS gate therefore retains its timeout behavior.  The exact provider, hardware inventory and retest condition are in the [external COM boundary record](m0-t420-s30-external-com-boundary.md) and the debt ledger. |
| Original `nt_com.c` success lifecycle without a real peer | Test-only mock pass | Direct compilation of the unchanged original provider against a controlled Win32 device contract passes `open → write → receive queue → read → close` and emits `T420_S30_NT_COM_MOCK_OPEN_WRITE_READ_CLOSE_OK`.  It is not deployed and cannot replace the external-medium result. |

## Reproducible inputs and verification

`Build-T420S30EmsGuestTest.ps1` rebuilt the `D30`, `P30`, and `E30` guest
fixtures plus the PIF/CONFIG/AUTOEXEC inputs under
`build/M0-T420/S30/closure-fixtures-r3`; it writes only the declared test
media under `O:\winnt\tests`.  `Build-T420S30CommsGuestTest.ps1` rebuilt the
COM/LPT guest fixtures.  `Verify-T420S30ComMock.ps1` compiled and ran the
direct-source mock from the same closure build root.

The formal x86 r2 product build completed all 526 declared edges and deployed
the five selected artifacts (`run16.exe`, `basesrv.exe`, `ntvdm.exe`,
`dtmgr.exe`, and `VDMREDIR.dll`) to `O:\winnt`; hashes are recorded in
`O:\winnt\logs\m0-t420-s30-formal-artifacts-r2.json`.  Its full 17-route
COMMAND/MEM/EDIT matrix passed in
`O:\winnt\logs\m0-t420-s30-command-r2-summary.json`.  The real profile,
loopback, and LPT transcripts cited in the S30 status record remain the
runtime evidence; test-only compilation does not alter that product build.

A subsequent independent ConPTY rerun of `C31.COM` using the deployed package
also returned `exit=0`, emitted `S30_COM3_LOOPBACK_TX_RX_OK`, and contained no
guest command-resolution failure. This confirms that a transient timeout in a
previous aggregate rerun was not a reproducible UART-loopback or worker-exit
defect.

## Disposition

The reachable selected profile, EMS, COM-controller, LPT and original-provider
success contracts are proven.  The only unavailable result is external serial
peer I/O, whose original failure is preserved with zero product diff and a
concrete hardware/provider retest condition in `TODO.md`.  This closes S30
under the project external-boundary rule without relabeling that missing
physical capability as a pass.

## 2026-09-18 reopened verification

At the owner's direction S30 was reopened without changing the product.  The
current package passed all five retained guest routes again: `D30.COM`
(`S30_HIMEM_DOSX_OK`), `P30.COM` (`S30_PURE_DOS_OK`), `E30.COM` (all five
EMS markers), `C31.COM` (`S30_COM3_LOOPBACK_TX_RX_OK`), and `L30.COM`
(`S30_LPT1_WRITE_OK`, with the temporary LPT named-pipe mapping removed by the
observer).  Their raw transcripts are `O:\winnt\logs\m0-t420-s30-reopen-*.raw`.
The unchanged direct-source `nt_com.c` mock also again emitted
`T420_S30_NT_COM_MOCK_OPEN_WRITE_READ_CLOSE_OK` from
`build/M0-T420/S30/nt-com-mock-reopen-r1`.

The live Windows serial map remains only `\Device\USBSER000 = COM3`; it
does not add the CTS-capable external peer required for the original INT 14h
transmit/host-receive route.  No source, overlay, adapter or deployed artifact
changed.  The outstanding debt and its exact hardware retest condition remain
unchanged.
