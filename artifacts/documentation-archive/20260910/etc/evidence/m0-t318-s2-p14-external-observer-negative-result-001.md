# M0 T318 S2 P14 — External observer negative-result boundary

## Question

Can an external five-second observer determine whether the running selected
x86 CPU40 process has called original `demLoadDos`, without changing product
sources, guest state, or the BOP route?

## Method

The existing disposable console-owning launcher was extended only under the
ignored build tree.  A linker map placed original `demLoadDos` at RVA
`0x001f9ec0`.  Two isolated runtime copies were then observed:

1. a Win32 debug-event observation with a one-shot hardware execute breakpoint
   on that RVA; and
2. a no-debug observation that scanned the owned child for a 32-byte static
   NTDOS file anchor after the same five-second bound.

Neither mode wrote a target-process instruction, changed a guest register, or
added a product source/build input.

## Result

Neither run yielded a positive `demLoadDos`/NTDOS observation.  That is not
evidence that the original call did not occur:

- the debug-event form changes the historical process's debugger-visible
  environment and accumulated only 109–187 ms of CPU time, unlike P11's
  normal liveness observation; and
- the no-debug form inherited unrelated Git Bash failure text into the shared
  console.  Its file-anchor scan can only produce a negative result when the
  relevant guest bytes are transient, self-modified, or not discoverable by
  that host-memory heuristic.

## Disposition

P12 remains the source/composition proof of the original `0x50:0x11` route.
This experiment does **not** transfer or create a product blocker, and does
not support a claim that `demLoadDos`, NTDOS, EXEC, or parent return failed.
Future S2 runtime evidence must use an observer whose process/console state
is independent of inherited background traffic and whose observation does not
alter the old debugger-visible host contract.
