# M0 T354 — CPU40/NTDOS permanent COMMAND package-path closure

T354 reconstructed the whole reached permanent `COMMAND.COM` bootstrap
cohort. It established, without changing DEM, BOP, guest bytes or media, that
the source-generated permanent open fails because the current executable-
relative media root generates a 114-character `shell=` value for NTDOS's
64-byte `commnd` field.

The owner rejected all path-projection and guest-rewrite workarounds. The
product now enforces the original 63-visible-character input contract before
original host startup, displays an installation-path explanation, and returns
the app media-admission outcome. The current long-path package therefore does
not claim to boot COMMAND.

The next command-bootstrap runtime observation is conditional on a package
installed beneath a valid short Windows path and requires a new owner
admission. This is an environmental package contract, not an unfinished DEM
or BOP implementation.
