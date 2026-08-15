# T116 S1 unexpected-int machine-composition closure 001

## Result

The first machine-composition contract is source- and link-closed without
changing imported Bochs/OpenNT source, connecting a `#UD` path, initializing a
device, or calling the historical handler.

`src/bx-ntvdm-machine-composition/bx_ntvdm_machine_bop_v1.*` defines a
same-native-island contract for exactly one admitted initial handler:

- a fixed-width request that accepts only a preclassified machine selector
  `02h`;
- a dynamically scoped mechanics context with only registered 8-bit port read,
  registered 8-bit port write, and checked ordinary-RAM byte write;
- C-linkage compatibility definitions `inb`, `outb`, and `c_sas_store`; and
- only `declined`, `handled-and-resume`, and mechanics-failure outcomes.

The context contains native callbacks and is expressly not adapter ABI. It
cannot survive the synchronous call. Host-service and unavailable
classifications, non-`02h` selectors, nested calls, null mechanics and failed
mechanics are declined or failed explicitly.

## Source fidelity

The original handler is unchanged:

| Input | Source / role | SHA-256 |
| --- | --- | --- |
| Original source | `src/opennt/base/mvdm/softpc.new/base/system/unexp_nt.c` | `A96BA5EFFE9B1CEC5D78545F9656486A8E7B692E829C1C06634653BB8C905F0E` |
| Original x86 object | T113 fresh clang-cl projection, `.../unexp_nt.c.obj` | `270BDD2A959879B35E493739289A3B8E7DD3D30F7ADCD0BA46CF087183E534B2` |
| Composition implementation | `src/bx-ntvdm-machine-composition/bx_ntvdm_machine_bop_v1.c` | `7F3D261E2B40409A98A42CB16E7668FA7B89B56AB13DE3AE2D243CD38AF028AB` |

The original `unexp_nt.c` uses only the three compatibility imports. Its
source retains its PIC ordering: it masks the pending master source but clears
IRQ2 from that mask, acknowledges a pending slave first, then acknowledges the
master and writes the master ISR to BDA `0040:006B`. The focused contract test
therefore expects master mask `10h` from inputs ISR=`04h`, IMR=`10h`, not
`14h`; this is original behavior, not a replacement policy.

## Verification

The default-disabled MinGW static test
`bx-ntvdm-machine-bop-v1-test` passed in
`artifacts/build/current/adapter-cli-r1`. It uses a source-shaped test handler
solely to exercise the three imports; it does not contain a BIOS dispatcher,
device model, or imported OpenNT object.

The reproducible x86 link-only command is:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File `
  tools\probe\Invoke-T116UnexpectedIntLinkProjection.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\current\t116-unexpected-int-link-projection-r4
```

It calls the VS x86 environment, compiles only the new component and a probe
whose `main` takes the address of `unexpected_int` without invoking it, then
links that probe, the T113 object, and the component with matching `/MDd`.
The successful r4 output has these hashes:

| Artifact | SHA-256 |
| --- | --- |
| executable | `10D3FC51BCF2A12D819215C88CF2E31F2FCB2FFC7A1330A9E2F358B53F066305` |
| build log | `0F3D571AA82D602E9563DAF72C1B10D1E34E061F459013222F43E0FD621C8094` |
| linker map | `2CECEB65886949FB35F296B5E48BEF038A85D62ADA8F13352E5CA18D7F5D2146` |

The linker map retains all required symbols from the intended owners:
`_inb`, `_outb`, `_c_sas_store`, and the composition entry come from
`machine_bop_v1.obj`; `_unexpected_int` comes from `unexp_nt.c.obj`.

This is static/link evidence only. The executable was not run, no guest was
run, and it proves neither #UD delivery nor continuous execution.

## T117 exact request

The next packet may register one new Bochs exception entry for a
default-off, selector-blind machine-composition callback. Bochs must pass the
already copied generic exception/window facts and a dynamically scoped
three-operation mechanics context to the component, then accept only its
generic `declined` or `handled-and-resume` disposition. It must not decode a
selector, initialize a device, expose that context to the adapter, or add a
host-service branch. The component, not Bochs or the adapter, will own the
first `02h` selection after a full-catalogue disposition check.
