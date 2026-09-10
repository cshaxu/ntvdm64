# M0 T400 S7 — isolated VDMDBG WRITE-selector discovery

## Diagnostic run

The existing test-only fixture was run against the selected staged x86 product:

```text
build\tools\t397-s4-vdmdbg-write-selector-probe.exe O:\t398\ntvdm32.exe O:\t398
```

It creates `DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE` child only, loads the
system `vdmdbg.dll`, forwards only `STATUS_VDM_EVENT` to
`VDMProcessException`, makes no target-memory write and terminates that child
when the bounded query ends.  It is not an ordinary-product observation.

## Result

```text
events=27 vdm-events=0 selector=0x0000 module= segment=0 result=4
selector-unavailable-before-deadline
```

The fixture received ordinary debug events but no `STATUS_VDM_EVENT`; it could
not initialise the VDMDBG module-selector view and therefore could not prove a
`WRITE`, zero-based segment `1` selector.  The result does not say that WRITE
was not loaded, that a selector does not exist, or that any `FInitMemory`
branch executed.  It only closes this diagnostic interface as unavailable for
the selected standalone x86 container.

## Consequence

No repair is selected.  A subsequent test-only observer may instead parse the
immutable WRITE NE segment data and compare it, read-only, with every live LDT
descriptor reachable from CPU40's already-observed LDT base.  Only an exact
segment-byte match may bind a selector to WRITE; absent such a match remains a
bounded unavailable result.  Any resulting selector must still be observed in
a separate unchanged ordinary run before it can attribute a CPU40 CS/IP sample.
