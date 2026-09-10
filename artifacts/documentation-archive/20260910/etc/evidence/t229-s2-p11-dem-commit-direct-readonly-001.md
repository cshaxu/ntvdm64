# T229 S2 P11 — DEM demCommit Direct/Readonly conformance

OpenNT release demCommit calls FlushFileBuffers but clears CF even when a read-only handle causes access denied. The formal whole-provider owned-handle fixture proves AX/BP token routing, no GPR result and CF clear in Direct and Readonly; all four formal modes exited zero on 2026-08-18.