# adapter-bochs

Bochs-only lifecycle and machine assembly over `bochs-core`: SIM/logging,
headless native device composition, parameter tree, port-space and PIC
lifecycle.

It may depend on `bochs-core` only. It must not contain or import OpenNT, DOS,
VDM, WOW, BOP, SoftPC/CCPU or Win32 semantics. T260 S3 moved all former
the former project-prefixed bridge, finite-run, observation and machine-session files to
`adapter-softpc`; the retained interfaces use neutral Bochs-native names.

The static library name is `adapter-bochs`. `app` and `adapter-softpc` may
consume its declared selector-blind, fixed-width mechanical facade.
`adapter-softpc` never imports `bochs-core` and never sees a Bochs type,
object or global.
