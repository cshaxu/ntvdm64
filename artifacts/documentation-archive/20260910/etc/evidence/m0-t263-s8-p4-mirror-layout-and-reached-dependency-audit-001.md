# M0 T263 S8 P4 — mirror layout and reached-dependency audit

Date: 2026-08-24

## Question

Can a mirror reduce review noise by restoring the upstream line layout without
reintroducing historical dependencies that are merely includable, rather than
actually composed and verified?

## Procedure

1. Compare each production mirror against its pinned upstream source using
   whitespace-normalized line matching, retaining upstream bytes for every
   logically unchanged line.
2. Treat a same-name include as admitted only when the retained body uses its
   contract and the needed provider is reachable in the active composition.
3. Remove an otherwise unused historical include, diagnostic block or ABI
   branch rather than retaining it through an empty facade.
4. Rebuild and run the focused formal host-mirror fixture.

## Results

- `opennt-guest`: all 616 comparable `dos-v86` files, two `wow16` files and
  ten `bin86` files remain byte-identical to their OpenNT baselines.
- `bx-core`: 182 comparable files were scanned; 164 are byte-identical and
  18 contain registered Bochs mechanical changes. After layout recovery, none
  of those 18 has a separately recoverable whitespace-only diff; every
  remaining diff belongs to a registered `BX-*` divergence.
- `vrputil.c`: unchanged retained lines now retain the original layout. Its
  actual provider needs only character conversion, public Win32/LAN Manager
  declarations and its utility header. The historical NT4 product includes and
  `VR_DIAGNOSE` path are intentionally cropped under `HOST-DIV-018` rather
  than represented by empty compatibility behavior.
- `vrinit.c`: the retained lifecycle gate does not use the named-pipe ABI, so
  its previously restored `vrnmpipe.h` branch was removed. Its remaining
  static-link name divergence is `HOST-DIV-017` and its whole VDD/ICA/NetBIOS
  crop remains `HOST-DIV-022`.
- `nt_error.c`: the unused self-declaration include was removed; the retained
  direct-access body keeps only the headers needed for its original constants,
  buffers and public-Win32 façade.
- `vrnmpipe.c`: its selected synchronous paths retain original control flow
  where previously equivalent shorthand had changed it (notably the
  `ERROR_MORE_DATA` and open-pipe removal paths). The few exact pure manifests
  required from broad CCPU/VDM headers are explicitly limited by new
  `HOST-DIV-025`; no empty CCPU/VDM API surface was imported.

## Verification

Fresh formal Ninja target
`bin/t263-s8-opennt-host-mirror-fixture.exe` rebuilt `nt_error.c`, `vrinit.c`,
`vrnmpipe.c`, and `vrputil.c`, archived `opennt-host.lib`, and exited `0`:

```text
T263 S8 OpenNT host mirror: utility, lifecycle and pipe paths pass
```

The composed BOP 59 fixture was also rebuilt after the `bx-core/cpu.cc`
layout restoration and exited `0`.

## Conclusion

Formatting is no longer used as a substitute for source recovery. Unchanged
mirror lines follow upstream layout; a surviving diff is either a source-cited
whole-block crop or a named, reachable semantic divergence. Future owner
packages must apply the same reached-dependency test before restoring a
historical include or block.
