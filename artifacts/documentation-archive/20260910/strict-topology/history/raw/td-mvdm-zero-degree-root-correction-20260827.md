# Td Closure — MVDM Zero-Degree Root Correction

The owner corrected the MVDM BFS root definition on 2026-08-27. Zero-degree
contains all original `mvdm-host` definitions and their reachable,
provenance-verified call closure from the selected original OpenNT `mvdm` tree
already mirrored in a project `mvdm-*` component. Local but unreachable MVDM
support, tool, firmware, and guest definitions are excluded.

This replaces neither original source provenance nor final runtime disposition.
The detailed decision and the required rebaseline procedure are in
[the Td record](../etc/operations/td-mvdm-zero-degree-root-correction-001.md).
