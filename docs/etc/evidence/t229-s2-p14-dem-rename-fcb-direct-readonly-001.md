# T229 S2 P14 — `demRenameFCB` Direct/Readonly conformance

OpenNT `demfcb.c:demRenameFCB` accepts distinct checked source and destination
FCB paths, enumerates source matches, performs DOS template substitution, and
uses `MoveFileOem`; it rejects an unchanged source/destination path.

The existing FCB wildcard provider is exercised through the installed DEM
whole provider with two gathered guest paths.  Direct renames a fixture-owned
same-drive exact match.  Readonly takes the shared namespace-content gate
before host enumeration or rename, returning `AX=5`, CF set and retaining the
source file.  The fixture passes in both profiles after formal Ninja rebuild.

This closes neither wildcard/template variants nor all `demRenameFCB` failure
cases; it establishes the two-path provider and profile boundary.