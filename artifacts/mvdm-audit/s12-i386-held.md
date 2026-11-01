# S12: hold the original `i386` branches for ownership review

The observed MSVC x86 WOW32 command compiling `wkgthunk.c` does not define
`i386`. The recorded compiler predefine probe reports it absent for x86 and
x64. No corresponding project/generated C/header definition or definition
in the installed Windows SDK and Visual Studio C headers was found. A
source-body inventory finds 64 original-matching `i386` positive branches in
the observed closure, plus branches carrying existing differences. These facts
show a build selection, not that every branch is dispensable.

In `src/mvdm/wow32/wkgthunk.c`, the original x86 branch of
`WK32ICallProc32` calls `WK32ICallProc32MakeCall`; its source comment describes
x86 calling-convention and stack-cleanup compatibility. The original helper
is present as `src/mvdm/wow32/i386/callpr32.asm`. The observed WOW32 command
closure does not include that helper. The alternative C call path remains in
the source. This is a concrete reason to preserve this unselected original
branch for the separately queued WOW32/foundation recovery instead of
classifying all `i386` code as an obsolete host platform.

This is source and build-graph evidence only. It does not prove a runtime
failure, equivalence of the current fallback, or that enabling the original
helper alone is sufficient. No source, guest or product image was changed in
this review. Each other `i386` branch needs the same ownership and supported
configuration assessment before deletion.
