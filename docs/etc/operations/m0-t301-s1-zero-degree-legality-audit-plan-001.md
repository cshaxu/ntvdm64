# M0 T301 S1 — Zero-Degree Legality Audit Plan

## Purpose

The owner has paused first-degree classification.  Before T301 may inspect or
classify any first-degree source body, it must prove that the selected
zero-degree MVDM closure is structurally lawful.

## Scope

The audit consumes only the frozen original-source zero closure and the
subsequent, source-shape-confirmed rebase inputs.  It creates one physical row
per zero-degree definition identity.  A row records:

- original source root, relative path, hash, line and linkage;
- whether it is an original `mvdm-host` root or the exact confirmed input edge
  that admitted it to the continuous MVDM closure;
- exact body-parse result and any preprocessor-condition context; and
- a structural legality result, or an explicit reason that the body/variant
  still needs proof.

`src.old`, current product sources, adapters, build output, and runtime traces
are forbidden inputs.  This is a source-graph correction, not a provider,
import, build or runtime change.

## Gates

1. Every pre-existing zero identity must still name the exact hashed file in
   an approved MVDM root and be an `mvdm-host` root.
2. Every rebased identity must have at least one source-shape-confirmed old
   boundary call or a source-shape-confirmed expanded MVDM edge.
3. A translation-unit-local (`static`) target may only be admitted through an
   edge in the same original source file.
4. Each selected definition must yield an exact body, or be retained in a
   separate conditional/unbalanced-body ledger.  Conditional directives are
   evidence, never silently treated as an unconditional body.
5. No row is removed merely because a lightweight parser cannot understand its
   historical C form.

## Exit condition for the pause

T301 may resume first-degree work only after the legality ledger accounts for
every zero identity and explicitly lists every non-structurally-valid body or
variant.  Resolution of those remaining rows must be governed before they can
be silently treated as zero-degree facts.
