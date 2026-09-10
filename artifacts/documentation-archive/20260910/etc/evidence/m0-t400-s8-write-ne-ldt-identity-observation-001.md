# M0 T400 S8 — external WRITE NE-to-LDT identity observation

## Immutable-media identity

The staged `O:\t398\system32\WRITE.EXE` has an NE header at `0x400`.  Its
segment table begins at `0x440`; its one-based segment 5 entry has file offset
`0x3700` and length `0x942`.  The selected original map assigns
`FInitMemory` to `0005:0000`.

The test-only Console observer reads the first 14 bytes of that segment:

```text
8cd89045558bec1e8ed883ec0a56
```

The prefix stops before the segment's first far-call relocation.  It then
uses the existing read-only CPU40 page translation to inspect only present
LDT executable descriptors and compares that fixed prefix at each candidate
base.  It writes neither child nor media.

## Diagnostic discovery

The bounded x86 run (`t400-s8-ne-ldt-001.txt`) retained the ordinary Write
dialog and reported:

```text
cpu40-ne-attempts=23
cpu40-ne-descriptors-examined=180966
cpu40-ne-executable-descriptors=3175
cpu40-ne-matches=1
cpu40-ne-selector=0x172c
cpu40-ne-ldt-base=0x00126800
cpu40-ne-base=0x00187f40
```

Thus `0x172c` is uniquely bound to immutable WRITE logical segment 5 in this
stage/session.  This discovers module identity only; it does not establish
that `FInitMemory` was executing when the ordinary dialog was sampled.

## Ordinary selector revalidation

An unchanged non-debug Console-owner run (`t400-s8-selector-ordinary-002.txt`)
with the discovered selector and no product diagnostics read:

```text
cpu40-selector=0x172c
cpu40-selector-base=0x00187f40
cpu40-selector-code=8cd89045558bec1e8ed883ec0a566a00
```

The first 14 bytes exactly equal the pinned media prefix.  Its periodic
current CS remained `0x021f:0xaebc`, a different host/modal-wait location;
that is expected after Write reaches the error dialog and cannot identify the
earlier `FInitMemory` return.

## Result and next boundary

S8 closes the selector-identity gap without a product change and selects no
repair.  The only remaining non-mutating witness is a bounded high-rate
external sample of the now-proven selector during initial execution.  A hit
can establish a source offset; an absent hit is sampling-limited, not proof
that the segment was never entered.
