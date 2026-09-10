# M0 T282 S7 — original COMMAND initialization evidence

## Question

Can the first initialization slice of the original COMMAND package run as
unaltered source on both host architectures?

## Inputs

- Original and mirrored `dos/command/cmd.c`, SHA-256
  `9d0a773fcc8d7765b0fe3fe3750b5c9a29a596362f78a221161318fd83cc0262`.
- Original and mirrored `dos/command/cmddata.c`, SHA-256
  `937c134e46b5c714b9106d85fd0250b88fca5958e69a1e60fdeab558d4f4f8d2`.
- Existing OpenNT declaration carriers and public `GetSystemDirectory`.

## Procedure

1. Generate `build/M0-T282/S7/{x64,x86}/build.ninja` through
   `New-T282OriginalCommandInitializationNinja.ps1`.
2. Compile, link and execute `command_initialization_fixture.exe` with the
   matching MSVC `/MT` architecture.
3. Check source identities and verify `CMDInit` against the public system
   directory result while observing the original `cmddata.c` initial values.

## Observations

Both architecture fixtures completed:

```text
x64: PASS: original COMMAND initialization composition
x86: PASS: original COMMAND initialization composition
```

Each compile emits the same three warnings from exact original `cmd.c`:
`C4131` for the K&R declaration and two `C4100` warnings for unused `argc` and
`argv`. No mirror code was changed to suppress them.

## Interpretation

`cmd.c` and `cmddata.c` are directly composable original source. The original
initial globals and `CMDInit` system-drive update execute on x86 and x64 using
only a public Windows API. This is a closed initialization slice, not a claim
that dispatch, acquisition, process, console, PIF, broker or guest lifecycle
has been enabled.

## Follow-up

The next T282 slice must select a complete adjacent COMMAND owner boundary;
it must not infer one from a guest trace. Remaining original paths retain the
package tracker dispositions.
