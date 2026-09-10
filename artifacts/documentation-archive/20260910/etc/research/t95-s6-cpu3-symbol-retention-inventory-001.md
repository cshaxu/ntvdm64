# T95 S6 CPU3 symbol-retention inventory 001

## Question

After retaining all original CPU project objects, which unresolved symbols are
still genuinely outside the CPU island, and do any of them justify admitting a
product device, GUI, configuration or adapter provider?

## Inputs and procedure

The input is the frozen 93-object CPU3/MSVC x86 inventory from
`t95-s6-cpu3-project-objects-001-20260811-001`. One read-only
`dumpbin /symbols` pass produced
`artifacts/analysis/t95-s6-cpu3-symbol-retention-001-20260811-001/cpu-symbols.txt`.
The parser groups external definitions and undefined references by original
object, then removes a reference only when another object in the same 93-unit
CPU island supplies that exact identity.

The raw symbol file SHA-256 is
`691A45A766F75CEE8259D868A4B00DD6B53563CE9B28ED73F3BC2363B1362FFD`.
The machine-readable unresolved ledger is
`cpu-unresolved-after-internal-providers.json`, SHA-256
`597B1164C22AE8AC0A112EC200596D8BF750E4C2E6EDB210E32CEE29EAEC8A41`.
The first two parser attempts recognized no symbols because their patterns
omitted one of `dumpbin`'s two leading hex fields; they did not rerun
`dumpbin`, compiler or linker. The final parser matches both fields and is
the only result used here.

## Observation

The 93 objects expose 1,253 external definitions and make 1,039 distinct
external references. After same-island providers are removed, exactly **51**
identities remain. Their unique-identity and object-reference counts are:

| Owner class | Identities | References | Meaning |
| --- | ---: | ---: | --- |
| MSVC CRT/runtime | 21 | 46 | Allocation, strings, integer helpers, cookie, RTTI and nonlocal control support. |
| Original Bochs logging | 7 | 63 | `logfunctions` constructor/destructor and error/info/debug/panic/put. |
| Original SIM/parameter tree | 10 | 11 | `SIM`, parameter/list construction and state-save registration helpers. |
| Minimal-machine globals | 4 | 63 | `bx_cpu`, `bx_mem`, `bx_devices`, `bx_pc_system` storage. |
| Original memory mechanics | 3 | 3 | CPU physical-page read/write and host-memory address helpers. |
| Original PC-system mechanics | 2 | 3 | timer countdown and reset. |
| Original I/O mechanics | 2 | 2 | `bx_devices_c::inp/outp`. |

No unresolved identity is a VMX/SVM/SMX/ADX/AVX/XSAVE handler, a CPU decoder
table member, a device plugin, GUI, configuration frontend, adapter or OpenNT
symbol. The complete original CPU object set therefore closes its own native
instruction family at object-symbol level.

## Interpretation

The remaining requirements align with the intended boundary: CPU execution
requires retained Bochs machine mechanics and its own original logging/SIM
infrastructure, plus the MSVC CRT. They do not authorize a product shell.
In particular:

- the global symbols are already owned by the registered Bochs-local
  minimal-machine composition, but need fresh object evidence before a link;
- physical memory and I/O ownership remain Bochs mechanics, not adapter work;
- SIM and logging are the previously recorded retention roots and have no
  admitted narrow replacement; and
- `pc_system` supplies real CPU reset/timer mechanics, so its lifecycle root
  cannot be silently replaced to make a link succeed.

## Decision and follow-up

The CPU island is **source-built and internally symbol-closed**, but the
minimal machine is not link-closed. A new link observation is not yet
admitted. Before one can be bounded, a source-to-object inventory must provide
fresh CPU3/MSVC objects for the selected original SIM, logging, memory,
PC-system and isolated port/machine mechanisms, while explicitly excluding
their product-owner retention edges. Adding `devices.cc`, `main.cc`, GUI,
configuration, plugins, adapter or OpenNT objects is rejected.
