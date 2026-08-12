# M0 T95 S5: Generic Gather-Read Admission

## Why it is necessary

OpenNT `demsrch.c` retains search state per current PDB/PSP. The existing DTA
registration deliberately retains only a physical locator for the current-PDB
word; it cannot lawfully substitute that address for the guest value. Each
search service also needs one direct contiguous buffer. At least two disjoint
guest ranges are therefore needed at one copied #UD boundary:

1. the service buffer (`OpenBuf`, DTA, or `SEARCHBUF`), and
2. the two-byte current PDB value at the registered locator.

This is a transport requirement, not a DOS search convenience. Omitting it
would collapse distinct processes into one continuation owner and contradict
the source lifetime model.

## Admitted shape

The successor is a generic adapter/Bochs mechanical record with a bounded
array of fixed physical ranges and corresponding copied byte spans. It must:

- be versioned separately from v1, preserving v1 behavior unchanged;
- cap both range count and total bytes at compile-time fixed bounds;
- validate every range against the aperture before any copy;
- keep consumer identity, selector, service number, PDB interpretation, and
  host capability outside the record;
- retain pass-through on malformed records or failed copies; and
- permit one atomic completion after all ranges are copied from the same
  boundary snapshot.

## Explicit exclusions

This admission does not authorize a Bochs DOS/BOP decoder, guest pointer
crossing, arbitrary scatter I/O, host-file reads, or a search implementation.
The only possible Bochs change is generic bounded copying of the already
validated ranges, recorded as a mechanical intrusion if it proves necessary.
The resulting bytes remain adapter-owned and are consumed only after the
same-island boundary check.

## Next evidence

The adapter-side record and synthetic fixture are now present:
`bx_ntvdm_guest_gather_read_action_v1` accepts up to four ranges and 256 total
bytes. Its 2026-08-10 C11 fixture proves pass-through/resume preservation, a
128-byte plus 2-byte request, zero-range rejection, overflow/total-cap
rejection, and invalid-record rejection. Reproduce it with:

```powershell
cmake --build artifacts/build/cli-component-layout-check --target bx-ntvdm-guest-gather-read-action-v1-test
& .\artifacts\build\cli-component-layout-check\bx-ntvdm-guest-gather-read-action-v1-test.exe
```

It is connected in source to the generic Bochs #UD seam, before the legacy
one-range v3 consumer. A gather pass-through alone reaches v3, preventing a
legacy generic trigger from consuming a declared gather trigger first. The
source boundary check passes; same-version native fixture evidence remains
unavailable because r24 now has a retained first compiler stop before the
fixture can link. The adapter-side lifecycle is now also complete:
one `BX_NTVDM_HOST_PENDING_GUEST_GATHER_READ` holds the copied boundary,
validated action, and fixed 256-byte output arena; take/complete require that
same boundary and clear on a mismatch. Its host-session regression queues and
takes the 128+2 case, then verifies mismatch cancellation and successful
completion. The output encoder may be unit-tested from source-shaped inputs,
but only after the Bochs fixture passes may a search service bind its PDB range
to the generic facility and dispatch those DTA/FCB atomic outputs.

## Source-shaped search-result output plan

`bx_ntvdm_search_result_v1` now prepares, but does not dispatch, the two
atomic multi-write response forms using an already-projected namespace entry
and an opaque session token.  The pathname form writes the DTA token and
22-byte result suffix; the FCB form writes exactly bytes 1--51 of `SEARCHBUF`
in two writes and deliberately leaves byte 52 untouched.  It performs no
enumeration, wildcard matching, service selection, or FCB-next empty-result
decision.  Its focused C11 fixture proves address/length/payload layout,
8.3 projection, token byte order, atomic preflight and byte-52 preservation.

Reproduce the full current S5 focused regression set with:

```powershell
cmake --build artifacts/build/cli-component-layout-check --target `
  bx-ntvdm-search-result-v1-test bx-ntvdm-guest-gather-read-action-v1-test `
  bx-ntvdm-host-session-test bx-ntvdm-adapter-runtime-test
& .\artifacts\build\cli-component-layout-check\bx-ntvdm-search-result-v1-test.exe
& .\artifacts\build\cli-component-layout-check\bx-ntvdm-guest-gather-read-action-v1-test.exe
& .\artifacts\build\cli-component-layout-check\bx-ntvdm-host-session-test.exe
& .\artifacts\build\cli-component-layout-check\bx-ntvdm-adapter-runtime-test.exe
```

