# NTVDM / WOW32 Seam Audit

This book is the navigation layer for the OpenNT-based audit of the Win16
application path through NTVDM and WOW32 to a modern Windows host. It explains
what must be retained, translated, or supplied at each seam so that an
implementation can minimize original-mirror differences while preserving the
original observable meaning.

## What this book is

- A fixed, two-level map of the audit: six Parts and thirty-three sections.
- A practical implementation guide: every section names its ownership boundary
  and the minimum-diff rule that applies before code changes.
- An English-language orientation resource written in short, translation-friendly sentences.

## What this book is not

- It is not a replacement for the separately managed OpenNT-only seam ledger.
- It is not a claim that every source candidate, product image, or runtime path
  has been verified.
- It does not audit SoftPC/V86 internals as an independent target, alter Guest
  binaries, or authorize product changes.

## Evidence status

The underlying static audit is based on OpenNT commit
`5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`. Its fixed direct source perimeter
contains 458 physical files: 201 WOW32 files, 248 NTVDM Win16-host files, and
9 `obj.vdm` files. The ledger records source contracts and modern connection
choices separately from build/image and runtime receipts. Do not turn a page
in this book, an API name, a source export, or a successful link into a claim
of semantic equivalence.

## Reading order

Read [Method and status](method-and-status.md), then use the smallest relevant
Part and section. A new finding must be assigned to an existing `x.y` section
or explicitly excluded; this book intentionally has no `x.y.z` layer.
