# Redirector service owner bindings

The [observer](redir-service-owners-observer.py) checks source identity against
both originals and binds 22 mapped object functions from four selected source
files to released VDMREDIR code. The [snapshot](redir-service-owners-observations.json)
records 11 initialization/interrupt functions, one DOS-process cleanup,
seven mailslot functions and three path/error utility functions, with source,
object, map and DLL hashes. Relocation fields are masked during body matching;
matching code is not proof of relocation targets or runtime behavior.

Original code owns the async completion queues, interrupt accounting and
mailslot bookkeeping; Windows owns the corresponding real handles and native
services. The [dispatch binding](redir-dispatch-release-review.md) verifies
the original service slots, and the [async lifecycle audit](../shard-3/redir-async-role-review.md)
retains callback quiescence, failures and cleanup obligations. Real guest
requests, provider availability, handle reuse, concurrent completion and
teardown still require verification. These are partial source roles, not a
claim that all network behavior has been restored.
