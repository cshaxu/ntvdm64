# Method, Evidence, and Current Status

## Fixed scope

The audit has 24 fixed modules arranged in eight parts. They cover the
non-WOW DOS host path: DOS program/COMMAND intent enters original NTVDM host
services, crosses a finite historical service seam, and must be connected to
modern Windows without changing the guest. The fixed module map prevents a
convenient source hit from silently becoming an unlimited audit of the V86
machine, BIOS, or a private Windows subsystem.

The source baseline is OpenNT commit `5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`.
Original-source facts and modern official-interface constraints are different
evidence classes. Identical names, a compatible-looking Win32 API, or a
successful lexical search do not establish semantic equivalence.

## What is counted

The ledger counts different units separately: entries, call sites, shared
structures, protocols, mechanisms, and build variants. A structure is not a
function; a declaration is not a compiled export; a call location is not proof
that the call succeeded. Classification A/B/C/D/U, confidence, evidence type,
static audit closure, and implementation validation are also separate fields.

The current generated dashboard reports a 299/489 manual source-file
disposition and 64/64 build-recipe review. Neither measure is a contract or
runtime completion percentage. It also reports 0/66 implementation-validation
receipts. Those numbers describe their own explicitly named denominator; this
book publishes no invented overall percentage.

## An important status qualification

The dashboard headline says 24/24 modules are closed, while the retained 5.2
module record says 13 input identities are admitted, 0 are statically closed,
and its complete denominator is not frozen. This book preserves both facts.
It treats the broad headline as a dashboard projection that needs reconciliation,
not as permission to mark keyboard/mouse semantics implemented or complete.
All other chapter status statements are likewise bounded to the denominator
named inside that chapter.

## How to use a chapter

Each module chapter follows the same reading order:

1. State its owner boundary and explicit exclusions.
2. Preserve the source-observed protocol: values, widths, ordering, failure,
   side effects, aliasing, and lifetime where source evidence establishes them.
3. Separate a direct modern API from a required compatibility provider.
4. Give the smallest connection direction, never a product patch.
5. Transfer adjacent responsibilities to the fixed module that owns them.

Static source closure can be sufficient to finish a research module. It is not
a waiver for later build, object/ABI, provider, lifetime, fault, concurrency,
or runtime evidence. Those receipts belong to Part 8.
