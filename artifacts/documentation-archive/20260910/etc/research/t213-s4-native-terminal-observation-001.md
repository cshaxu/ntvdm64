# T213 S4: Native Terminal Observation

Packet: M0 T213 S4, Ordinary Mode  
Date: 2026-08-15  
Status: passive native observation closure

## Procedure

The probe copied the known composition object cache into a fresh artifact root
and rebuilt the four current objects changed by T213/S3: cancellation
controller, COMMAND lifecycle provider, boot composition, and composition
runtime. It then rebuilt the native CLI plus lifecycle/event/worker sources.

## Observation

Fresh MSVC x64 `/MT` execution printed:

```text
terminal=2 detail=1 lifecycle=3 presentation=3 cancellation=0
```

This is the existing composition rejection before guest CPU entry. No BOP,
including `54:00`, was reached. It neither disproves the completed COMMAND
terminal package nor authorizes a new BOP patch.

## Follow-up

The next owner is the complete current-profile composition admission chain
returning detail `1`; trace remains only a later verification tool.
