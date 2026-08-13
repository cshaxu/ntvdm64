# M0 T198 S67: Printer-Disabled `BOP 17` Witness

## Result

The active top-level composition now calls the exact existing `BOP 17`
printer-disabled provider.  It preserves the original non-`PRINTER` SoftPC
profile: no LPT I/O is performed and guest state is resumed after three bytes
without a manufactured success or error code.

## Clean source-built witness

The complete MSVC x64 `/MT` run at
`artifacts/build/t198-s67-printer-disabled-r1` advances beyond the reached
LPT0 request to:

```text
next=5e:e8 terminal=1:8dc8:08b1
```

`5E` is a top-level OpenNT notification selector; `E8` is following guest
code, not a service identifier.

## Next frontier

OpenNT identifies `5E` as its 16-bit-to-32-bit notification plane.  The
current tree has an exact config-done candidate restricted to `AL=0`, but
notification semantics can carry host-visible effects.  The next task must
audit the reached input and continuation before composing anything.

