# T95 S6 MSVC Configure-Substitution Audit 001

## Finding

The imported MSVC `Makefile` family is generated configuration output, not an
independent static build description.  A repository-wide scan found 105 unique
`@VARIABLE@` substitutions across the original `Makefile.in` inputs; all have
values in the retained historical `config.status`.

The important variables are not limited to compiler flags.  They determine
archive membership, GUI backend object sets, optional-device object sets,
plugin split, platform libraries, executable suffixes and the primary target.
Therefore a `config.h` macro projection cannot honestly be paired with the
old generated makefiles.

## Gameport proof

The original `iodev/Makefile.in` has a `@GAME_OBJS@` placeholder.  The
historical generated `iodev/Makefile` resolves it to `gameport.o` because its
`config.status` simultaneously says `BX_SUPPORT_GAMEPORT=1` and
`GAME_OBJS="gameport.o"`.  The accepted CPU3 projection instead says
`BX_SUPPORT_GAMEPORT=0`, but cannot alter that generated object value.

The same configuration channel controls at least `PCI_OBJ`, `CDROM_OBJS`,
`SOUNDCARD_OBJS`, `SOUNDLOW_OBJS`, `NETDEV_OBJS`, `NETLOW_OBJS`, USB object
sets, GUI backend objects/link options, plugin object split and archive
variables.  The first actual build observation demonstrated the resulting
contradiction at `GAME_OBJS`; it must not be repaired one object at a time.

## Plugin interpretation

With the historical `BX_PLUGINS=0` configuration, `configure.in` deliberately
sets `GUI_NON_PLUGIN_OBJS` (and therefore `IODEV_NON_PLUGIN_OBJS`) to the
complete core, optional-device and support-object lists.  `plugin.cc` then
uses its built-in optional-plugin control path.  This is compatible with the
architecture's existing `plugin_ctrl` profile gate, but only when the complete
configure-derived values and `config.h` describe the same feature set.

Turning a denied device back on merely to satisfy the old makefile is rejected.
Turning on external plugin loading solely to reduce static objects is also not
an implicit solution: it would change the native lifecycle and needs separate
feature admission.

## Decision

The next MSVC configuration must be produced as a **single original
configure-derived tuple**: `config.h`, every generated Makefile and
`config.status` must result from one explicitly recorded input profile.  A
new root-local generator may orchestrate that original process only after the
exact CPU3/nogui/deny feature options, host-tool probes, expected generated
files and failure handling are written down.  It may not emulate the 105
substitutions or maintain a per-object suppression overlay.

No new configuration was generated in this audit, and no build was retried.
