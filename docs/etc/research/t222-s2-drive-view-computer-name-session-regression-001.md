# T222 S2 Drive-View Computer-Name Session Regression

The former `50:41 demGetComputerName` NUL-only fallback did not meet the
T222 Direct/Readonly host-integrated disposition.  It is now a minimal
source-derived capability shim: `GetComputerNameW` followed by
`WideCharToMultiByte(CP_OEMCP)` replaces the OpenNT `GetComputerNameOem`
wrapper without importing its old Rtl/heap composition.

On success it preserves the original contract: write the fixed 16-byte guest
field through one checked action, pad the first 15 bytes with spaces after the
OEM name, NUL terminate byte 15, and return `CX=01FF`.  If the host query or
conversion fails, the prior source-shaped one-byte-NUL/clear-CH failure path
remains.  Both Direct and Readonly observe the same host identity; neither
creates a profile-local name.

The focused MSVC x64 `/MT` normal-session fixture compiled and linked from
`build/M0-T222-S2/031-drive-view-computer-name`; its executable exited zero.
This covers the identity member of the drive-view package, not complete T222
closure.
