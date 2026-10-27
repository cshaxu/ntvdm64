# How to read the evidence

This book is an English reading layer over an OpenNT-only seam ledger. It is not a replacement for that ledger and it must never turn a source observation into a claim about a shipped image or a running NTVDM64 instance.

## The evidence ladder

Every substantive section uses the same six questions:

1. **What is bounded?** The exact interface family and the things deliberately transferred to another numbered section.
2. **What did the selected OpenNT source establish?** A source conclusion is tied to the OpenNT commit, path, locator, build condition, and, where available, SHA-256 recorded in the seam ledger.
3. **What must a modern carrier preserve?** The minimum-diff rule preserves ordering, widths, ownership, failure values, and retirement conditions before convenience or abstraction is added.
4. **What may not be inferred?** A source recipe is not an emitted image; an export lookup is not a successful ABI call; a static contract is not runtime compatibility.
5. **Where does the work go next?** A handoff names an existing `x.y` owner; it is not permission to silently expand the current section.
6. **What would validate implementation?** Static closure, image selection, and runtime receipts are separate states.

## Status vocabulary

| Field | Meaning |
| --- | --- |
| **Unit** | `entry`, `callsite`, `shared structure`, `protocol`, `mechanism`, or `build variant`. These are different denominators and must not be added together as “functions.” |
| **Support class** | `A` public modern API, `B` bounded conversion, `C` compatibility behavior, `D` private/original-shaped adapter requirement, `U` deliberately unresolved or conditional. It is a disposition, not an implementation status. |
| **Audit status** | A `closed` seam has a bounded static source contract, modern disposition, dependencies, and acceptance conditions. It does **not** mean built, shipped, or runtime-tested. |
| **Confidence** | The scope of the evidence—not a global quality score. Its qualifiers remain part of the conclusion. |
| **Implementation validation** | A separately recorded build/image/runtime state. `not implemented or tested by audit` means exactly that. |

## Source identity

The authoritative static baseline is OpenNT commit `5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`. Source citations use repository-relative paths so the site remains portable. The ledger retains absolute checkout paths, hashes, timestamps, raw evidence JSON, exact stable seam IDs, and all rejected or deferred candidates.

For example, `base/mvdm/wow32/wow32.c:184-282` identifies a reviewed source slice. It does not state that every helper it calls was reviewed, that this was linked into an image, or that a modern host can use the same pointer ABI.

## What the book will and will not enumerate

The audit has a large, evolving candidate frontier. Its range is intentionally not promoted to a global API or function denominator. A chapter therefore lists **canonical seam identities** that carry distinct contracts, not every lexical hit, alias, helper, disabled branch, or raw call location. Where a chapter has a finite local list, it says so. Otherwise it gives the bounded source evidence and points to the ledger category rather than inventing a completion percentage.

The record separately retains active and disabled dynamic requests, shared layouts, protocols, mechanisms, build alternatives, source-file coverage, and unresolved provider fronts. A page must preserve that separation instead of collapsing it into a false “N APIs supported” claim.

## Link to implementation review

An implementer should read a chapter in this order: read the contract and non-inferences, locate the stable seam ID in the ledger, inspect the cited source slices and hashes, then attach implementation/image/runtime receipts through the Part 6.3 promotion gate. The safe default is to preserve the original owner and introduce only a typed, named carrier at the unavoidable modern boundary.
