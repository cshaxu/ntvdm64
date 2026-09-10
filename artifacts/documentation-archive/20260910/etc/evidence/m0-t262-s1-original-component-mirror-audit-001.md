# M0 T262 S1 — original-component mirror audit

## Question

Do the three original-code component roots contain only byte-identical source
files or explicitly marked/indexed original subsets and minimal edits?

## Inputs and procedure

- Compared each `src/bx-core` file against the same relative path in pinned
  Bochs 2.6: `O:\repos.external\bochs-2.6-compat\bochs-2.6`.
- Compared `src/opennt-guest/dos-v86` and `wow16` against
  `refs/opennt/base/mvdm/dos/v86` and `refs/opennt/base/mvdm/wow16`.
- Compared every `src/opennt-host` source/header to its named OpenNT path,
  then scanned local `DIVERGENCE(<ID>)` markers and its README register.
- Inspected untracked files inside production roots separately from unrelated
  untracked repository material.

## Observations

### bx-core

- 183 non-README files: 164 exact, 18 modified original Bochs files, and one
  non-mirror project header: `cpu/bx_ntvdm_exception_intercept.h`.
- The modified files are `cpu/access.cc`, `access32.cc`, `cpu/cpu.cc`,
  `cpu/cpu.h`, `ctrl_xfer16.cc`, `ctrl_xfer32.cc`, `exception.cc`, `i387.h`,
  `icache.cc`, `init.cc`, `instr.h`, `io.cc`, `soft_int.cc`, `stack.cc`,
  `string.cc`, `memory/memory.cc`, `memory/memory.h`, and `memory/misc_mem.cc`.
- `bx-core/README.md` names the central exception IDs, but none of the 18
  changed files currently has a local `DIVERGENCE(<ID>)` marker. This fails
  the owner-required mirror standard even where the central intrusion ledger
  has already approved the mechanical change.
- The non-mirror header is a typed `#UD` mechanical-boundary declaration. S2
  must decide its actual adapter-facing owner and move it with `git mv`, or
  merge its declarations into a properly owned existing boundary header. It
  cannot remain merely because `exception.cc` consumes it.

### opennt-guest

- `dos-v86`: 578/578 files exact. `wow16`: 2/2 exact. No modified or
  non-mirror path was found.
- `dos-v86/cmd/append/sysmac.lib` is an untracked file, but is byte-identical
  to its OpenNT counterpart. It is a legitimate original guest input, not a
  code exception; S3 must decide whether to version it as a required mirror
  input or record a source-build exclusion without touching unrelated
  untracked artifacts.
- The empty local intrusion register is therefore correct for the tracked
  guest source; no local `DIVERGENCE` marker is needed.

### opennt-host

| Live path | Original path | Result | Local exception coverage |
| --- | --- | --- | --- |
| `inc/oemuni.h`, `inc/vrnmpipe.h` | `base/mvdm/inc/*` | exact | none needed |
| `softpc.new/host/inc/error.h`, `nt_pif.h`, `nt_uis.h` | matching `base/mvdm/softpc.new/host/inc/*` | exact | none needed |
| `softpc.new/host/src/nt_pif.c` | matching OpenNT path | exact | none needed |
| `vdmredir/vrputil.h` | matching OpenNT path | exact | none needed |
| `softpc.new/host/src/nt_error.c` | `base/mvdm/softpc.new/host/src/nt_error.c` | reachable subset/minimal edit | `HOST-DIV-012..014` local and indexed |
| `vdmredir/vrinit.c` | `base/mvdm/vdmredir/vrinit.c` | reachable subset/minimal edit | `HOST-DIV-017,022,024` local and indexed |
| `vdmredir/vrnmpipe.c` | `base/mvdm/vdmredir/vrnmpipe.c` | reachable subset/minimal edit | `HOST-DIV-015,016,020,021,023,024` local and indexed |
| `vdmredir/vrputil.c` | `base/mvdm/vdmredir/vrputil.c` | reachable subset/minimal edit | `HOST-DIV-018,019` local and indexed |

All 13 `HOST-DIV-012..024` README entries have a corresponding local marker.
No non-mirror production file is present below `opennt-host`.

## Disposition

S2 repairs `bx-core` local annotation/index quality and removes its non-mirror
header from the component. S3 closes the guest versioning decision without
changing its source content. S4 performs a hunk-level confirmation of the
already consistent `opennt-host` subset/README coverage and repairs any gap
found during that review. S5 repeats the full hash/marker audit and builds the
affected formal closure.

## Confidence and limitation

Hash results are high confidence for exact files. The current S1 ledger is
complete at file granularity; hunk-by-hunk marker placement for the 18 Bochs
changes is deliberately deferred to S2 so it can be corrected rather than
mistakenly declared compliant.
