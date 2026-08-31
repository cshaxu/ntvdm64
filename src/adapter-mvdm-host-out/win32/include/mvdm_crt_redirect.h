#ifndef MVDM_CRT_REDIRECT_H
#define MVDM_CRT_REDIRECT_H

/*
 * The selected NT4 MVDM host intentionally intercepts these CRT spellings:
 * `getenv` reads its CPU-environment list, while the stdio and `_assert`
 * names deliver source-era diagnostic and input behavior. Modern UCRT exports the same
 * symbols.  Apply this header only while compiling original SoftPC host TUs,
 * so their source-level call graph remains intact without interposing on the
 * product process or the UCRT itself.
 */
#define getenv  mvdm_host_getenv
#define fprintf mvdm_host_fprintf
#define _assert mvdm_host_assert

/* Win32/x86 links the historical fprt `printf` family as the original
 * process-wide provider.  Win64 UCRT owns that family, so only the x64
 * original host translation units require private binding names. */
#ifdef _WIN64
#define printf  mvdm_host_printf
#define fgets   mvdm_host_fgets
#define gets    mvdm_host_gets
#define puts    mvdm_host_puts
#define fwrite  mvdm_host_fwrite
#endif

#endif
