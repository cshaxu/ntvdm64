# T229 S2 P9 — DEM demChMod Direct/Readonly conformance

OpenNT demChMod returns queried attributes in CX and mutates attributes only for AL nonzero. The provider had incorrectly written all query branches to DX; this repair changes startup, Direct, Overlay and retained Virtual query results to CX.

The whole-provider fixture proves host attribute query projection in CX for both modes, Direct Archive attribute set, and Readonly AX=5/CF=1 pre-host refusal. All formal Direct/Readonly modes exited zero on 2026-08-18.