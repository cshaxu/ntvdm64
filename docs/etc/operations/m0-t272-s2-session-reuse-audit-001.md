# M0 T272 S2 session reuse audit

## Question

Which existing `src.old/session` files can form the new neutral multi-instance
session component without carrying provider or process-global behavior forward?

## Inputs

- `src.old/session/session.c` and `session.h`;
- `src.old/session/session_cancellation.cc` and `session_cancellation.h`;
- `src.old/session/session_input.c` and `session_input.h`; and
- the M0 T272 S1 owner/source manifest and current architecture rules.

## Disposition

- `session.c` / `session.h`: **replace**. They provide the useful bounded
  lifecycle/teardown pattern, but `next_identity` is process-global and
  teardown callbacks lack a context. The new `src/session/session.{c,h}` takes
  an explicit caller-supplied identity, keeps lifecycle state in the instance,
  and registers context-bearing callbacks.
- `session_cancellation.cc` / `session_cancellation.h`: **retire**. Their
  global `active` and `reason` variables violate the multi-instance rule.
  Cancellation reason is now owned by the individual session.
- `session_input.c` / `session_input.h`: **split**. The copied strings are
  mechanically safe, but startup target, code page, environment, published
  directories and re-entry counters are app/adapter payload, not neutral
  lifecycle data. No part is imported in S2.
- `README.md`: **replace** with the current disposition register.

## Resulting contract

The new component exposes only explicit instance lifecycle, bounded teardown,
per-instance cancellation and thread-local bind/unbind. Thread-local binding
is not a process-global current session: it is empty by default, accepts one
explicit active instance per thread, and cannot cross a component ABI.

## Verification

`tests/session/session_lifecycle_test.c` verifies two independent session
instances, rejection of competing thread binding, refusal to dispose while
bound, cancellation ownership, completed-state behavior, duplicate teardown
registration and LIFO deterministic teardown.
