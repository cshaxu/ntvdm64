# T202 S5 COMMAND whole-provider and regression design

## Atomic ownership rule

S6 may switch COMMAND product routing only when one `bx-vdm` COMMAND session
owns all `54:00..10` identities. The switch is atomic at the package entry:
no selected COMMAND service may fall through from the new provider into a
legacy boot/readonly/profile leaf. A service whose necessary capability is not
admitted still returns its source-derived deferred/unavailable outcome from
the same provider.

The provider keeps host resources session-private. Guest registers, guest
memory and the bx-mantle mechanical ABI carry only copied fixed-width values,
not a `HANDLE`, host pointer, callback, CRT allocation or Bochs type.

## Provider slices and dispositions

| Slice | Services | State owner | Profile disposition | S6 result |
| --- | --- | --- | --- | --- |
| terminal/no-op | `00`, `03` | COMMAND session | n/a | Preserve typed stop and source no-op. |
| bootstrap/context | `01`, `02`, `04`, `05`, `0F` | one copied launch/environment/CWD record | direct/readonly/overlay/virtual | Rehost as one bounded context transaction; direct CWD/environment use requires explicit CLI selection, never ambient state. |
| console/keyboard | `09`, `0E`, `10` | one CLI console/keyboard state record | readonly/deferred | Preserve no-console/DX=0 branch until a complete console profile is admitted; no VDD route. |
| process/lifecycle | `07`, `08`, `0A`, `0B` | declared process/result lifecycle record | direct/deferred | Keep a complete source-derived deferred outcome for 08/0A until typed child lifecycle exists; 07/0B must share that decision rather than inventing a process result. |
| config materialization | `0C`, `0D` | selected boot-input record | readonly/overlay/virtual | Preserve copied boot-input path results; defer host config/temp-file creation to an explicit profile, never the default host system path. |
| standard streams | `06` | opaque stream table (future) | direct/readonly/deferred | Preserve explicit deferred result until one stream table can map guest use without exporting raw `HANDLE` values. |

## Required regression

One MSVC x64 `/MT` package fixture must bind a complete session and cover all
17 services through common ingress. It must prove:

1. every service produces a typed resume or controlled stop, never generic
   pass-through;
2. `54:06` cannot expose its historical `BX:CX` host-handle encoding;
3. `54:08/0A` cannot create a process or child result before the lifecycle
   capability is selected;
4. selected CWD and environment inputs are copied, bounded and do not mutate
   ambient host state under readonly/overlay/virtual dispositions;
5. `54:0C/0D` do not create/delete a host system or temporary file by default;
6. console/KEYB branches cannot call VDD or retain guest pointers; and
7. registration, CMDINFO, return/next and teardown reject stale or cross-
   session state.

A single bounded native trace follows only after that regression. It observes
the package switch and cannot add a `54:xx` task.

## Admission consequence

The package has no missing ordinary Win32 API. The prerequisites are explicit
bx-vdm capability records: copied host context, opaque streams and typed child
lifecycle. Therefore S6 is admitted as a whole COMMAND provider recovery;
individual `54:06`, `54:08` or `54:0A` patches remain prohibited.
