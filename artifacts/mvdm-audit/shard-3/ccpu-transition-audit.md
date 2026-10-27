# CCPU return/exception transition audits

## `c_xtrn.c` / DIV-348

Byte comparison against pinned OpenNT shows one logical source change: wrapping original setjmp-driven `call_cpu_function` body in `__try/__finally`, moving `interface_active=FALSE` into the finally; original setjmp, original `check_interface_active` architectural error and callback dispatch remain. Scope is the CCPU interface-active flag, not the WOW callback-frame/task owner. Source SHA-256 `5A4B0F0F7C7B7699BA1DD31BB461854DB7169ECEF13B765F99EC18DA15ACA4BE`.

Current checked-in `verify-wow-callback-abort-ccpu.ps1` SHA-256 `92380BFA84CB2DB4A2E6F6C4BA14FDC2DAB485E54C20D2B96D6E231ABF3C6D41` was rerun against current stable worker/WOW32 graph in `build/t473-s10/shard-3-callback-r1`: exit 0, three original CPU thread abort/nested/continue reports and `CCPU_INTERFACE_OK native-unwind=1 architectural-error=13 normal-reuse=1`. The test exercises native SEH and original architectural-error plus normal reuse via actual function source; it is a focused fixture, not a real guest callback family test. It does not prove all nested cross-task shutdown races.

## `iret.c` original target stack width difference

Ignoring CRLF differences, current source has exactly one semantic hunk versus pinned original: after loading new SS, it calls `set_current_SP(new_sp)` instead of selecting SET_SP/SET_ESP from operand size; the new selection uses target stack width. Current `iret.c` SHA-256 `C9E2E694604D71064AE87351B9E23C76B0E0835A274E600B3D98B2818D5160B3`. Map `build/t473-s10/shard-3-ccpu/stack-current.map` identifies actual `_IRET` as `original-ccpu386:iret.obj`.

The existing `tests/mvdm-host/ccpu_stack_transition_test.c` SHA-256 `6963BAC40412853992B9C0390C6AAFDC055514E72C96E02F0631EA4124440FF8` was linked independently using the current worker stack-test link graph, plus its current `ccpu_host_fixture_seams.obj` to satisfy three BaseClient symbols and the graph's existing `/force:multiple` precedence. The map records the original CCPU IRET provider. Exit 0: `CCPU_STACK_PROFILE cases=36 failures=0`, including CALL/RETF/IRET mixed operand/stack widths and specified fault-before-commit cases. This demonstrates selected actual objects under the isolated CPU fixture, not all privilege, paging, task-switch, nested interrupt or real guest cases; error/abort boundaries outside those cases remain open.

Both source files retain `partial` because they contain non-original changes; test coverage cannot turn mirror differences into full identity. No existing guest or product source was changed.
