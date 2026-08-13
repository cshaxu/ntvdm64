# T198 S18 DTA Composition Closure 001

`50:1B` is now a first-class pending-read branch of the existing
boot-namespace composition. It reads the two-byte SFT offset prescribed by
OpenNT `demSetDTALocation`, completes the existing source-derived DTA
registration record, and installs that record through the plane's existing DTA
setter. The x64 `/MT` source closure including this new direct dependency exits
zero. Search first/next are not claimed until a native continuous fixture runs.
