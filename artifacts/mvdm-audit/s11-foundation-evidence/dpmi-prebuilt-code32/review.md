# Current package: DOS 32-bit code-segment witness

The isolated sealed package ran the existing `D38R32.COM` probe, SHA-256
`C8666EAAE8757DBFCA0B08E73731DD457FB4E174DC1FE48957840D8DE59FFB24`.
The retained entry disassembly shows a DPMI 32-bit client request and the
descriptor setup for the 32-bit code-segment transition. Its guest output
reports `S38_INT21_CODE32_OK`, high EDX/buffer handling, client-state checks,
I/O failure cleanup and network/print real/protected comparison; Run16 exited
zero. The runner checked the CODE32 and I/O cleanup markers. It reuses the
older runner's `xms-failure` switch as an internal selector, but this result
is an INT 21 code-segment test, not an XMS test. The Z: mapping was removed.

This observes actual guest execution for this prebuilt binary and package. It
does not establish the probe's source build provenance, test the dedicated
32-bit-code interrupt/fault-return path, or establish all INT 21 operations.
No guest was rebuilt or modified, and no installed image or product source was
changed.
