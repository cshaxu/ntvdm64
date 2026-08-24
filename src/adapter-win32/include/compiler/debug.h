/*
 * Target-local preprocessor compatibility wrapper for the fixed OpenNT
 * graphics-render object gate. The original PROD debug.h makes this trace
 * macro empty but gives it a fixed arity; nt_cga.c has one historical call
 * with a trailing comma after its fourth value. Preserve the original source
 * and retain the no-op PROD behavior while accepting that legacy spelling.
 */
#include_next <debug.h>

#ifdef PROD
#undef sub_note_trace4
#define sub_note_trace4(...)
#endif
