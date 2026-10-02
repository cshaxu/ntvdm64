# T423 S22 native EDIT colour audit

## Question

Why does `run16 edit.exe` appear monochrome in the NTKVM Window route: does
colour disappear in NTW32 capture/frame transport/rendering, or has the target
already chosen a monochrome terminal presentation?

## Inputs

- Published S21 baseline `9610f9e19`; no production source was changed.
- `%SystemRoot%\System32\edit.exe`, file version 1.2.1.0.
- New checked-in probe:
  `tests/observation/ntw32_native_colour_probe.c`.
- Production NTW32 sources: `console_state.c`, `text_frame.c` and
  `presentation.c`; NTKVM `window_frame.c`.

## Procedure

The x86 probe creates the same class of private ordinary Console as NTW32,
launches the system EDIT there, waits only for the test target to draw, attaches
read-only to that Console, and records its `CHAR_INFO` attributes and
`CONSOLE_SCREEN_BUFFER_INFOEX::ColorTable`.  It then invokes the production
`ntw32_text_frame_pack` on those exact captured cells and records the resulting
frame attributes and RGB palette.  The test repeats with VT output enabled and
with a terminal-like environment (`TERM`, `COLORTERM`, `WT_SESSION`).  It does
not modify guest media, product binaries, the registry, or the target UI.

The probe was compiled `/MT` x86 under
`build/M0-T423/S22/colour-probe` and run as:

```text
ntw32-native-colour-probe.exe edit-colour-observed.txt
```

The existing production packer fixture was also run:

```text
ntw32-text-frame-test.exe ntw32-text-frame-test.log
```

## Observations

Both ordinary-Console variants completed successfully.  Each observed a
53x14 page with output mode `0x0000000f` and exactly these nonzero low-nibble
attribute counts:

| Attribute | Captured cells | NTW32 packed frame |
| --- | ---: | ---: |
| 7 | 636 | 636 |
| 15 | 106 | 106 |
| all other 0--15 values | 0 | 0 |

The native Console colour table was not monochrome: it contained, for example,
blue/orange/green entries as well as greys.  `ntw32_text_frame_pack` converted
the Windows `COLORREF` layout to the protocol's `0x00RRGGBB` layout correctly;
the 16 packed RGB values match the native table after that byte-order conversion.
`ntw32-text-frame-test` also completed `130` checks with `0` failures.

The probe also issued the same OSC 4 colour-table query shape before launching
EDIT, flushed unrelated input, and accepted only a matching `ESC ] 4 ; 0`
response.  Both variants reported `query_reply=0`; enabling VT output and
terminal-looking environment variables did not make an ordinary Console answer
on `CONIN$`.

The current [Microsoft Edit Windows source](https://github.com/microsoft/edit/blob/main/crates/edit/src/sys/windows.rs)
enables virtual-terminal input/output; its documented
[terminal setup path](https://github.com/microsoft/edit/issues/30) sends OSC
4/10/11 colour queries to the host terminal and uses the answers for indexed
colour setup.  The ordinary hidden Console is a Console API surface, not a
bidirectional RGB terminal capable of replying to those queries.  Supplying
terminal-looking environment variables does not create those replies.

## Interpretation and confidence

**High confidence:** this is not an NTKVM rendering loss, a wrong copied
palette, or an NTW32 frame conversion error.  On the current native-worker
backend, Edit itself has already emitted only attributes 7 and 15 before
NTW32 reads the screen.  The Window renderer then correctly displays those
two grey/white selections.

The source-level reason is terminal-capability negotiation: modern Edit is a
VT/RGB terminal application, whereas the retained NTW32 backend intentionally
uses a hidden ordinary Console.  Enabling VT mode alone is insufficient; it
must receive the terminal's query responses.  A real repair that preserves
Edit's full RGB theme would therefore require an actual bidirectional terminal
endpoint (for example a ConPTY-style backend) or a complete VT query/response
emulator.  Either changes the currently approved hidden-Console architecture;
it is not a safe local renderer patch.

## Follow-up

### Historical ConPTY correction

The first S22 conclusion must not be read as evidence that a helper-free
ConPTY backend is impossible.  The repository's S9 P5 delivery `3b40345f8`
is named "replace helper backend with shared ConPTY".  Its retained
[S9 migration ledger](m0-t423-s9-conpty-migration.md)
records removal of the 330-line helper pair, one retained ConPTY across
native-to-DOS-to-native use, authenticated nested paths, and a native terminal
model that owns parser state, screen snapshots and terminal replies.  The
formal tests covered fragmented VT, RGB attributes, alternate screen, resize
and a cursor-position reply.  Thus a single long-lived `ntw32.exe` can own
the pseudoconsole, its synchronous input/output pipes, parser and target
launches; no extra helper process is an API requirement.

That historical code does not yet prove the specific modern Edit colour path:
its retained reply assertion is CPR (`ESC[6n` to `ESC[row;colR`), while this
target asks OSC palette/default-colour queries.  A correct revival must retain
the single-owner/no-helper topology, recover the previously proven terminal
state and handoff mechanisms, then add only the target-independent OSC 4/10/11
reply contract and prove it against real Edit.  It must not emulate colour by
program-name special case.

## Closure disposition

The owner subsequently approved the S23--S28 worker-control sequence and
authorized automatic sequential admission.  S22 therefore closes with this
bounded result: the ordinary hidden-Console product path preserves exactly the
attributes and palette supplied by the target, and modern EDIT elects its
monochrome fallback because that endpoint cannot answer its generic OSC
4/10/11 terminal queries.  No target-name special case, static palette,
forced redraw or renderer patch is admitted.  Restoring a bidirectional
terminal backend is a separate architecture decision, not an S22 colour
patch.

No production source changed for this disposition.  The existing S22 probe,
the 130-check production packer fixture, and the already-published S22
management-projection delivery are the applicable verification; no new
unverified executable is published.
