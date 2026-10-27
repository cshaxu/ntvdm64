# NTVDM DOS Host / Modern Windows Seam Audit

This book is the navigable, English reading surface for the existing OpenNT
DOS-host seam audit. It answers one deliberately narrow question: how can an
NTVDM-derived host preserve the observed DOS-program contract while reconnecting
its non-WOW host services to modern Windows with the smallest defensible diff?

It is not a product-design document, an emulator architecture guide, or a
replacement for the audit ledger. It does not audit the CPU, V86 monitor, BIOS,
or guest binaries as independent projects. Those areas appear only when their
observable boundary is needed to explain a DOS-to-host contract.

## Reading the result correctly

The source baseline is OpenNT commit `5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`.
The audit deliberately does not read the NTVDM64 product implementation, modify
guest media, or claim that a modern binary has been built or run. A chapter's
static closure means its *fixed, local source-contract denominator* has been
accounted for. It never means that all historical code, all relevant modern
providers, ABI compatibility, concurrency, cancellation, or end-to-end behavior
has been proved.

The audit ledger currently distinguishes 24 fixed two-level modules. They are
the table of contents of this book, not task IDs and not a function count.
Each chapter records its exact scope, source facts, preserved semantics, modern
connection constraints, minimum-diff direction, and ownership handoff. The
authoritative machine-readable records remain in the separate audit workspace;
this book intentionally does not duplicate its inventories or snapshots.

Start with [Method and status](method-and-status.md), then follow the parts in
order. The [Glossary](glossary.md) explains the domain language used throughout.