The 2026-08-10 execution built and passed all four.  The runtime target emits
`identity-to-pending-transaction path verified`; the other focused targets
return zero without diagnostic output.

## Adapter-only paired search-plan core

`bx_ntvdm_search_plan_v1` is the first composition of the completed S2--S5
adapter pieces.  It accepts only an adapter-local, already-normalized query:
an admitted drive index, a DOS-relative 8.3 directory sequence, an 8.3
wildcard and requested attributes.  It enumerates through the frozen
read-only namespace, rejects volume-label requests pending their trace gate,
filters hidden/system/directory entries unless requested, copies the selected
ordered entries into the PDB-bound session store, and emits the existing
atomic pathname or FCB result plan.  It reads no guest memory and does not
decode a selector, BOP, DEM service, CPU register, or Bochs object.

The focused fixture creates a temporary contained directory, proves pathname
first/next ordering, a cross-PDB token rejection, a final zero token, FCB
output, attribute admission, and no-match with no output plan.  Reproduce it
with:

```powershell
cmake --build artifacts/build/cli-component-layout-check --target bx-ntvdm-search-plan-v1-test
& .\artifacts\build\cli-component-layout-check\bx-ntvdm-search-plan-v1-test.exe
```

This is not runtime dispatch: translating the four source-defined guest
buffers plus current-PDB word into the normalized query and emitting the
source-defined no-match result are still separate S5 gates.  FCB-next empty
behavior and volume labels remain fail-closed.

## Source-shaped request and continuation decoding

`bx_ntvdm_search_request_v1` is the bounded decoder in front of the search
plan.  It takes copied 128-byte `OpenBuf` staging bytes only after a later
service admission, requires an in-buffer NUL, ASCII/OEM 8.3 components, and
an explicit rooted drive form (`C:\\...`).  It produces the adapter-local
drive, relative directory, wildcard and requested attributes.  The FCB form
implements the source rule `AL ? DL : 0` for its attributes.  Path next reads
the two opaque words from DTA offsets 0/4; FCB next reads them from SEARCHBUF
offsets 32/36.  It neither looks at SEARCHBUF byte 52 nor reaches a host path.

The explicit-drive restriction is a first-profile admission boundary, not a
claim that DOS relative-drive/current-directory behavior is implemented.
That state has no admitted owner yet, so relative and drive-relative strings
fail closed instead of being mapped to a host current directory.

```powershell
cmake --build artifacts/build/cli-component-layout-check --target bx-ntvdm-search-request-v1-test
& .\artifacts\build\cli-component-layout-check\bx-ntvdm-search-request-v1-test.exe
```

The focused C11 fixture passes for pathname and FCB first input, pathname and
FCB continuation token offsets, unterminated input, drive-relative input and
a zero token.  It remains a pure adapter decoder: no selector/runtime dispatch
or Bochs access is added.

## Pathname no-more-files result

The source-defined pathname error case now has a separate atomic result
builder: `bx_ntvdm_search_result_v1_prepare_path_no_more`.  It writes exactly
the DTA continuation pair at offsets 0--7 as zero and returns a checked v2
CPU result with `AX=ERROR_NO_MORE_FILES (0x12)` and `CF=1`.  This matches the
pathname `demFileFindNext` invalid/exhausted behavior and the FindFirst source
normalization of file-not-found to no-more-files; it does not overwrite the
remaining DTA bytes.

The encoder test now verifies the single eight-byte write, zero payload,
preflight, AX delta and CF result.  No equivalent FCB error builder exists:
the FCB-next empty-result contradiction remains an explicit trace gate.

## Paired adapter transaction core

