# M0 T318 S2 P52 — Original XMS configuration prerequisite

## Question

`xms.486/xms.c::XMSInit` returns failure when `xmsMemorySize` is zero.  This
record verifies whether the selected ordinary DOS profile can reach that
branch, rather than treating the linked XMS service table as sufficient proof.

## Original ordering and result

The original CPU40 startup order is:

```
base/support/main.c::host_main
  -> config()
  -> cpu_init()
  -> InitialiseDosEmulation()
     -> scs_init()
        -> XMSInit()
```

Original `softpc.new/host/src/config.c` performs its memory calculation before
`sas_init` and before `scs_init`:

- a WOW profile uses the original fixed 8192 KiB XMS value;
- the selected ordinary DOS profile uses original PIF extended-memory data or
  `XMS_DEFAULT_MEMORY_SIZE`;
- the original ordinary-DOS floor then forces any value below 1024 KiB to
  1024 KiB.

Only after that calculation does original `config()` call `sas_init`; the
later original `XMSInit` therefore cannot enter its `!xmsMemorySize` failure
branch for either selected profile.  This uses no adapter decision and no
host-width-specific branch.

## Disposition

There is no XMS-size repair, profile rewrite, mapper alteration, or runtime
observation to admit.  The existing session mapping backend remains relevant
only after original XMS allocation begins; it is not a substitute for original
configuration ownership.

The next valid source-first boundary remains first guest reachability after
the original NTIO entry transaction.  The frozen product is unchanged and is
not rerun.
