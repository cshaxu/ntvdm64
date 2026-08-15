# Requested Reports And Formal Releases

`artifacts/` is a Git-tracked publication boundary. It is not a build,
diagnostic, evidence, input, toolchain, cache, or runtime-log directory.

Only these owner-approved contents are allowed:

| Directory | Allowed contents |
| --- | --- |
| `reports/<task-id>/` | A human-readable report explicitly requested by the owner. |
| `build/<task-id>-<version>/` | An explicitly approved formal executable release and its manifest. |

Evidence, inventories, hashes, operational records and research conclusions
belong under `docs/etc/`. All temporary build, probe and debug output belongs
under the ignored `build/<task-id>/<run-id>/` tree and must be removed when it
is no longer reusable.