`bx_ntvdm_search_transaction_v1` is the one adapter-owned composition point
for the source-shaped byte decoder, namespace/session search plan, successful
atomic outputs, and the pathname no-more plan.  Its inputs are already copied
source buffers plus a supplied current-PDB value; it has no guest address
translation, selector, BOP, DEM dispatch, Bochs object, or host-path escape.

For pathname first/next, `1` returns a success transaction and `0` returns a
fully prepared no-more transaction; malformed first input is rejected before
any output, while an absent/invalid next token follows the source no-more
path.  FCB first/next returns a transaction only for the proven success path.
Any FCB no-match/error is deliberately rejected with no transaction until its
source ambiguity is resolved.  This distinction prevents the pathname error
model from silently becoming an FCB implementation.

`bx-ntvdm-search-transaction-v1-test` passes for pathname continuation,
pathname invalid-token no-more, FCB continuation, FCB invalid-token
fail-closed, and malformed path-first input.  It uses private session entries,
not a Bochs or guest runtime.

## r24 native-fixture attempt

The retained r24 root was re-audited on 2026-08-10. Its `configure.log` and
generated `Makefile` prove that configuration completed successfully; the
earlier observation at `checking for fseek64...` was only an intermediate
timeout view and is withdrawn. `build.log` records the actual stop:
`make: *** No rule to make target 'ntdos64-observation-fixture'. Stop.`

This is a project build-shim spelling mismatch, not a Bochs source or toolchain
failure. The configured Windows Makefile defines
`ntdos64-observation-fixture.exe` because its `@EXE@` suffix is `.exe`. The
script has been corrected to request that exact declared target. The target's
static prerequisite closure is the native Bochs iodev/hdimage/cpu/cpudb/memory
and nogui libraries plus `BX_OBJS`/`SIMX86_OBJS`; it excludes `all`, `bochs`,
`bximage`, `bxcommit`, and any newly admitted device or OpenNT semantic object.

That one corrected invocation was executed once. It reached the first actual
minimal-target compiler action, `iodev/devices.cc`, and stopped because the
generated `config.h` has no `SIZEOF_INT_P` value of 4 or 8:

```text
config.h:509:4: error: #error "could not define bx_ptr_equiv_t to size of int*"
```

Thus r24 classifies as **external configuration/toolchain incompatibility
before adapter/gather compilation**, not a gather seam error. The target did
not link and no fixture executable exists. Per T95 policy this does not
authorize a generated-config or configure compatibility patch, a second build
attempt, a device change, or a default/all build. The exact log is retained at
`artifacts/build/bochs-2.6-native-nogui-ucrt-r24-gather-cpu5/build.log`.

### Exact configuration provenance

The retained `config.log` narrows the external condition further. The
unmodified Bochs 2.6 `AC_CHECK_SIZEOF(int *)` probe invoked UCRT64 GCC, but
the linker reported `reopening conftest.exe: Permission denied`; all preceding
unsigned-size probes completed successfully. Autoconf therefore fell through
to its historical `SIZEOF_INT_P=0` case, which caused the later `config.h`
error. This is an external temporary-executable lock during configure, not an
x64 pointer-size result and not a project or Bochs source diagnostic.

No retry is made in the recorded r24 directory. A future task may only use a
new, clean build root after the external locking condition is independently
cleared and its toolchain/process provenance is recorded; it must still build
only the declared fixture target once and must not patch configure/config.h.

### Approved-toolchain retry check

The governing execution rules require the Bochs island to use MSVC with a
documented Bochs 2.6 Win32 recipe.  The local VS 2022 BuildTools installation
does provide `cl.exe` and `nmake.exe`, so the absence is not an unavailable
compiler.  The imported 2.6 tree, however, provides only its historical
`vs2008/bochs.sln` / `.vcproj` **full `bochs.exe`** solution; it contains no
MSVC/nmake recipe for the declared `ntdos64-observation-fixture` target.

Consequently a fresh UCRT configure retry would not be accepted Bochs-island
evidence, while invoking the VS2008 full solution would violate the declared
minimum-target boundary.  The next admissible recovery is a separately
admitted S6 MSVC fixture recipe derived from the already audited target
closure, followed by one clean-root build; this is a build-recipe task, not a
permission to alter Bochs machine semantics or retry `bochs.exe`.
