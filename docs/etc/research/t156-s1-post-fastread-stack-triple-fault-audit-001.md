# T156 S1 post-fast-read stack triple-fault audit 001

## Question

Does the post-T155 stack stop identify a bad `50:42` return, a Bochs stack
mechanics defect, or an unobserved guest/control-state predecessor?

## Trace facts

The last fast-read event in
`artifacts/analysis/t155-s1-fastread-observation-001/bochs.log` is at tick
320,848:

```text
selector=50 ... service=42
bulk transaction committed bytes=0
accepted resume cs=0032 eip=6351 ss=00a7 esp=0918 ... eax=4005 ... flags=46
```

It is the expected successful EOF disposition: zero payload, resume at the
four-byte BOP continuation, and no failure result.  Roughly 5,696 ticks later
the sole terminal state is:

```text
stackPrefetch(2): access [0x0000ffff] > SS.limit [0x0000ffff]    (three times)
CS=0000 EIP=019d  SS=00a7 ESP=0003  instruction: push es
exception(): 3rd (12) exception with no resolution
```

The trace has no `50:43` event.  Thus the stop is neither an immediate
FASTREAD return failure nor a FASTWRITE selection.

## Mechanics audit

`refs/bochs/cpu/stack.cc:92-95` rejects a non-expand-down stack write when
`offset > limit-len`; it calls the architectural `#SS` path.  The same file
uses `stackPrefetch(offset, 2)` for a word write at lines 146-152.
`refs/bochs/cpu/exception.cc:1325-1327` delivers a real-mode interrupt or
exception by pushing FLAGS, CS and IP through those same 16-bit stack writes.

For the displayed instruction, 16-bit `push es` from SP=3 would first use
offset 1, which is within a `ffffh` segment.  The observed `ffffh` access is
therefore consistent with exception delivery after SP has reached 1: the next
16-bit frame push wraps to `ffffh`, which fails a conventional real-mode
SS limit.  Repeated `#SS` delivery then reaches Bochs' normal third-exception
triple-fault termination.

This demonstrates correct enforcement by the retained Bochs stack mechanics;
there is no evidence for relaxing the limit, wrapping the stack in adapter,
or changing a Bochs device.

## Guest/source correlation and limit

OpenNT's `handle.asm:459-474` shows that the original `FastOrSlow` caller
tests the returned carry and continues through `dor2`; the T155 EOF result is
consistent with this contract.  `FastOrSlow` at lines 564-579 merely chooses
the fast/slow service based on the MIPS flag and does not own later stack or
control transfer.

By the terminal state the CPU is executing `CS=0000:EIP=019d`, not the
`0032:6351` fast-read continuation.  The retained log has no instruction or
vector predecessor between them.  It cannot identify the *first* exception
that leaves SP at 1, nor bind `0000:019d` to an original source label.  The
responsible source is therefore **unobserved guest control state or an
unobserved earlier vector transfer**, with high confidence that it is not a
direct adapter BOP result and high confidence that Bochs correctly rejects
the recursive stack fault.  The exact guest/machine owner remains bounded
uncertainty.

## Rejected actions

- Do not make the stack wrap or extend SS in Bochs.
- Do not write a stack or exception workaround in adapter.
- Do not attribute `CS=0000:EIP=019d` to `50:42` absent a transition trace.
- Do not rerun the same observation: T155's single admitted run is already
  consumed.

## Follow-up disposition

A later separately admitted diagnostic package may add one default-off,
post-commit trace of the first exception/vector transition after the EOF
resume, with no guest-memory reads or state change.  Its sole purpose would
be to distinguish an original guest vector/control-flow defect from a
machine-composition predecessor.  No repair is admitted by this audit.
