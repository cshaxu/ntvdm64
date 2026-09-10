# M0 T310 S8 P4 — C-video native-word varargs closure

## Scope

This closure covers all five `%d` diagnostic calls reached in the selected
C-video generated-rule profile:

- `ev_glue.c`: the two read-pointer chain messages;
- `j_c_lang.c`: the three `mask` range messages.

## Source finding

Under `CVIDC_RULE_WORD`, `IUH` is `UINT_PTR`.  The historical `%d` format
therefore consumed only an `int` while each caller supplied a native host word
on x64.  These are diagnostic-only calls, but they are still a real variadic
call ABI mismatch.

## Resolution

`MVDM-HOST-DIV-122` adds the standard `<inttypes.h>` declaration and uses
`PRIuPTR` with a `uintptr_t` view at those five calls.  No generated rule,
video table, controller state, mapping-manager behavior, or guest value
changes.

## Verification

Forced formal `/MT` Ninja object rebuilds completed for both source units on
both architectures:

- `build/M0-T310/S8/p1-machine-source/x64/cvidc-native-word-varargs.log`
- `build/M0-T310/S8/p1-machine-source/x86/cvidc-native-word-varargs.log`

Neither log contains a C4477 warning for `ev_glue.c` or `j_c_lang.c`.  Other
historical, non-width C-video warnings remain compiler-visible.
