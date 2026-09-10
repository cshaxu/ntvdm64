# T194 S13 v6 normal-return BYOB-root materialization 001

Date: 2026-08-12  
Packet: M0 T194 S13  
Disposition: v6 input root source/test closed; no engine execution.

## Materialized root

`artifacts/inputs/t194-v6-normal-return-root-r1` contains exactly:

| Guest filename | Bytes | SHA-256 | Source |
| --- | ---: | --- | --- |
| `NTIO.SYS` | 33792 | `CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937` | retained T177 BYOB input |
| `NTDOS.SYS` | 27858 | `957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84` | retained T177 BYOB input |
| `COMMAND.COM` | 50384 | `908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43` | retained T177 BYOB input |
| `TARGET.EXE` | 882 | `69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC` | exact T177 source-built `SHARE.EXE` |

The v6 profile declares exactly these four components, one non-terminal target
at `C:\TARGET.EXE`, fixed four-file metadata, and the retained NTIO startup
handoff. It contains neither `QUIT.COM` nor an ambient host path.

## Validation

The existing adapter-runtime fixture was invoked only through its supplied
profile installation/startup-plan preparation branch, with
`NTDOS64_ADAPTER_LAUNCH_PLAN=2,1,e,00`. It accepted the profile and reported a
33792-byte startup payload. No Bochs engine, CPU loop, BOP or guest code was
run.

## Follow-up

T194 S14 must audit the exact runner and thin shim binary identities against
the r2 bundle and this root. A watchdog observation remains a separate S.
