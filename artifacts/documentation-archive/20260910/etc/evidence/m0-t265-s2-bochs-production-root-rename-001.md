# M0 T265 S2 Bochs production-root rename evidence

## Scope

S2 performs only the admitted pathname and formal-build graph migration:

- `src/bx-core` → `src/bochs-core`;
- `src/bx-mantle` → `src/adapter-bochs`.

The moves are Git renames.  No Bochs instruction, memory, machine or adapter
contract is changed by this packet.

## Recorded migration

- Git records 183 tracked files under the old core root and 20 under the old
  mantle root as renames; pre-existing unstaged content edits remain unstaged.
- All live production include paths now use `bochs-core/...` or
  `adapter-bochs/...`.
- The formal component manifest, include-root projection, module-library names
  and generator validation now use `bochs-core` and `adapter-bochs`.
- The retained fixture directory name `tests/bx-mantle/` is historical test
  organization only; its source includes and the formal library inputs use the
  renamed production roots.  It is not a production component root.
- `refs/bochs/bochs.h` and `refs/bochs/gui/siminterface.h` are formal-build
  support headers.  Their four project-local include paths were changed to the
  renamed roots; no upstream Bochs declaration or behavior changed.

## Formal build record

The fresh graph root is `build/M0-T265-S2/r001`.  Its first execution exposed
one omitted support-header path (`bx-core/cpudb.h` in
`refs/bochs/gui/siminterface.h`); that path was repaired before the recorded
full graph execution.  The full Ninja process completed with exit code `0`:
390 compile/archive/link steps and 81 linked `.exe` outputs.  Its
`build-full.log` and `build-full.exitcode` are retained in that disposable
build root.

After removing the corrupted disposable `.ninja_deps` written by the earlier
duplicate processes, a single-process 471-step rebuild completed with exit
`0`.  The final `ninja -n` result is exactly `ninja: no work to do.`

## Exclusions

- Historical `docs/history/`, evidence ledgers and superseded proposals retain
  their original component spelling as historical evidence.
- Existing unstaged changes outside this S2 path-only migration are not part
  of the S2 commit.
