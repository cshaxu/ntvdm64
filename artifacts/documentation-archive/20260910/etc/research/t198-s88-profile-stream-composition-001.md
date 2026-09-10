# M0 T198 S88: Immutable Stream Profile to BOP-5F Composition

## Result

The first CLI stream-capable profile is now the sole pair
`ntdos64-byob-profile-v7` / `nt4-en-us-cli-stream-v7`.  It requires the exact
declaration:

```json
"guest_display_state": "stream-io-v1"
```

The parser projects that declaration as the closed value
`BYOB_GUEST_DISPLAY_STATE_STREAM_IO_V1 = 2`.  v1--v6 profiles retain zero and
do not acquire a display default.  No command-line `--display` switch, host
console behavior, or display backend was added.

## Composition

`bx_ntvdm_boot_namespace_composition_v1` is now ABI version 2 and owns the
selected value as profile composition data.  It calls the S87 source-derived
transaction only for the exact real-mode `C4 C4 5F` / `AX=BEEF` form and only
when the selected value is `2`.

The ordering is fixed:

```text
validated v7 JSON -> byob_profile_selection.guest_display_state = 2
  -> bx-vdm composition v2
  -> S87 checked read DS:SI+34
  -> checked one-byte ordinary-RAM write 2
  -> original CF / RIP+3 result
```

When an explicitly selected `BOP 5F` cannot complete the checked transaction,
the composition declines it.  It does not take the old CF continuation without
publishing the selected state.  Unselected/legacy profiles retain the previous
continuation-only behavior.

## Verification

| Check | Result |
| --- | --- |
| `tests/runner/byob_profile_test.c` | MSVC x64 `/MT` test accepts v7 only with `stream-io-v1`, projects `2`, retains zero for v6, and rejects a mutated value. |
| `tests/bx-vdm/bx_ntvdm_boot_namespace_composition_v1_test.c` | Full current composition source closure executes selected BOP `5F`, reads the synthetic K.SYS offset, writes `2`, and returns CF/`RIP+3`. It separately rejects a selected transaction whose target is outside the one-MiB aperture. |
| `tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1` | Its source list now matches the registered current composition membership rather than the pre-provider S16 subset. r4 source-builds, links and runs the complete x64 `/MT` fixture. |

Evidence artifacts:

- `artifacts/build/t198-s88-profile-stream-r1/profile-compile.log`
- `artifacts/build/t198-s88-profile-stream-r1/profile-run.log`
- `artifacts/build/t198-s88-profile-composition-r4/compile.log`
- `artifacts/build/t198-s88-profile-composition-r4/link.log`
- `artifacts/build/t198-s88-profile-composition-r4/run.log`

The refreshed composition fixture also corrected an obsolete assertion: the
current composition has already admitted `BOP 15h/AH=88h` and its fixed
`0x0C00 KiB` result, as recorded by S23.  The fixture now asserts that
existing behavior rather than contradicting the current composition.

## Limitations And Transfer

This is an adapter/CLI composition proof, not an original-guest continuity
claim.  The current CPU5 NTIO fixture still links an older adapter object set,
so it cannot demonstrate that the actual reached guest BOP-`5F` performs this
new transaction.  The next S must rebuild that native fixture with the updated
composition and observe the original guest path.  It must not add output,
BOP `42`, ROM/VGA/PIC/device behavior, a host display API, or any Bochs change.
