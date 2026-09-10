# T198 S5 P3 Finite UD Stop Witness 001

Fresh CPU5/Pentium-MMX evidence is at
`artifacts/build/t198-s5-ud-stop-probe-r1`.  The probe rebuilt the complete
finite CPU5 source closure, then replaced only `whole_cpu_exception.obj` with
`BX_NTVDM_ENABLE_MANTLE_UD_BRIDGE=1`.  Its entry bytes are `0f 0b` (`UD2`),
and the private finite-run fixture enabled only mantle typed stop.

The process exited zero.  This proves the complete mechanical path:

```text
UD2 -> copied core event -> fixed mantle bridge -> typed STOP
    -> kill_bochs_request -> original CPU-loop return -> cleanup
```

The manifest excludes `main.cc`, product configuration, GUI, device archives,
adapter, OpenNT, BOP and CLI.  No selector/service interpretation occurred.
This closes the core-to-mantle bridge witness, not BOP execution.
