# T221 S3 P40: Virtual handle family closure

The T221 handle owner family is `50:00,02,08,16,1E,27`.  Under the explicit
Virtual view, the whole-provider fixture now covers opaque typed-token open,
seek, checked guest-read/write transactions, DOS time/date get/set, zero-byte
truncate, volatile commit, close and stale-token refusal. The x64 `/MT` run at
`build/M0-T221-S3/119` passed.

All operations remain in the existing private file object: no operation looks
up a Win32 handle for `VIRTUAL_FILE`, and commit has no host flush side effect.
This closes only the Virtual handle subfamily. FCB and search remain distinct
owner packages and are not implied by this result.
