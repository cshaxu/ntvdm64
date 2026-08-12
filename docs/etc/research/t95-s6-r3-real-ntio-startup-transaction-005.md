# T95 S6 r3 Real NTIO Startup Transaction 005

## Question

Can the linked r3 companion apply the source-built NTIO plan and complete its
single narrow generic #UD/write/resume transaction under direct process
supervision?

## Inputs and procedure

The direct-PID observer ran once in
`artifacts/analysis/t95-s6-r3-real-ntio-startup-transaction-005-20260811-001`.
It used the immutable r3 binary and exact source-built NTIO, NTDOS and COMMAND
inputs recorded in its JSON. The observer set only the established adapter
profile/root values (and explicitly cleared the old test/bridge values), ran
the fixed `-benchmark 1` command, retained Bochs's own log, and killed only its
direct r3 PID after the 30-second watchdog elapsed. A process audit found no
remaining r3 process.

## Observations

The record reports `planApplied: true`, one and only one generic commit, and
no broad-interceptor marker:

```text
guest transaction committed address=8b00 bytes=6cd2 resume=479
```

Those values match the recorded source-built NTDOS payload length and the
historical real-mode `DI:0000` destination. The NTIO plan was therefore loaded
and the default-off narrow path consumed one generic write/resume result.

The process did not reach its benchmark controlled-stop marker. After the
commit, the log repeatedly reports:

```text
prefetch: EIP [00010000] > CS.limit [0000ffff]
```

and terminates with:

```text
prefetch: getHostMemAddr vetoed direct read, pAddr=0x00000000000a1800
```

The watchdog then killed the exact direct process PID; exit is `-1`. The
evidence JSON SHA-256 is
`D9BF6F5827395D1E05F149C7B1CEEB57B8850F473530F0DEA6B42231ACCAC9EE`; the
Bochs log SHA-256 is
`82FA7B1B40AE87A383C6D3E746EC576F26C2AC36C665509AB5A036759A928F3E`.

## Interpretation

This is the first runtime proof of the desired narrow composition:

```text
source-built NTIO plan → generic #UD boundary → adapter transaction
→ ordinary-RAM NTDOS write → checked resume
```

It is not DOS boot/runtime proof. The post-resume prefetch/CS-limit failure is
a separately classified Bochs execution-state defect. No extra service,
device, guest semantic, broad interceptor, OpenNT object, SoftPC object, or
host capability is selected by this result.

## Follow-up

The next gate is read-only provenance of the post-resume CS:EIP state and the
exact CPU result application path. Any repair requires a separate exception or
adapter/Bochs decision; it must not be hidden by increasing the benchmark,
adding a device, or widening the service plane.
