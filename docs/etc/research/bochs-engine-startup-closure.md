# Bochs Engine Startup Closure

## Finding

`ntdos64-run --engine` cannot yet name `bochs.exe` directly.

The runner's engine ABI is:

```text
--byob-profile <profile> --byob-root <root> -- <target> [args...]
```

By contrast, the imported Bochs 2.6 native entry accepts its own configuration
ABI: `-q`, `-f <bochsrc>`, `-benchmark`, logging options, and then Bochs
configuration directives. In `src/bochs/main.cc`, an unrecognized dash option
panics and remaining non-flag arguments are passed to the Bochs configuration
parser. Therefore forwarding the runner's engine argv to stock Bochs would be
incorrect; it is not a missing compatibility switch.

## Required Thin Shim, Not Yet Implemented

A future `ntdos64-bochs-engine` shim may be admitted only after the first
guest startup descriptor exists. Its whole responsibility is bounded:

1. receive the already validated runner engine ABI;
2. materialize a versioned, adapter-owned Bochs configuration *data file* in a
   private run directory (ROM paths, memory amount, selected `plugin_ctrl`,
   log path and other reached mechanics only);
3. preserve the runner-installed adapter environment unchanged;
4. invoke stock Bochs solely as `bochs.exe -q -f <generated-bochsrc>`;
5. collect the child exit/log result and return it without translating guest
   DOS behavior.

It must not parse a DOS executable, load NTIO/NTDOS/COMMAND, construct a PSP
or command tail, implement BOP/DEM, own a filesystem, or select devices by
ad-hoc command-line flags. The target and arguments must be rejected until a
separate guest bootstrap descriptor defines their source-derived handoff.

## Why It Is Deferred

The current r12 ROM demonstrates only a synthetic real-mode transaction. It
has no admitted NTDOS reset vector, entry register state, firmware/BDA/IVT
requirements, command-tail consumer, or controlled stop contract. Generating
a production Bochs configuration before those inputs exist would create an
apparently runnable CLI that silently discards the requested target. That is
more dangerous than an explicit unsupported-engine result.

The next admission work is therefore the *whole* first guest-startup
descriptor: original input identity, load locations/order, CPU/register entry
state, required firmware/device closure, host-service catalogue, target
transport, and expected stop/failure behavior. Only that descriptor may cause
the shim implementation to begin.

## Boundary Consequence

This conclusion keeps the boundaries intact:

| Owner | Responsibility now |
| --- | --- |
| Runner | Validate BYOB identity and install the same selection in the child environment. |
| Future shim | Translate runner process lifecycle to native Bochs configuration syntax. |
| Bochs | Parse its original configuration and assemble/reset its own machine. |
| Adapter | Install resources and handle admitted typed guest-to-host transactions. |
| Guest | Execute NTIO/NTDOS and eventually consume target/command-tail semantics. |

No Bochs source change is needed or authorized by this finding.
