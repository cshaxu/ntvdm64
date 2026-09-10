# M0 T354 S3 — configuration-to-`commnd` cohort attribution

## Source facts

`cmdGetConfigSys` calls original `ExpandConfigFiles(TRUE)` and passes its
temporary filename into original NTDOS.  The sole project divergence in
`cmdconf.c` remains the selected package SystemRoot binding; the original
`GetShortPathNameA` normalization, configuration copying and `shell=` source
body remain in place.

The retained original code writes this value:

```text
shell=<SystemRoot>\System32\command.com /p <SystemRoot>\system32
```

The one permitted S2 run left the original temporary file behind.  Its exact
tail confirms that the expected line was generated, with the selected stage
root:

```text
shell=O:\repos.hobby\ntvdm64\build\T354S2\mvdm\System32\command.com /p O:\repos.hobby\ntvdm64\build\T354S2\mvdm\system32
```

Original `sysconf.asm::trys/storeshell` copies the `shell=` value into
`sysinit2.asm::commnd`.  That storage is exactly 64 bytes:

```asm
commnd db "\\COMMAND.COM",0
       db 51 dup (0)
```

The package root observed by the fixed observer is 40 characters; neither
volume supplies a shorter 8.3 form.  The generated `shell=` value alone is
`2 * 40 + 34 = 114` characters before its end of line, while `commnd` can hold
at most 63 characters plus its terminator.  The original host code checks only
that `achSysRoot` itself fits in 64 bytes; it does not establish the stricter
combined `commnd` bound.  The resulting input overflows the original firmware
buffer and is therefore not a valid source-compatible package SystemRoot.

This accounts for the S2 source result: later startup still uses the untouched
default `C:\COMMAND.COM`.  It is not evidence of an incomplete `50:12`
provider, nor permission to redirect that provider.

## Selected recovery boundary

The next whole cohort is a **session-owned short MVDM SystemRoot projection**:
it must give retained original `cmdconf.c` and retained NTDOS parser a
DOS-reachable root whose full generated `shell=` value fits the original
64-byte `commnd` contract.  It must resolve the projected root to the immutable
adjacent package media without writing `COMMAND.COM` to real host C, without
overriding ordinary Direct host-drive semantics, and without adding a BOP or
DEM special case.

The exact projection spelling, collision policy and adapter owner are not
selected in S3; they affect the host namespace contract and require separate
admission.  A long arbitrary executable path cannot be treated as a valid
replacement for historical `SystemRoot` merely because its root substring
fits `achSysRoot`.
