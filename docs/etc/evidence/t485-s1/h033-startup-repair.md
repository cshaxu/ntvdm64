# H033 startup regression repair, current verification

Package 02D4C396-7B6E-4E71-8284-47F4D8DEB69A was built for x64/x86 and published to assets/release and O:/WINNT/SYSTEM32. All thirteen image hashes matched. Evidence is under build/t485-s4-h034.

The hardware interrupt entry now declines protected-mode handling before modifying flags or stacks when DOSX tables are absent, when the CPU is in V86, or when its active GDT/LDT no longer match the existing DOSX shadow tables. Software interrupts and fault entry were unchanged. No interrupt acknowledgement was added. DOS dialog selection now uses the existing DOS provider for ordinary DOS sessions, preserving the button result and the existing TerminateVDM path.

The three independent desktop cases passed on runtime58. Full58c also passed its Win3.1 standard, Win3.1 enhanced and Win95 desktop gates. Win3.1 reports valid desktop/mode and exit zero. Win95 reports valid desktop/mode and natural retirement under the previously approved exit-one boundary. The enhanced-mode log records an identified COM1 warning and serial-ignore=True before successful desktop and exit.

Full originally stopped at a stale REDIR hash. H032's SP3 REDIR receipt is now supplied alongside the early migration receipt. Runtime58 passes guest-media validation. Archive content was not changed by this correction.

WOW output tests passed: embedding 31 assertions, fax 135, font 16, and the independent Win16 GDI program produced GDIOK.DAT. The fax correction accepts the already-installed driver result inside its executing thread and removes the caller's invalid cross-thread GetLastError check.

The existing selected-object DPMI suite passed in dpmi58, including hardware-owner checks for absent LDT/IDT and changed GDT/LDT after initialization. Each rejected hardware entry preserved FLAGS, stack, IP, mapping count and dispatch count. This is an entry-level test with CPU accessors supplied by the fixture; actual WOW/DOS4GW coverage remains separately required.

Full58c passed all 23 gates in 554.143 seconds, including the three desktop cases, WOW windows and the real DOS4GW/NESticle menu test. Serial-close59 identified the COM1 dialog and clicked its visible enabled Terminate button; the worker exited with 0xff, before desktop readiness. Its separate acceptance intentionally requires termination, not a desktop.

Relative to the previously committed XP-mirror adaptations, this batch changes only non-excluded code at these sites. Fixed-SP1 SequenceMatcher point/line deltas are dpmiint.c +1/+8, nt_error.c +1/+2, wmdisp32.c +4/+9, wow32fax.c +2/+12. These are changes in source differences, not source length. Existing excluded branches are untouched. No new #line directive was added at these sites; prior anchors remain. The standalone raw census is mirror59.json and includes pre-existing excluded differences in its absolute totals.

The WOW dialog entry retains the existing message-stack cleanup contract after the XP entry split. Its final selected-object suite passed 191 scenarios. This correction and the fax thread result correction are included in the same published package. Normal user guest binaries were not modified or rebuilt.