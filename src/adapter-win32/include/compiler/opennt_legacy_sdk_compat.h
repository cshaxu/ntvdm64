/* Applied after the modern staging preamble for legacy source identifiers. */
#ifdef h_addr
#undef h_addr
#endif

/*
 * The recovered NT host headers publish host_stream_io_enabled as the
 * SoftPC boolean (IBOOL), while winsigni.h later supplies BOOL as UINT when
 * it sees no preprocessor macro. Keep the historical translation unit on
 * its signed 32-bit spelling before those headers are read. This is a
 * compile-only type spelling bridge; IBOOL and the old Win32 BOOL are both
 * int in the x86 profile and it supplies no host behavior.
 */
#ifndef BOOL
#define BOOL int
#endif
