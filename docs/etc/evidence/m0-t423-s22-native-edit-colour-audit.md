# T423 S22 native EDIT colour audit

## Question

Why does `run16 edit.exe` appear monochrome in the NTKVM Window route: does
colour disappear in NTCON capture/frame transport/rendering, or has the target
already chosen a monochrome terminal presentation?

## Inputs

- Published S21 baseline `9610f9e19`; no production source was changed.
- `%SystemRoot%\System32\edit.exe`, file version 1.2.1.0.
- New checked-in probe:
  `tests/observation/ntcon_native_colour_probe.c`.
- Production NTCON sources: `console_state.c`, `text_frame.c` and
  `presentation.c`; NTKVM `window_frame.c`.

## Procedure

The x86 probe creates the same class of private ordinary Console as NTCON,
launches the system EDIT there, waits only for the test target to draw, attaches
read-only to that Console, and records its `CHAR_INFO` attributes and
`CONSOLE_SCREEN_BUFFER_INFOEX::ColorTable`.  It then invokes the production
`ntcon_text_frame_pack` on those exact captured cells and records the resulting
frame attributes and RGB palette.  The test repeats with VT output enabled and
with a terminal-like environment (`TERM`, `COLORTERM`, `WT_SESSION`).  It does
not modify guest media, product binaries, the registry, or the target UI.

The probe was compiled `/MT` x86 under
`build/M0-T423/S22/colour-probe` and run as:

```text
ntcon-native-colour-probe.exe edit-colour-observed.txt
```

The existing production packer fixture was also run:

```text
ntcon-text-frame-test.exe ntcon-text-frame-test.log
```

## Observations

Both ordinary-Console variants completed successfully.  Each observed a
53x14 page with output mode `0x0000000f` and exactly these nonzero low-nibble
attribute counts:

| Attribute | Captured cells | NTCON packed frame |
| --- | ---: | ---: |
| 7 | 636 | 636 |
| 15 | 106 | 106 |
| all other 0--15 values | 0 | 0 |

The native Console colour table was not monochrome: it contained, for example,
blue/orange/green entries as well as greys.  `ntcon_text_frame_pack` converted
the Windows `COLORREF` layout to the protocol's `0x00RRGGBB` layout correctly;
the 16 packed RGB values match the native table after that byte-order conversion.
`ntcon-text-frame-test` also completed `130` checks with `0` failures.

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
palette, or an NTCON frame conversion error.  On the current native-worker
backend, Edit itself has already emitted only attributes 7 and 15 before
NTCON reads the screen.  The Window renderer then correctly displays those
two grey/white selections.

The source-level reason is terminal-capability negotiation: modern Edit is a
VT/RGB terminal application, whereas the retained NTCON backend intentionally
uses a hidden ordinary Console.  Enabling VT mode alone is insufficient; it
must receive the terminal's query responses.  A real repair that preserves
Edit's full RGB theme would therefore require an actual bidirectional terminal
endpoint (for example a ConPTY-style backend) or a complete VT query/response
emulator.  Either changes the currently approved hidden-Console architecture;
it is not a safe local renderer patch.

## Follow-up

S22 remains active pending owner direction: accept ordinary-Console native
colour degradation as the backend limitation, or approve a distinct terminal
backend/replanning.  No production executable has been rebuilt or published.
