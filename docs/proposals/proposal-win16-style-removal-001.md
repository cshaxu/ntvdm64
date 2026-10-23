# Win16 Style removal

## Objective

Remove the project-owned Win16 Style feature completely because its successor
now lives in a separate project. The resulting runtime must retain ordinary
DOS, Win16, Win32 and Win64 launch/lifecycle behavior, but expose no Win16
Style command option, propagation state, injected styling path, style DLL,
demo or release asset.

## Scope

The admitted implementation must trace and remove the whole feature graph,
not just hide its command-line switches:

- Run16 parsing and propagation of `--win16-style` and
  `--win16-style-all`, including inherited child-launch state;
- BaseSrv/RPC records, reservations and worker-facing style state that exist
  solely for this feature;
- NTHook and NTVWM/WOW attachment or propagation branches that select style
  injection;
- the `win16-style` component, `ntw16s32.dll`, `ntw16s64.dll`, `W16Style.exe`
  and their tests/demo/build rules;
- release-manifest, package-stage and deployment entries for those three
  images; and
- documentation, evidence and divergence registrations whose only current
  purpose is this removed feature.

## Required approach

Start with a complete producer-to-consumer inventory and classify every style
field, protocol member, switch, build target and release artifact. Remove
feature-only state and simplify any now-dead transport rather than retaining
a disabled configuration plane. Preserve unrelated Run16 classification,
BaseSrv lifecycle RPC, worker-base protocol, NTHook launch propagation and
WOW/NTVWM behavior. Do not modify, rebuild or patch guest media. Do not
replace the removed feature with a compatibility shim, a hidden default or a
new styling implementation.

## Verification and exit

Verify the normal no-style launch paths for DOS, Win16, Win32 CUI/GUI and
Win64 CUI/GUI as applicable to the selected product matrix; prove all removed
public switches are rejected; prove no style DLL/demo is built, staged,
published or referenced; and rehash the reduced release set at both release
destinations. The source audit must show that no stale protocol consumer or
propagation branch remains. A package is eligible for closure only after
the required build, tests, publication, hash verification, review, commit and
push.

## Non-goals

This package neither imports the successor project nor changes its interface.
It does not expand, replace or close the queued WOW32 capability packages.
