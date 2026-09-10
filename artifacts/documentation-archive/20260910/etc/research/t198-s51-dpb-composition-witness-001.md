# T198 S51: DPB multi-write composition witness

## Result

The finite composition now invokes the existing `50:46` DPB producer using
the immutable drive snapshot already owned by its gset plane.  It maps the
returned existing multi-write transaction into the existing mantle ordinary
RAM write action, executes that action, and returns the producer's existing
typed resume result.

No Bochs code, new BOP ABI, device, host namespace, or detached runtime is
involved.

## Clean source-built witness

`artifacts/build/t198-s51-dpb-composition-r3/run.log` records that execution
passes the former `50:46` frontier.  The next unhandled BOP window begins
with selector `15` and reaches terminal `8DC8:0657`:

```text
observed-drive-resume=1 observed-dta-resume=1
observed-hard-error-resume=1 observed-first-generic-ud=0
observed-stop=1 next=15:26 terminal=1:8dc8:0657
```

The r3 closure compiles every explicit current composition source, including
the DPB producer, while the fixture bridge remains the sole generic-UD ingress
definition.  The manifest marks the separately compilable generic-UD provider
bridge as compile-only for this fixture to avoid a duplicate ingress symbol.

## Source interpretation

The `:26` diagnostic is not a BIOS service number.  BIOS BOP selectors are
three-byte `C4 C4 selector` instructions; the bridge's four-byte display
treats the following normal instruction byte as a pseudo-service.  OpenNT
`sysinit1.asm` issues `BOP 15h` after loading `AH=88h` to query extended
memory.  Its original SoftPC BOP table maps selector `15` to `cassette_io`,
but the non-invasive finite profile provides only the source-observed AH=88h
memory-size contract through `bx_ntvdm_bios_memory_service_v1_dispatch`.

The next task must capture the actual `EAX` and three-byte BOP window at this
unhandled boundary.  It must not assume AH=88h merely from source proximity,
nor activate cassette, CMOS, or an original BIOS product handler.
