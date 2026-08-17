# T225 S22 P4 Original INT 06 Handler Build/ABI Admission

## Question

Can the original SoftPC `illegal_op_int` / `unexpected_int` handler pair be
added unchanged to the formal MSVC x64 `/MT` Ninja product closure that reaches
the S22 `C4 C4 06` boundary?

## Source and Build Evidence

The original sources remain authoritative:

- `src/opennt/base/mvdm/softpc.new/base/system/illegalp.c` owns
  `illegal_op_int`;
- `src/opennt/base/mvdm/softpc.new/base/system/unexp_nt.c` owns
  `unexpected_int`.

The existing standalone `src/opennt/CMakeLists.txt` deliberately defines an
x86 historical compiler/CRT island. It rejects non-Clang compilers and states
that it must be configured in a 32-bit toolchain environment. Its recorded
T113 build compiled `unexp_nt.c` with clang-cl `-m32`; it proved only an x86
object closure and identified the original imports `inb`, `outb`, and
`c_sas_store`.

A fresh, disposable x64 probe at
`build/M0-T225-S22/opennt-system-x64-compile-r1` attempted to configure the
same `ntdos64-opennt-system-provider-objects` source group with the product's
x64 environment. It stopped before any object compilation: `clang-cl` is not
present on this host, and the source gate does not admit MSVC. The retained
`configure.log` records the exact failure. This is toolchain evidence only;
it did not produce or consume a product object.

The formal Ninja manifest deliberately permits only `src/bx-core`,
`src/bx-mantle`, `src/bx-vdm`, and `src/cli` module sources. It neither
compiles an `src/opennt` unit nor accepts an x86 object. Thus the historical
x86 objects cannot legally or mechanically link into the x64 product.

## Native Dependency Closure

The original handler pair requires these operations:

| Original operation | Current owner | Current state |
| --- | --- | --- |
| Read and amend saved INT 06 frame at `SS:ESP` | bx-core/bx-mantle CPU + ordinary guest RAM | The machine stage has checked ordinary-RAM copy primitives, but no admitted live handler capability. |
| Read/write PIC command/mask ports `20h`, `21h`, `A0h`, `A1h` | bx-mantle native port/device lifecycle | Current minimal mantle owns only default empty port-space. Its reads are `FFh` and writes are ignored; that is not an original PIC implementation. |
| Store BDA interrupt cause | bx-mantle ordinary RAM | Mechanically available only after a handler call context is admitted. |
| Report historical illegal-op condition | CLI/mantle diagnostic boundary | The old `host_error` callback is not a formal product ABI and must not be recreated as a hidden adapter host-service route. |
| Select `06h` only | bx-vdm machine-composition plane | The legacy `bx_ntvdm_machine_composition_v2` contains the old selector test, but is outside the formal graph and combines old callback glue with the x86 handler island. |

## Disposition

No direct addition is admissible in S22:

1. an x86 historical object cannot link with the MSVC x64 product;
2. adding the legacy callback island would bypass the formal Ninja graph and
   retain an obsolete pre-layer architecture;
3. treating empty-port `FFh` reads as a PIC would silently change the original
   `unexpected_int` policy; and
4. moving the selector into Bochs or the host-service plane would violate the
   bx-core/bx-mantle/bx-vdm boundary.

The correct successor is a dedicated **bx-mantle machine-composition recovery
package**. It must first establish an x64 source-derived rehost contract
against the exact original handler bodies, then provide only typed mantle
operations for saved-frame RAM, native port dispatch, and diagnostics. Before
that handler can run, the package must separately prove whether a minimal
native Bochs PIC lifecycle is required by the reached path. It must not copy
x86 objects, invent a PIC response, add an adapter BOP provider, or alter
Bochs #UD mechanics.

## S22 Result

S22's startup objective is now closed as an admission result: source-built
Direct/Readonly execution reaches original NTIO -> NTDOS -> COMMAND and the
next missing owner is precisely bounded. The current formal product cannot
continue through that owner without a separately admitted bx-mantle recovery;
this is neither an unclassified BOP nor an authorization for a trace-led patch.