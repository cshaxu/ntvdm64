# T198 S18 Native DEM Lifecycle Witness 001

## Transaction

The native real-mode fixture executes `50:12 -> 50:16 -> 50:00 -> 50:02`
against the existing boot-namespace plane. Its opaque RAM payload contains
both instructions at `1000:0000` and `C:\CONFIG.SYS` at `0100:0100`. Guest
code sets DS:SI, preserves the returned AX:BP token, reads two bytes, seeks
absolute zero, restores the token and closes it before HLT.

## Result

The fixture links against the same MSVC x64 CPU5 core/mantle and S16 adapter
closure used by S17, then exits zero. The run log reaches the expected HLT
after 18 native ticks. No host-side register injection, provider change,
Bochs semantic change or host filesystem is involved.

This proves the existing DEM lifecycle is a native dependent transaction, not
four unrelated handler successes. It does not prove the wider DOS runtime or
path-search services.
