# M0 T194 S54: Deferred Native-POST Observation

## Question

Does the S51 deferred native-POST composition reach the paired metadata BOP
path without the S48 direct-pre-POST aperture stop, and what bounded facts can
one observation establish?

## Inputs and Procedure

The S47 controller ran once with a 15-second watchdog and `retryPolicy: none`.
Its inputs record the unchanged runner, shim, profile and target hashes and
the S51 engine hash
`79D404A9451F218009408D75AF2F2A36CFB8F984FFF64C0328DC34A8EBB0857E`.
The evidence root is
`artifacts/analysis/t194-s54-deferred-metadata-observation-001`.

The terminal record reports wrapper PID 20032, watchdog termination after
15427 ms, successful tree cleanup (`treeKillExit: 0`), no cleanup timeout and
no launch error. It retains stdout SHA-256
`CC3A0FEED71C02B9F4950E0F1A737A94CC12EA6C56B974004AAA0122A5A23B17` and
stderr SHA-256
`26BDCA847FA24AB17AE8A4FFB2C177315AD990490A7D1F6243E65E4B9C99A426`.
The post-run process check found no matching `ntdos64` or `bochs` process.

## Observations

- Native POST completed far enough to reach `50:11` and commit the existing
  NTDOS transaction at guest address `0x8b00`, 0x6cd2 bytes.
- The trace reached `50:21`, `AX=4408h`, three times: first with `BX=f002h`,
  then twice with `BX=0002h`. It also reached four `50:42` fast-read calls.
  The listener is observational; these lines alone do not prove the metadata
  provider's result or a DOS IOCTL success.
- The direct S48 `0xa1800` prefetch-map veto did not occur. The later retained
  fault is instead `EIP [00010000] > CS.limit [0000ffff]`, followed by
  stack-prefetch `0xffff` and the Bochs third-exception panic.
- The listener recorded 6551 BOP entries, 6509 of which are selector `06`.
  This is a repeated, source-classification target, not a reason to add an
  adapter handler or alter Bochs fault mechanics.

## Interpretation and Follow-up

S51 proves the native-POST/deferred composition removes the earlier
direct-entry aperture-stop variable and makes `50:21` reachable. It does not
prove continuous execution, `54:11`, a normal return, metadata-provider
success, or the cause of the later EIP/stack fault. The next source-only task
is to classify the observed selector-06 re-entry and the control predecessor
of the late fault against OpenNT and existing SoftPC evidence before any new
runtime attempt or semantic change.
