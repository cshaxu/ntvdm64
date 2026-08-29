# M0 T310 S8 P4 — SoftPC copy engine native-alignment closure

## Source contract

The original `copy_fnc.c` `fwd_word_fill` routine uses a 32-bit word pattern
and a locally aligned four-byte pointer to fill private SoftPC backing memory.
In the NT4 x86 build, `unsigned int` served both as the data unit and as a
native pointer-sized alignment carrier.  That second use truncates an x64
backing address before the alignment mask.

## Recovery

`MVDM-HOST-DIV-064` retains the original data width and fill ordering:

- the fill datum remains `unsigned int` (four bytes);
- only the two private alignment calculations use `uintptr_t`;
- the locally derived span uses `size_t`; and
- the original in-source word-fill loop is retained instead of using a new
  adapter, mapping identity or substitute controller.

The changed values are private native addresses and byte counts, not guest
addresses, host handles or external identities.  The session mapping manager
is therefore intentionally not involved.

## Verification

Formal MSVC `/MT` Ninja compiled the original source object on both
architectures outside the sandbox:

- `build/M0-T310/S8/p1-machine-source/x64/copy-fnc-x64.log`
- `build/M0-T310/S8/p1-machine-source/x86/copy-fnc-x86.log`

Neither log contains a `copy_fnc.c` occurrence of `C4311`, `C4312`, `C4047`,
`C4057`, `C4133`, `C4152`, `C4244` or `C4267`.  This closes the private
native-address ABI issue only; controller-level RAM and video runtime evidence
remain governed by the rest of S8.
