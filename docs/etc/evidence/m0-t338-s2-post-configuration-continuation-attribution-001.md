# M0 T338 S2 — Post-configuration continuation attribution

## Question

After the original configuration service accepts the packaged MVDM media
layout, which original source owner is reached before any DOS child execution
or parent-return work may be selected?

## Fixed observation

The existing non-debug, console-owning observer was rebuilt with one
diagnostic-only addition: it records the launched image's module base, size
and path before the existing bounded timeout snapshot.  It neither reads guest
memory nor alters guest state, inputs, execution or resumption.

One unchanged run used the selected T337 x86 product and the byte-identified
`build/M0-T337/S3/package-contract-r3` media stage.  The resulting bounded
record is `build/M0-T338/S2/post-config-continuation-observation.txt`:

- image base: `0x00bd0000`;
- timeout: eight seconds, then the existing observer watchdog stop;
- original-frame chain after rebasing to the link-map image base:
  `ErrorDialogBox -> RcErrorDialogBox -> ExpandConfigFiles ->
  cmdGetConfigSys -> CmdDispatch -> MS_bop_4 -> c_cpu_simulate ->
  host_start_cpu -> host_main`.

The map identifies `ExpandConfigFiles` and `cmdGetConfigSys` as the original
`dos/command/cmdconf.c` object, and the BOP frame as the original
`softpc.new/host/src/nt_bop.c` object.  This is a source-owner attribution,
not a claim that the external observer selected a unique source line inside
`ExpandConfigFiles`.

## Configuration-path finding

`app_package_layout_set_media_roots` sets the session MVDM system root to
`<exe>\\mvdm`, not `<exe>\\mvdm\\system32`.  Original `nt_pif.c` therefore
resolves the no-PIF `config.nt` and `autoexec.nt` pair at that root; the fixed
stage contains both files there.  Original `cmdconf.c` subsequently writes
the configured shell as `%SystemRoot%\\System32\\command.com`, and the fixed
stage contains that shell at `mvdm\\system32\\COMMAND.COM`.

Consequently, `config.nt` is not a missing-file fallback and is not located in
the wrong package directory.  It is actively processed by the normal
`50:3B`/`SVC_GETCONFIGSYS` path.  The observed `RcErrorDialogBox` is one of
the original fatal branches inside configuration expansion; this S2 record
does not guess which predicate caused it.

## Transfer

The next finite owner is the original COMMAND configuration-expansion error
contract: `cmdconf.c::ExpandConfigFiles` and its same-shaped temporary-file,
OEM conversion and session-system-root bindings.  S3 must identify the exact
source predicate before changing any binding.  It must not add a guest loader,
skip `config.nt`, replace COMMAND, invent a BOP result, or introduce a second
executor.

## Limits

No normal DOS child execution, NTDOS parent restoration, COMMAND `54:0B`
completion or x64 guest execution is claimed.
