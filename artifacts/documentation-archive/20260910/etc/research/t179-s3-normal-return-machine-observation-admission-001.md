# T179 S3 normal-return machine observation admission 001

## Decision

Admit one direct, bounded observation of the existing T177 native-POST and
machine-composition binary with the retained source-built SHARE normal-return
input.

## Exact inputs

| Input | Identity |
| --- | --- |
| Existing binary | `t177-s4-native-post-machine-composition-r1/ntdos64-t98-current-adapter.exe`, SHA-256 `8758F4335CB32B4FB97688ED3860E8B0C9E86D8155E6AA316E54F346177E8BFA`. |
| Target | Retained `t182` `TARGET.EXE`, source-built SHARE, 882 bytes, SHA-256 `69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC`. |
| Profile/configuration | Retained T182 profile SHA-256 `543DCA5555DF7D0D0AA7FC828F66C5B114E3C9AC7C3D0E39120E280C9798F67B`; bochsrc SHA-256 `41F284D4880839EB1852A0C5361CE704C38D07D60587E9B848476023058F83DF`. |
| POST ROM | Retained source identity `E4CA4AE5190A15ED741D1DFED5A58533A2B6926A40302D839FB714F9FC673FFB`. |

## Envelope

One existing direct-PID controller invocation uses `LaunchKind=exe`, an empty
tail, ten-million benchmark, 30-second watchdog and 50-second outer timeout.
It writes only a fresh evidence directory. No source edit, compile, link,
archive rebuild, device/firmware/vector change, provider selection change or
second observation is permitted.

## Required classification only

Record whether `50:3C` is reached, whether selector `06h` produces the
existing machine-composition report/outcome, and whether guest execution
continues beyond the former self-loop. A normal `54:11`, simulator exit, or
watchdog is descriptive only and cannot by itself establish CLI result
semantics.
