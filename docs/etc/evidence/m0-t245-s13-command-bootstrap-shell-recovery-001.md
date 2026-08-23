# M0 T245 S13 — source-built COMMAND `SHELL=` recovery

## Question

Can the current source-built `COMMAND.COM` be supplied to the original DOS
bootstrap without a guest-memory patch, a boot-volume alias, or a Bochs
change?

## Source-first ledger

1. **Original source reuse.**  `src/opennt/base/mvdm/dos/command/cmdconf.c`
   is already imported as `src/bx-vdm/bop/opennt/command/cmdconf.c` and is the
   original producer for the temporary configuration file requested at
   `sysinit1.asm:1163` (`CMDSVC SVC_GETCONFIGSYS`).  Its normal producer
   creates a `SHELL=` line.
2. **Smallest compatibility seam.**  `sysconf.asm:1779..1804` stores the
   executable token of that line in `sysinit2.asm:commnd`; `sysinit1.asm:1517`
   later uses `commnd` for the original `INT 21h/AH=4Bh` command launch.  The
   bx-vdm startup session therefore supplies only the admitted bundle's OEM
   `COMMAND.COM` pathname to imported `cmdconf.c`.  The imported source keeps
   its filtering, `SHELL` ordering, `/p`, and optional `/c` or `/e` handling.
3. **External-code intrusion.**  None.  No bx-core, bx-mantle or Bochs source
   changes are used.
4. **New behavior.**  None.  The named shim rejects an unrepresentable input
   instead of supplying a different guest filesystem behavior.

## Fixed contract

`commnd` is `"\\COMMAND.COM",0,51 dup (0)`: exactly 64 bytes.  The source
parser has no quote grammar and ends the executable token at the first literal
space.  The session seam therefore accepts an OEM, NUL-terminated path of at
most 63 characters with no literal space.  `GetShortPathNameW` is used only to
obtain the existing host pathname spelling; it does not create a short alias.
If the volume cannot provide a representable no-space path, startup admission
fails rather than truncating, mapping a root to `C:`, or changing the guest.

For the current source bundle the admitted string is:

```
O:\repos.hobby\ntdos64\build\output\dos\COMMAND.COM
```

It is 51 characters and contains no literal space.

## Implementation and verification

- `command_config_shim.c` carries the one session-owned bootstrap string.
- `dem_v2_startup_composition.c` derives and bounds it at bundle admission.
- `command_v2_runtime_session.c` installs it before the original `54:0C`
  `cmdGetConfigSys` call.
- The only imported-source delta in `cmdconf.c` substitutes the original NT4
  installed `command.com` pathname when that bounded session value is present.

Formal MSVC x64 `/MT` Ninja graph `build/M0-T245-S13/formal-r1` built and ran:

```
ninja -C build/M0-T245-S13/formal-r1 -j 4 \
  bin/t231-s5-command-config-direct-fixture.exe
build/M0-T245-S13/formal-r1/bin/t231-s5-command-config-direct-fixture.exe
```

The fixture verifies the emitted `shell=O:\NTDOS64\DOS\COMMAND.COM /p`
line, retained `/e:512`, source-terminal handling, and rejection of both a
64-character executable token and a token containing a space.  It completed
with exit code zero.  The same graph fully linked `bin/ntdos64-native.exe`.

## One native observation

One Direct source-built run used the current `build/output/dos` and
`build/output/wow16` bundles, `share.exe`, a 1,000,000-tick budget and BOP
sequence observation.  It still accepted `5E` and stopped through `50:3D`;
the sequence has 28 records and ends with:

```
5E at 8e08:08af, disposition=resume
50:3D at 8e08:0984, disposition=stop
```

This does not invalidate the source path publication: it proves the newly
linked route does not fabricate a continuation.  Nor does it prove which
guest `INT 21h` pathname/drive/EXEC predicate selected `comerr`, because no
post-`5E` file-service BOP was observed.  The exact guest-owned execution
and parent-return cause remains in M0 T245's NTDOS `EXEC` package; it is not
a COMMAND BOP or Bochs defect.

## Follow-up

S13 closes the source/ABI path publication seam.  A later T245 subtask may
observe the original guest `commnd` and its `INT 21h/AH=4Bh` predecessor under
the declared guest-EXEC owner boundary.  It must not replace that observation
with a virtual boot volume or a `50:3D` resume rule.
