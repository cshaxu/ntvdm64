# Video interface, BIOS dispatch and EGA switch: shard 1

Three frozen unchanged original source paths remain pending: selected source,
map presence and neighboring tests do not prove full original behavior.
Snapshot identities: worker `ntvdm.exe` SHA-256
`D3437091D920A74FF72FDC8ED023C6207642682A7739C45751E4F03A8C1D53E5`,
link map SHA-256
`641E5C33430445ECC73885176250194BE936549654E14864074BD0B7D92D3CE2`.
`ninja -t commands` for the three object targets selects NTVDM, PROD,
V7VGA and the common compatibility includes, without a `/DX86GFX` switch;
`host/inc/host_def.h` defines `EGG` and `VGG`. This is a command-line check,
not a full fresh preprocessor dump of every header.

* `base/video/gvi.c`: `bios/reset.c` obtains `C_GFX_ADAPTER` from configuration
  and calls `gvi_init`; `gvi_init` conditionally terminates the old adapter on
  soft reset, chooses a supported adapter or CGA fallback, initializes it and
  invokes host adapter/clear logic under its build condition. `gvi_term`
  dispatches teardown separately. `_gvi_init` is present in the selected
  map from `original-softpc-video:gvi.obj`. This establishes concrete
  selection and reset source path, not actual exercise of every adapter,
  fallback, or repeated reset. Obtain configured profile and live reset
  tracing plus the exact adapter/mode tests before any complete classification.
* `base/video/video_io.c`: the selected `bios/bios.c` BOP dispatch table maps
  BOP 10 to `video_io`; the function bounds checks AH against EGA_FUNC_SIZE,
  calls IDLE_video and dispatches `video_func[AH]`; in the NTVDM non-X86GFX
  branch it can disable stream I/O for operations other than 0Eh and 13FFh.
  `bios/bootstra.c` also directly invokes `bop(BIOS_VIDEO_IO)` for output;
  `bios/reset.c` sets IVT 10h to a firmware entry under conditions and
  separately invokes BOP for output. `_video_io` is present in the map from
  `original-softpc-video:video_io.obj`. This supports entry and table identity,
  not that every INT10 application reaches BOP 10: EGA maps BOP 42 separately
  and guest/firmware hooks can redirect INT10. Verify actual guest firmware,
  runtime routing, invalid AH and stream state before declaring full coverage.
* `base/video/ega_dmmy.c`: conditionally compiled original dummy returns zero
  for `get_ega_switch_setting`; map attributes the symbol to
  `original-softpc-video:ega_dmmy.obj`, and `objdump -t` confirms the selected
  object defines it. `ega_prts.c::ega_ipstat0_inb` sets the switch-sense bit
  from that return before exposing the status register. A separate historical
  `host/src/stubs.c` also defines the same name, but the selected link map
  identifies the provider in this snapshot. Zero here is the original dummy
  implementation, not independent proof of complete physical EGA DIP switch
  emulation; ascertain selected port mapping, value/variant behavior and
  relevant configuration before judging semantic completeness.

`tests/observation/verify-dos-video-source-contract.ps1` has static checks for
Video7 painter geometry; `tests/mvdm-host/video7_packed_painter_test.c`
contains assertions for the packed painter; and
`tests/observation/verify-video7-mode-flags.ps1` constructs a separate DOS
BIOS mode probe. T460 S4 `docs/etc/evidence/m0-t460-s3-addon-tool-audit.md`
records the source-contract checks. These do not by themselves exercise the
three above individual paths or prove all BIOS calls and adapter lifecycle.
No fresh runtime regression was run for this audit. Retain all three pending.

## Default VGA versus conditional EGA port handler

On this selected build, `host/src/config.c::config_inquire(C_GFX_ADAPTER)`
returns `VGA` unconditionally; the corresponding OpenNT source has the same
branch, although the entire `config.c` files are not identical. `bios/reset.c`
passes that value to `gvi_init`, whose `VGA` case calls `vga_init`.
`vga_prts.c::vga_init` registers `vga_ipstat0_inb` at
`EGA_IPSTAT0_REG`, which `base/inc/ios.h` defines as port 0x3C2. The VGA
handler returns its own status-register byte; it does **not** call the EGA
switch-sense dummy. It initializes that byte to `0x70`. By contrast,
`ega_init` registers `ega_ipstat0_inb` at the same port when the EGA adapter
is initialized; that EGA handler obtains the switch-sense bit from the dummy.
Do not conflate the two different status register initializers or bit layouts.
The worker map contains both `_vga_ipstat0_inb` from `vga_prts.obj` and the
local `_ega_ipstat0_inb` from `ega_prts.obj`, while `_config_inquire` comes
from `config.obj`; link-map presence alone does not show which branch ran.

`ega_dmmy.c` selects the dummy for `EGG` and either non-NTVDM or NTVDM
without `X86GFX`; the selected worker command has `/DNTVDM` and lacks
`/DX86GFX`; selected `host_def.h` defines EGG. Its SHA-256
`4F7AAA63FB26F4FB556EA90183149CEE7DFD048B954922AE335E17AE3CAFC40D`
exactly matches the OpenNT comparison file. The observed source/map identity
therefore establishes an original selected helper, not a new source change.
It does not establish EGA guest support, a configured EGA execution, or an
actual observable default-VGA call to the dummy. The fixed VGA selection is
source and selected-link evidence, not an independently instrumented runtime
trace. No EGA guest test or upstream runtime comparison was performed, so
retain pending rather than claim either complete equivalence or a new defect.

## Isolated port probe attempt, not a passing runtime witness

A separately authored `video-port-probe.asm` assembles to DOS `VPORT.COM`,
performs mode 3, emits BIOS INT10 teletype `B`, reads port 0x3C2 and prints
its value with `VPORT=`. This fixture does not alter the installed guest.
Its COM SHA-256 is
`A44A7C7D0FD770DC2CF9FC16914C1DDEBC065800C1FD732B19E8D3D41388B30E`.
The first archived runner, `video-port-run-first.ps1`, used S9 package-r10,
the standard short physical test stage, private desktop observer and
`COMMAND.COM /c VPORT.COM`. On this attempt observer reported
`result=timeout` after its 35-second deadline and an empty screen, despite
the wrapper process returning zero; this is **not** a successful port, BIOS,
or product-exit test and does not locate the failure. The owned isolated
stage and its processes were cleaned.

A second runner, `video-port-run-prepared.ps1`, instead uses the earlier
successful interactive COMMAND prompt and screen-gated input mechanism. Its
preflight detected other project processes and declined to start, without
modifying or terminating them. No second runtime result exists. Do not infer
port values or handler reachability from either attempt. Later continuation
must reconstruct the known-good test route, verify process ownership, and use
a fresh result path.
