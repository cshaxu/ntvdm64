# M0 T284 S3 — outgoing historical-adapter root admission

## Result

`src/adapter-mvdm-host-out/` is now the sole classified home for historical
OpenNT-facing dependency bindings required by `mvdm-host`. It contains exactly
seven named family boundaries:

```text
win32  softpc  monitor  redir  wow  vdd  debugger
```

Each family has a local boundary README. The root README fixes its owner,
permitted dependencies and explicit initial no-source state.

## Verified limits

- No production `.c`, `.cc`, `.h` or public export was added.
- No existing adapter source was moved, copied or selected by a build graph.
- No family calls `bochs-core`; the future `softpc` family may reach only
  typed `adapter-bochs` mechanics.
- `adapter-mvdm-host-in` remains separate as the selector-blind inbound
  copied-frame/machine-event bridge.
- Redirector, WOW, VDD and debugger remain explicitly empty/unavailable
  families, rather than placeholder implementations.

This completes the required boundary admission. T284 S4 may now move only the
audited Win32-family inputs from the S1 inventory.
