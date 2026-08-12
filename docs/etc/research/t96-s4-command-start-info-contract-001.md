# M0 T96 S4 COMMAND Start-Info Contract 001

Pinned OpenNT `cmdmisc.c:893-896` defines `SVC_GETSTARTINFO` (54:10) entirely
as `AL = DosSessionId ? 1 : 0`. Its documented meaning is whether the VDM was
started with a new console/session, allowing COMMAND.COM to change TSR exit
behavior. It does not create a console, start a VDD, access guest memory, or
pass a host handle.

For the non-invasive CLI profile the only admissible replacement input is an
explicit immutable session-policy Boolean captured at CLI launch. A future
provider may preserve AH and write only AL=0/1 after common ingress, registry,
and COMMAND-plane validation. It must not infer the value from ambient console
state or add console/VDD behavior. Current disposition remains deferred until
that session-policy value is part of an admitted copied launch ABI.
