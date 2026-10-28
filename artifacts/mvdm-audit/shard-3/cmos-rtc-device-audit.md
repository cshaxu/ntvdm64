# CMOS / RTC device lifecycle review

Successor: the [system-provider checkpoint](../system-native-role-review.md)
adds current image-bound source selection and real guest publication tests.
CMOSNT is now partial, superseding this record's pending classification.
The asynchronous and full IRQ8/BOP70 obligations below remain open.

Scope: shard-3 `softpc.new/base/system/cmosnt.c`, frozen byte-identical OpenNT. Source-only recheck and prior recorded tests; no product or guest change, no fresh runtime test.

`cmos_init:1050-1076` registers the IRQ8 `RtcIntEoiHook` and initializes RTC state. `cmos_inb/outb` own port registers 70h/71h, with register C read clearing its flags; `QueueRtcInterrupt:301-340` combines pending cause bits, uses `RtcEoiPending` to avoid reissuing the same outstanding request, writes C cause/IRQ flags and requests immediate or delayed IRQ8 through PIC/host. `RtcTick` uses host time for alarm/update events. `RtcIntEoiHook:344-357` clears its outstanding gate, optionally recalculates alarm, and schedules a new periodic interrupt while PIE and period remain set. It does not inspect its CallCount parameter: cancellation (`-1`) uses the same device rearming logic as normal EOI; this differs from the mouse callback and is not evidence that cancellation is silently ignored. Register C, PIC state and the EOI gate have distinct owners.

Selected `ntvdm.exe.map` resolves `cmos_init`, `cmos_inb/outb`, `RtcTick`, `QueueRtcInterrupt`, and `RtcIntEoiHook` to `original-softpc-system:cmosnt.obj`. Existing isolated `pic-reset-eoi-test` (24 assertions, logged in `pic-reset-ownership-audit.md`) exercises actual RTC callback with fixture-seeded pending/in-service PIC and mocked dispatch boundary: PIE disabled clears gate without requeue; PIE enabled creates one delayed new IRQ after reset snapshot. Those cases support the specific reset/requeue contract only. Historical T420 S24 guest sees a periodic C-register flag; it does not establish BIOS BOP70 countdown/INT4A completion, separately recorded in `bios-memory-rtc-audit.md`.

No proof yet of asynchronous overlap of PIC reset, register C read, alarm tick and delayed delivery; correct alarm rollover/time changes; zero/invalid period; device teardown or a full current-package guest witness. No current difference in this original file is identified, but linked source plus a few tests cannot prove full lifecycle, so the file remains pending. A bounded follow-up should test actual producer/cancel/requeue and register C consumption with fake clock + genuine callback and deterministic event ordering, then a separate real guest IRQ8/BOP70 wait and alarm witness. No success stub or product patch is warranted by present evidence.
