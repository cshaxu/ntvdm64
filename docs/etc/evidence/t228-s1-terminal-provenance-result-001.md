# T228 S1 P3 — Terminal provenance result

## Result

The fresh provenance-only current-source CPU5 MSVC x64 `/MT` Ninja derivative completed 434 targets.  Its `ntdos64-native.exe` ran the hash-locked four-image T228 input manifest `c888239b9de415f5c5f38b8d8dcacf5a30c102ab1465189c1ffc1a97fb1b9b6a` in Direct and Readonly at 100,000,000 ticks.

Both runs completed inside the 60-second watchdog with exit `4`, identical stdout hash `1feb6b6a347a42169085ece455ffcd03be1fc5aae40db8a0dd4b9117cabae7e5`, and identical stderr hash `905ddf181613eb9b79dfeec0a507def81b0c26ffb1a1518ce10e4a19b0166406`.

The existing `50:36` lifecycle record remains `0041:5A70`, followed by the scalar terminal history `5A5F:9F33..9F51`, `SS:SP=0C41:FDCB`.  The explicit terminal record is:

```
CS:IP=5A5F:9F51  SS:SP=0C41:FDCB
instruction=000000000000000000000000000000
stack=bc3a9717d20897176e00
```

Both checked ordinary-RAM reads were valid.  No bytes are decoded or interpreted by the diagnostic.

## Attribution

The terminal instruction window is zero-filled ordinary RAM, not a byte range from the declared source-built COMMAND or NTDOS static input.  Thus this is not evidence of an unimplemented BOP/provider, host mutation mode, or guest-image read failure.  It establishes a complete **guest-control-flow / bx-core-bx-mantle continuity** owner boundary: some earlier transfer reached non-image RAM.

The raw stack words are retained only as evidence; without the preceding CS-transition record they do not identify the transfer instruction or select a repair.  No address-specific patch, device enablement, BOP service, or CPU semantic change is selected.  A following package may use the already registered CS-transition provenance mechanism to determine the last transition into this region, then choose one complete owner domain.
