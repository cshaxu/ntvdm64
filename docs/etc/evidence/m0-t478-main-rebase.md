# T478 rebase onto main

## Request and inputs

Owner requests replaying this worktree onto the already-pushed main branch,
then explicitly delegates all combined compilation, tests and publication to
the main worktree. No build, test or publication is performed here.
Original branch tip e2ba4beee; base cd91c8a57; target main572d854a3.
The66 branch commits are replayed above the17 unique main commits.
Backup ref: codex/mirror-audit-before-main-rebase-20261106.
Main's uncommitted VM/frontend and queue/proposal work is untouched and is
not included in this rebase.

## Resolution

- Preserve both runtime implementations. Main's VM COMMAND bridge and startup
  readiness additions compose with T478's redirector cleanup and ForceDOS
  propagation. CMake and the formal Ninja generator merge automatically.
- Keep main application0.0.479 and protocol67.
- Keep main's T479 S6 active packet; preserve T478 closure and all stage evidence.
  Merge both document indices, TODO rows and mirror exception records without
  duplicated document bodies.
- Main and T478 independently used divergence IDs870/871. Keep main's bridge
  IDs; renumber T478 XMS/FDC records to887/888 and the XMS comment references.
  These are provenance changes only, with no executable statement changes.
- Keep the ten main release images and manifest as one historical snapshot.
  Retain T478's added VCDEX.DLL as historical output, explicitly outside that
  ten-image manifest until main builds the full eleven-image package.

## Verification and handoff

Static review compares the final source with the independent three-way merge,
checks conflict markers and whitespace, and verifies main is an ancestor.
No compilation, test, runtime gate or publication is claimed. Main must build,
test and publish the combined source, including VCDEX. Historical build paths,
commit hashes and package receipts remain historical rather than being rewritten
to suggest they validated this rebase. No O:/winnt write or remote push.

Static verification result: main572d854a3 is an ancestor of the rebased HEAD;
all66 commits were replayed. The final src/tools/CMake tree matches the independent
three-way merge result except the resolved version header, merged provenance
README and the two XMS comment-ID replacements. No conflict markers remain in
changed source/document files; git diff --check passes. Assets differ from main
only by the retained historical VCDEX.DLL. No product verification was run.


## Latest main checkpoint included

Main advanced during the first replay to f095c77bf, preserving its previously
uncommitted S6 work. A second rebase includes that checkpoint too; final base
is f095c77bf. Main's paused S6 status, candidate fill implementation, tests,
queue/proposals and recorded failing fill002 normal exit are retained. This
worktree neither repairs nor certifies that separately tracked failure.
The original66 commits plus the integration reconciliation commit are now
above the latest main checkpoint. Main worktree/index remain untouched.
Final static comparison: src/tools/tests/test/CMake/assets match the independent
three-way merge b753228e8c88c4b80c5560006060f712f8faa7f3 exactly. Mainf095c77bf
is an ancestor; diff whitespace check passes. Main worktree is clean and still
at f095c77bf. No compilation, tests or publication were performed.
