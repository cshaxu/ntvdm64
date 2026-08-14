# T199 S41: COMMAND native integration observation

## Question

Does the source-built CPU5/Pentium-MMX composition handle the COMMAND
`54:01` boundary after S40 removed the adapter-invented prerequisite that a
`54:05` registration must already exist?

## Inputs and method

One and only one guest execution was run through
`tools/Invoke-T198S97PostBootstrapNativeProbe.ps1` into
`artifacts/build/t199-s41-command-native-r1/`.  The script rebuilt the current
composition manifest using MSVC x64 and the static `/MT` CRT, with the locked
source-built COMMAND.COM/SHARE.EXE fixture inputs and the CPU5/Pentium-MMX
native-core recipe.  The fixture bridge binds the boot-namespace session first
and the native XMS/DPMI session only after that session declines, matching the
production generic-bridge ordering.

The retained probe JSON records `runs: 1` and guest process exit `2`; its
`run.log` is the exact observation record.

## Observation

The guest completed the previously established DEM traffic and stopped at:

```text
observed-stop=1 next=54:01 plane=0/0/1 error-lock=1 gset=1
```

`observed-first-generic-ud=0` is not evidence that #UD transport is absent.
The fixture records BOPs through its canonical ingress and deliberately stops
at the first BOP that neither bound package accepts; it only marks a non-BOP
#UD as a generic event.  The run also proves that the current source-built
manifest compiled and linked the composed providers before execution.

## Interpretation and confidence

S40 removed the known false condition that a missing copied `54:05`
registration alone must decline `54:01`.  This run nevertheless reaches an
unhandled `54:01`, so its actual startup CMDINFO register/input shape does not
yet meet the currently implemented COMMAND transaction contract.  The bridge
does not record the stopped BOP's fixed register state or inspect its guest
CMDINFO bytes, therefore this observation cannot identify which CMDINFO field,
layout constraint, bootstrap prerequisite, or return-lifecycle output is
missing.

The frontier remains the existing **COMMAND bootstrap/launch/environment/
lifecycle package**.  It is not authorization to patch service `54:01` in
isolation, to add Bochs selector knowledge, or to infer DOS/firmware behavior.
Confidence is high in the run count and frontier, and deliberately limited on
the cause.

## Follow-up

T199 S42 must recover the complete original CMDINFO ABI package from its
OpenNT callers and `cmdGetNextCmd`: packed layout, caller construction,
DS:DX input, bootstrap/environment fields, returned field writes, and failure
paths.  It must establish source-built whole-package regressions before any
new native integration run.  No second S41 guest run is permitted.
