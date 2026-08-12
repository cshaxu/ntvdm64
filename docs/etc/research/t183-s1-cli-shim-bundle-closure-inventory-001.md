# T183 S1 CLI shim/bundle closure inventory 001

Date: 2026-08-12  
Packet: M0 T183 S1  
Disposition: complete; the current shim/bundle closure is finite and suitable for S2.

## Process-boundary source contract

`src/cli/ntdos64_bochs_engine.c` is the existing CLI-owned process boundary.
It accepts only the runner's fixed eight-argument ABI, derives the bundle root
from a fixed `ntdos64-bochs.exe` name, validates a five-line fixed-order
manifest, copies three verified ROM files into a private run directory, writes
the frozen native configuration, and invokes the engine only as `-q -f
bochsrc`. The native parser receives neither BYOB profile/root nor launch-plan
arguments.

The source creates no Bochs/adapter link edge. Its fixed bundle members are:

| Member | Current source role | SHA-256 |
| --- | --- | --- |
| `ntdos64-bochs.exe` | T182 linked engine, copied under the fixed bundle name | `B4AAA70645DE2061B34B2D072D852419290BAD4C7FC710DF95C8380F7510898D` |
| `ROM/BIOS-bochs-latest` | retained native BIOS input | `D974DE50C94D76445123FD1AC886612351BED7CC5E3C8D07C556814DA72BEBD6` |
| `ROM/VGABIOS-lgpl-latest` | retained native VGA BIOS input | `CD9FDD6A789DCD22B8A6B3B152788D43238DE49CCE674CFF57BDEB94580246C6` |
| `ROM/ntdos64-post-handoff.rom` | retained deferred POST option ROM | `E4CA4AE5190A15ED741D1DFED5A58533A2B6926A40302D839FB714F9FC673FFB` |

The source's manifest contains one schema line followed by exactly the four
corresponding uppercase SHA-256 values. It is a content identity record, not
a configuration language.

## Current build closure

`CMakeLists.txt` declares `ntdos64-run` from the current v5 runner and its
profile/identity/target/declaration/plan sources, and separately declares
`ntdos64-bochs-engine` from the one shim source, linked only to `bcrypt`.
The existing focused policy test proves fixed native arguments, ROM validation,
BYOB environment inheritance, EOF stdin, and rejection of trailing shim
directives. It uses a fake engine, so it is source/process evidence rather
than a real engine launch.

The T181 MinGW build produced `ntdos64-run.exe` but did not request the shim
target. The linked T182 engine is present, and the three exact ROM inputs are
available from the retained native fixture. No missing source, compiler,
firmware, or adapter/Bochs dependency remains.

## S2 decision

S2 may generate one fresh CLI build directory with the established MinGW
configuration, build only `ntdos64-run`, `ntdos64-bochs-engine`, and the two
existing probe binaries needed by the policy test, then assemble the exact
four-member bundle with a generated fixed-order hash manifest. It must not run
the linked engine or alter the shim's fixed configuration.
