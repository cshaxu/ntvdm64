# Modern Toolchain Source Overlay Audit - 2026-08-07

## Scope And Method

This audit distinguishes the managed OpenNT source baseline from generated
research stages. It compared every `upstream-copy` entry in
`src/opennt/SOURCE-MANIFEST.json` to its recorded SHA-256 and examined the
active stage scripts and retained stage manifests for JWasm, UASM, OpenWatcom,
and host-compatibility rewrites.

## Finding: The Managed OpenNT Baseline Is Intact

All 2,392 files classified as `upstream-copy` match their recorded hash. There
are zero missing or hash-mismatched entries. Modern-toolchain changes are not
silently written over those original-source paths.

`src/opennt` also contains explicitly classified local inputs, shims, fixtures,
and 16 files below `overlay/`. They are selected by explicit CMake paths. They
are not upstream copies and must be audited separately as host/research
overlays; this audit found no evidence that they are hidden JWasm or UASM guest
assembly rewrites.

## Finding: Legacy Guest Stage Overlays Are The Cleanup Priority

| Area | Active script | Rewrite categories | Decision |
| --- | --- | ---: | --- |
| NTDOS | `stage-opennt-ntdos.ps1` | 10 | Retire from default use. Pristine MASM386 already accepts all 38 modules. |
| NTIO | `stage-opennt-ntio.ps1` | 8 | Retire from default use. Pristine MASM386 already accepts all 12 modules. |
| KEYBOARD | `stage-opennt-keyboard.ps1` | 2 | Retire pending per-module pristine MASM386 audit. |
| SETVER | `stage-opennt-setver.ps1` | 4 logical categories | Retire pending MASM386 audit and separate Microsoft C16 ABI recovery. |
| BUILDMSG | `stage-opennt-buildmsg.ps1` | host-only port | Retain as a host-tool adaptation, not a guest-source overlay. |

The NTDOS overlay changes include renamed `INVOKE`, structure/type conflict
workarounds, explicit `IF NOT INSTALLED` rewrites, substituted long-jump macro
wrappers, an external-declaration rewrite, and disabled IF2 layout assertions.
The NTIO overlay changes include legacy field/segment syntax rewrites, renamed
`INVOKE`, declaration joining, and disabled layout checks. These are all
modern-assembler accommodation, not required by the now-proven MASM386 path.

The retained generated directories show the scale of historical contamination:
the final NTDOS compatibility stages carry 10 transformations, NTIO managed
stages carry 8, and older HIMEM, KB16, KEYBOARD, MEM, and SETVER stages also
record toolchain-specific edits. They remain evidence only and are not deleted
because they explain past results.

## Immediate Cleanup Rules

1. The four active guest-overlay scripts require an explicit
   `-AllowRetiredCompatibilityOverlay` switch. Default use fails before copying
   or editing any source.
2. New guest stages copy only manifested upstream inputs and must record an
   empty `GuestSourceTransforms` list.
3. A host ABI adaptation belongs outside the guest source tree, or in a named
   host-only stage such as BUILDMSG. It cannot be described as an original guest
   build.
4. Do not delete legacy stages while their output is still cited. Mark them
   `legacy-compatibility-only` and link successor pristine evidence instead.

## Next Cleanup Waves

1. Replace the KEYBOARD and SETVER stage entry points with pristine MASM386
   auditors. For SETVER, record the C16 ABI gap separately from assembly.
2. Audit KB16, legacy HIMEM stages, and MEM's OpenWatcom C16 declaration edit;
   retain only a source-backed correction or a non-guest host adapter.
3. Audit the 16 explicit `src/opennt/overlay/` files by target and caller. Each
   must become either a source-backed host compatibility layer, a bounded
   research fixture, or a rejected overlay. This is a separate task from
   removing assembler-specific guest transforms.
