# T198 S41 stack position witness

The registered default-off position record now copies only SS and low SP in
addition to its existing fields. It does not read stack contents or guest
memory and does not change CPU state.

The rebuilt CPU5 x64 finite run observed the retained CS transition:

```text
9346:4D53 SS:SP 00A7:0918
0000:0000 SS:SP 00A7:090E
```

The ten-byte stack-position change rules out an ordinary near jump. It does
not identify the transfer owner; no BOP or device behavior changed.
