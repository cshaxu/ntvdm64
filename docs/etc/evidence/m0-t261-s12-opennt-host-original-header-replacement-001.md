# M0 T261 S12 original-header replacement evidence

## Question

Can the reached OpenNT-host headers be restored under their original names and
contents without retaining project-authored `*_compat.h` declarations?

## Inputs

- `refs/opennt/base/mvdm/softpc.new/host/inc/error.h`
- `refs/opennt/base/mvdm/vdmredir/vrputil.h`
- `refs/opennt/base/mvdm/inc/vrnmpipe.h`
- `refs/opennt/base/mvdm/inc/oemuni.h`
- reached source and consumer paths listed in the S12 packet.

## Procedure

1. Re-root each available historical header below `src/opennt-host` without
   editing it.
2. Replace all reached consumers of `nt_error_compat.h`, `vrputil_compat.h`,
   and `vrnmpipe_compat.h` with the original header or the actual adapter
   owner of non-original state.
3. Keep the VDMREDIR static-link selection outside the original header and
   register it as `HOST-DIV-024` / `BOP-DIV-087`.
4. Compile the directly affected host, adapter and BOP objects through the
   formal Ninja graph; use its generated archive/link response files for the
   focused BOP59 and VDMREDIR fixture link/run.

## Observations

- The following full-file SHA-256 pairs match exactly:
  - `error.h`: `40953FBB22A9C6A5BFF875414E4DD6822340EC4A2501EDEC79415573FEA16AB3`
  - `vrputil.h`: `39F3E56F1F344D6F5EF17DB3AD9F9451BE7E860259A1779E4EB3F997C53B3C84`
  - `vrnmpipe.h`: `06E77E0E94FF590B0AEF18577F53EDA9987C6DC2331AC6E8C185CFD10FE79DA6`
  - `oemuni.h`: `26C8509048272DE86ED1B93F5AF837434E90D8ECBC8A878D915C419BE2D2`
- The exact historical `error.h` contains two pre-existing trailing-tab
  lines (49 and 117). They remain unchanged to preserve the hash above;
  the repository whitespace gate was consequently re-run with only that
  byte-identical imported file excluded, and all project-authored changes pass.
- A negative scan finds no production or test include of the three removed
  `*_compat.h` files and no `*_compat.h` below `src/opennt-host`.
- `VrTerminateNamedPipes` now exposes the original `WORD DosPdb` ABI.  The
  registered static-session cleanup remains a source-derived body divergence,
  not a changed header declaration.
- The direct-access prompt choice/count state now belongs to
  `adapter-win32/facade/opennt_error_dialog_facade.*`; the original
  `host_direct_access_error` remains `VOID` in `nt_error.c` and `error.h`.
- The original `vrnmpipe.h` has no include guard.  Each reached translation
  unit now includes it once, after the original prerequisite Win32 types.
- The focused BOP59 fixture links and passes (`exit=0`) after the session
  resettable direct-access category bitset moved from the mirror body to its
  actual `adapter-win32` owner.
- A fresh, disposable formal Ninja root `build/M0-T261-S12/002` completed all
  312 required compile/archive/link edges for the focused BOP59 and
  Redirector fixtures. `t237-s5-bop-nosupport-fixture.exe` returns `0`.
- The Redirector fixture initially exposed that its direct named-pipe client
  was not opened with `FILE_FLAG_OVERLAPPED`, even though the retained OpenNT
  `VrReadNamedPipe` contract requires it. Correcting that fixture input to
  match the original DEM open precondition makes
  `t251-s3-redir-ingress-fixture.exe` return `0`; the mirror body was not
  relaxed or rewritten.

## Interpretation

The header surface is source-identical where OpenNT source exists. Modern
static composition has not been hidden in a replacement header: its explicit
divergences stay in the owning C translation units and component registers.
Both focused consumers link and pass through the fresh formal graph.
