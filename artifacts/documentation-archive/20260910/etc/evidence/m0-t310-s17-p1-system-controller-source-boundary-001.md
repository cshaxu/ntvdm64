# M0 T310 S17 P1 — system-controller source boundary

## Selected package

S17 begins from the complete original `softpc.new/base/system/sources`
manifest plus the selected host bindings reached by its reset/callback chain.
The companion ledger assigns every source unit one disposition.  It prevents
the previous broad “system” label from hiding unrelated platform, ROM,
device-endpoint or BOP ownership.

## Original call order

The original BIOS `reset` sequence performs controller initialization in its
own order: PIC/ICA, time-of-day/PIT, keyboard, DMA, device POST, then the
host reset/event setup.  S17 retains that sequence and reads the controller
definitions, initialization/port tables and the caller forms together.  It
does not reconstruct reset inside an adapter.

The host side follows the same separation:

```text
TimerInit / host_timer_init
    -> original host heartbeat
    -> host_timer_event
    -> time_strobe
    -> original PIT / RTC / quick-event / PIC paths
```

`nt_eoi.c` supplies the selected non-MONITOR ICA lock and delayed-IRQ bridge;
`qevnt.c` remains the original event queue rather than a new scheduler.

## Boundary decisions

- `at_dma.c`, `cmosnt.c`, `ica.c`, `qevnt.c`, `timer.c`, `timestrb.c`,
  `dummy_nt.c`, `unexp_nt.c`, `nt_timer.c`, `nt_eoi.c`, `nt_inthk.c`, and the
  controller portion of `nt_reset.c` are S17 sources.
- Original ROM loading is already closed in S16.  The alternate `cmos.c` and
  `quick_ev.c` are retained byte-exact but are unselected in the CCPU40
  manifest.
- `idetect.c` and host error/UI portions of `illegalp.c`/`nt_reset.c` transfer
  to S28 rather than becoming a controller shim.
- Calls from strobe/PIC/DMA into keyboard/mouse, floppy/disk, serial/parallel,
  sound or graphics remain exact original calls and transfer to S19, S21,
  S23 and S27.  S17 will not enable them as synthetic callbacks.

## Identity rule

PIT/RTC/PIC counters, event identifiers and the selected SoftPC private RAM
backing are controller-local data, not external host-object identities.  The
mapping manager is required only if a later endpoint exports a host resource
or crosses a fixed-width guest ABI; it is not introduced merely to tokenize
original private controller state.

## Next work

The remaining S17 work is a complete call-contract review of the admitted
controller sources: callback declarations/tables, selected reset callers,
heartbeat cancellation and the exact BDA/PIC/RTC update paths.  A formal x86
CCPU40 archive will then be refreshed.  This P1 is a boundary admission, not
controller runtime closure.
