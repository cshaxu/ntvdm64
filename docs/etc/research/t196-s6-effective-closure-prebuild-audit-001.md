# M0 T196 S6 Effective-Closure Prebuild Audit 001

## Question

Did the T196 boundary repairs create the effective configuration and retained
object closure required to admit a first minimal MSVC build?

## Current-source check

The current source retains the same owner edges identified by the T95 closure
contract:

- `bx_ntvdm_minimal_sim.cc` calls `bx_init_siminterface`, whose complete
  original implementation remains in `gui/siminterface.cc` and contains a
  `bx_atexit` product exit path;
- `cpu/init.cc` unconditionally calls `init_FetchDecodeTables`, so an exact
  CPU handler-object closure remains required rather than a hand-selected
  instruction subset;
- `cpu/exception.cc` still names `DEV_cmos_get_reg` in the original triple
  fault reset branch;
- `memory/misc_mem.cc` retains original VGA debug accessors; and
- `iodev/devices.cc` retains product initialization, plugin and CMOS checksum
  paths outside the isolated empty-port lifecycle.

## Result

T196 removed semantic/diagnostic boundary violations but did not, and was not
authorized to, extract a cohesive SIM/device/logging product shell. Therefore
the T95 configuration/object-closure blocker remains current. There is no
effective `config.h`, fixture root, retained COMDAT proof, or safe object/link
vector to build.

No compiler, linker, recipe, build root, or executable was invoked. The next
S6 action is to decide whether a separately admitted cohesive Bochs-internal
product-shell extraction can exist; absent that evidence, T196 must close with
this source-backed build limitation rather than retry an unclosed build.
