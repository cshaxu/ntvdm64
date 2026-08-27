# M0 T282 S3 — COMMAND console declaration-carrier evidence

## Question

Can the whole original COMMAND keyboard-layout source unit parse its next
historical console declaration boundary on both supported host architectures
without selecting a keyboard BOP provider?

## Inputs and procedure

- Compared `src/opennt-platform-abi/source/opennt/public/internal/windows/inc/winconp.h`
  to pinned OpenNT `public/internal/windows/inc/winconp.h`.
- Registered the exact source/hash and original `cmdkeyb.c` consumer in the
  platform-ABI manifest.
- Added an adapter-win32 declaration-only forwarder, retaining the existing
  adapter-first include order in the T282 COMMAND static graph.
- Outside the sandbox, regenerated the formal MSVC `/MT` graph and compiled
  original `cmdkeyb.c` on x64 and x86 under `build/M0-T282/S1/{x64,x86}`.

## Observations

- The selected OpenNT and OpenNT-4.5 carriers are byte-identical. The imported
  carrier hash is `26b1b3cff877ee2802aeff0eeab04f1daf7127e63279bcaa990c8b4f97384f2d`.
- Both probes compiled original `cmdkeyb.c`, including the historical
  `GetConsoleKeyboardLayoutName` declaration. They produced only original
  C-string, conversion and unused-local warnings.
- No keyboard layout, registry query, system-directory lookup, guest span,
  `cmdInitConsole`, `54:0E` route, executable or trace was invoked.

## Interpretation and follow-up

The exact private console declaration boundary is closed for static source
selection. The original keyboard provider body remains unchanged and inactive.
Its later recovery must bind the existing public console and registry APIs,
the source-shaped guest-memory facade and session policy as one COMMAND
provider package; this declaration S neither authorizes nor supplies it.
