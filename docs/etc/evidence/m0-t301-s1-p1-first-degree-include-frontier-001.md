# M0 T301 S1 P1 — First-Degree Include Frontier

## Inputs and procedure

The generator `tools/governance/export-t301-s1-first-degree-frontier.mjs`
consumes only T300's frozen first-degree boundary ledger and zero-degree
physical source identities. It reads direct and recursively reachable headers
from the approved OpenNT/OpenNT-4.5 trees, preserving caller path/hash/line.
No product source, `src.old`, adapter, build result or runtime trace is an
input.

## Result

- All 34,429 T300 boundary calls have a corresponding T301 work row.
- The calls originate from 443 physical MVDM caller files and reach 758 unique
  original header identities. Their repeated include closures are normalized
  into a caller-frontier ledger; each call row references that physical caller
  identity rather than duplicating the same header closure.
- 1,285 calls retain only a *preliminary* prior public-API lead; 885 retain
  only a *preliminary* prior original-source lead; 32,259 require a new
  declaration/body search. None is treated as a final result merely by
  spelling.

## Limitation and next action

This P establishes the reproducible search frontier, not a completed source
resolution. The attempted broad definition scan was stopped before it emitted
any ledger because common spelling matches caused near-whole-tree parsing and
unbounded memory growth. Its tool and disposable logs were removed. T301's
next P searches only declaration/frontier-authorized original packages and
records explicit hard-boundary/source-unavailable outcomes where no permitted
body is available.
