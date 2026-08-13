# M0 T198 S79: Machine-Image Producer Audit

## Question

Does the current approved OpenNT and pinned Bochs 2.6 checkout provide a
reproducible, source-authorized input for the missing reached C000 ROM/initial
IVT image?

## Inputs

- S78's absent-input map and the source-policy rule.
- `src/opennt` source/build files and `artifacts` input inventory.
- Pinned Bochs checkout
  `O:\repos.external\bochs-2.6-compat\bochs-2.6`, including `bios/`,
  `iodev/vgacore.cc`, and `iodev/biosdev.cc`.
- Current adopted `src/bx-core` and `src/bx-mantle` membership.

## Procedure

1. Search OpenNT source, manifests and artifacts for the named historical ROM
   resources and an output producer.
2. Inspect the pinned Bochs BIOS/VGA source and its build declarations.
3. Compare any candidate's address and device prerequisites with the adopted
   no-device core/mantle membership.
4. Treat prebuilt images as evidence only; do not copy, load or hash-admit
   them into this repository.

## Observations

### OpenNT

The current source names `bios1.rom`, `bios2.rom`/`bios4.rom`, EGA and VGA ROM
resources but contains neither their byte payloads nor a make/source producer.
`rom.c` receives them from `host_read_resource`; a resource name is not byte
provenance. The current approved OpenNT checkout therefore supplies no
reproducible C000 image.

### Pinned Bochs system BIOS

The external Bochs `bios/Makefile` has a source route for
`BIOS-bochs-latest`: `rombios.c`, `rombios32.c`, `rombios32start.S` and an old
tool chain (`gcc -m32`, `bcc`, `as86`, linker and objcopy). This system-BIOS
producer targets the F000 BIOS region, not the reached C000 video-ROM range;
the required legacy producer tools are not available in the current
environment. It is consequently neither a direct C000 answer nor an admitted
build input for this packet.

### Pinned Bochs VGA image

The external checkout contains prebuilt `VGABIOS-lgpl-*` files, but its README
states that the source is obtained as a separate archive. The checkout has no
VGA BIOS C/assembly/linker/make producer. Therefore the included VGABIOS file
is a prebuilt research artifact, not source-built payload provenance.

Moreover, Bochs loads that image through `bx_vgacore_c::init`, which also
creates VGA state, maps `A0000-BFFFF`, registers memory/I/O handlers, and
depends on SIM/device lifecycle. `biosdev.cc` separately registers the BIOS
diagnostic port set. Those iodev sources are not in the adopted minimal
`bx-core`/`bx-mantle` membership. Loading the prebuilt image alone would be
unsound; importing the complete VGA product slice would be a new feature
admission, not a mechanical image copy.

## Disposition

No candidate is an approved, reproducible C000 machine-image input in the
current checkout:

| Candidate | Source-built | C000 applicable | Minimal-machine compatible | Disposition |
| --- | --- | --- | --- | --- |
| OpenNT named ROM resources | No | Intended historically | Input absent | Reject |
| Bochs `BIOS-bochs-latest` producer | Potentially, with unavailable legacy tools | No, F000 system BIOS | Not evaluated further | Reject for S79 |
| Bochs `VGABIOS-lgpl-*` | No source producer in pinned checkout | Yes | Requires excluded VGA/device lifecycle | Reject |

This is an unavailable-input conclusion, not permission to synthesize a ROM,
load an external binary, enable a VGA device, or add a BOP provider.

## Interpretation And Confidence

High confidence: S79 finds no current source-authorized C000 payload route.
High confidence: the prebuilt Bochs VGA image has hardware/lifecycle
dependencies beyond the adopted no-device machine. Medium confidence: the
reached transfer is the historical fullscreen/native-INT10 branch; S80 must
establish that branch from the actual OpenNT initialization contract before a
profile decision is considered.

## Follow-up

S80 audits the original BOP `5F` keyboard/video initialization table,
`use_host_int10`, and saved INT 10 vector. It must distinguish a missing
source-derived CLI display-mode composition from a true requirement to admit a
Bochs VGA machine slice.
