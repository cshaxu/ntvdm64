# M0 T266 S2 — Bochs formal-input recovery

## Question

Can the current formal Ninja Bochs closure use only repository-owned Bochs
inputs, without changing Bochs semantics or retaining a live `refs/bochs`
dependency?

## Imported exact inputs

The following inputs were copied from the pinned local Bochs 2.6 snapshot to
the same relative path below `src/bochs-core`. Each source and destination
SHA-256 is identical.

| Relative path | SHA-256 |
| --- | --- |
| `bochs.h` | `65D1040E86C83EAEC1EA2AE49312E8899FDFBB0A18C673C8ADE0B8484741D697` |
| `config.h` | `A1FADC25502FB0A4644917B6F3A62987C125CC7335457F9C59E7CE7E62A45AC8` |
| `osdep.h` | `F81F7C70660DA5886E6E24E0934FD7BF65C80A6D756ABA30C598CCFA28A9C8BD` |
| `plugin.h` | `56D1960EAC5547522807EBFE5CE50F24FF6C83FF581DFE85D4509F64E2244A5C` |
| `extplugin.h` | `BAB522F7B3ED58A660A4F93927D19E20A811D6DB222C0E169829D6401BD4B201` |
| `param_names.h` | `E3C9BABE1258F6B5CDE223D54A668478F9953A1ED4C24EF48A98429E6114C6BD` |
| `bx_debug/debug.h` | `C7B3D872803A71B32A30ACA2891CC4123DB2A498A6F97B24E58331F6A013B1E6` |
| `gui/gui.h` | `9A5FDF45700E60288B89729C1ECCF1104C95E632E5D061019FD3562972EE4109` |
| `gui/siminterface.h` | `0B920D0C718B73F95267E0F38759966241A59869667FC8E3397EEA0314B979F6` |
| `instrument/stubs/instrument.h` | `7A2D587C0CEBFD8C1AC403B23CC3E32727B9031B0CB3BA7CC5025E8884083457` |
| `iodev/iodev.h` | `7B21F135C2FCB1ACE75CB8F6B0CBC78A63D418E6E85B78FEDB6E60D6B6070814` |

The current mechanics fixture also compares the exact, fixture-only original
iodev/devices.cc source imported to src/bochs-core/iodev/devices.cc:
F72A879D8607697B8F5CAC236720B58407D907704BD6A5BBA7731755FAC0A954.

## Re-rooted consumers

- The formal graph generator and pinned-config projector now read
  `src/bochs-core/config.h`.
- The component manifest includes only `src/bochs-core` formal Bochs roots.
- Current Bochs boundary fixtures read the owned `iodev` headers and exact `iodev/devices.cc` source.
- The T104 derivative fixture now asserts the owned `main.cc` source path.

Historical tools remain deliberately out of scope for S2 and are assigned to
S4 by the T266/S1 ledger.

## Verification

On 2026-08-24, outside the sandbox:

1. Generated a fresh formal graph at `build/M0-T266-S2/r001` using the
   repository-owned pinned config.
2. Ran Ninja for `lib/bochs-core.lib`, `lib/adapter-bochs.lib`,
   `bin/t261-s5-bop-ingress-fixture.exe`, and
   `bin/t244-s2-headless-8042-lifecycle-fixture.exe`; Ninja reported
   `no work to do` after the completed closure.
3. Ran both fixture executables by absolute path: each exited `0`.
4. Ran `ninja -n` for the same targets: it reported `no work to do` and
   exited `0`.
5. Searched all S2-owned generator, manifest and current fixture paths for
   `refs/bochs` or `refs\bochs`: zero matches.

## Result

S2 is closed. The known formal header/configuration closure is now exact,
repository-owned Bochs input. This proves neither a broad Bochs import nor a
runtime semantic change. S3 next handles reached OpenNT inputs; S4 then
retires historical executable consumers and archives approved reports before
S5 may delete `refs/`.
