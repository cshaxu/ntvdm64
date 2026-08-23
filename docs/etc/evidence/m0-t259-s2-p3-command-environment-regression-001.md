# M0 T259 S2 P3 — COMMAND Environment Regression

The directly imported OpenNT `cmdenv.c` environment transformation was run in
a fresh formal-r4 graph. The fixture passed its session-owned source snapshot,
sorted MULTI_SZ construction, stale-COMSPEC removal, OEM conversion and
dynamic VDM environment checks:

```text
T234 S2 OpenNT dynamic COMMAND environment composition verified
```

The fixture previously declared only `bx-vdm`, even though the current static
library closure also reaches mantle/core/opennt-host through active machine and
INT06 provider references. Its manifest now declares those actual transitive
libraries. No OpenNT or Bochs source changed.
