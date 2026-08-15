# T204 S5 P4 — adapter installation configuration reader

The adapter now has an installation-time reader for the paired CLI child
environment values.  It accepts only both nonempty paths, opens each once for
read, limits each raw OEM input to 4095 bytes, copies the bytes into the fixed
T204 provider input, captures OEM root/code-page facts, and discards every
path and handle before provider construction.

`bx_ntvdm_adapter_runtime_v1_install` creates the current direct-mode COMMAND
session-context policy, invokes the reader, and binds the ready provider into
the boot namespace.  Missing, unreadable, oversized or invalid paired input
rejects installation rather than retaining the synthetic production result.

The strict MSVC x64 `/MT /W4 /WX` translation-unit compilation of both the
new reader and current `bx_ntvdm_adapter_runtime.c` passes.  During this work
three pre-existing strict-build drifts were repaired: shadowed loop locals, a
stale `cmd_return_exit_code` call lacking its launch plan, and an uninitialized
DTA registration local.  No BOP or Bochs behavior changed.

An installation-level positive/negative fixture remains required before S5
closure.
