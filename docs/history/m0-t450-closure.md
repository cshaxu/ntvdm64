# M0 T450 — Win16 Style removal closure record

## Outcome

The project-owned Win16 Style implementation has been removed.  This is a
host-only change: guest media and guest configuration were neither rebuilt nor
modified.

The ordinary launch and lifecycle architecture remains the authority for DOS,
Win16, Win32 and Win64 execution.  The separate `win16style` / `run16s`
project is deliberately outside this repository's build, runtime and
acceptance contract.

## Removed graph

- Run16 rejects the two former public style options; it has no style state to
  propagate.
- The style-only fields were removed from native-launch, native-hook,
  frontend and BaseSrv RPC/reservation records.  The native-hook protocol was
  advanced from version 4 to 5 to make the removed tail unambiguous.
- NTHook, NTVDM, NTVWM and WOW no longer select, transport or apply an
  injected style.  Ordinary NTHook launch propagation remains intact.
- The `ntw16s` DLL, `W16Style.exe`, their CMake targets, build/publish
  scripts, demo and feature-only tests were deleted.
- The release package is reduced from thirteen to ten images.  The three
  deleted images are `ntw16s32.dll`, `ntw16s64.dll` and `W16Style.exe`.

`NtUserAlterWindowStyle` remains in the WOW EDIT binding.  It is the original
USER control API, not a project Win16 Style feature or propagation path.

## Evidence

| Gate | Result | Evidence |
| --- | --- | --- |
| Clean x86/x64 component build | Pass, including a second no-op increment | `build/t450-s1/component-build-r6/result.json` |
| Reduced package stage | Pass, ten-image manifest | `build/t450-s2/stage-r1/product-system32-manifest.json` |
| Publication | Pass; `assets/release` and `O:\\winnt\\system32` hashes match, removed images absent | `assets/release/release-manifest.json` |
| Removed-option contract | Pass: `--win16-style` is rejected with error 87 | `build/t450-s3/runtime-r2` |
| Ordinary launch | Pass: installed `run16 --wait cmd /c exit` returns zero | `build/t450-s3/runtime-r2` |
| Base RPC graph ownership | Pass | `build/t450-s3/base-rpc-split-r1` |
| Build hygiene | Pass | `build/t450-s3/hygiene-r1/stdout.log` |
| Active-source audit | Pass: no component, transport or propagation reference remains; only the unrelated original EDIT API remains | T450 S4 audit |

The direct standalone invocation of `base-client-rpc-first-test.exe` was not
counted: it requires its existing live BaseSrv fixture endpoint and fails
without that fixture.  It is neither a product regression nor replacement
evidence.  The broader WOW lifecycle runner was started and compiled the
affected owner objects, but its controlling execution session was interrupted
before it could link/run the fixture; it is likewise not claimed as a pass.

## Boundary and remaining work

There is no retained compatibility switch, disabled mode or hidden renderer in
this project.  A consumer that wants a retro appearance must use the separate
project at its own process-boundary contract.  T450 neither tests nor promises
that external integration.

T450 is ready for owner acceptance after commit/push.  Queued WOW32 capability
work is unchanged.
