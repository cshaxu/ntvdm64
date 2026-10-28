# Selected DOS and host input declarations

Four original headers are dependencies of selected worker compilations and
remain partial. The source reviews cover [COMMAND and DEM](shard-2/dos-command-dem-abi-review.md),
[CGA](shard-3/cga-herc-interface-role-review.md) and [mouse input](shard-3/mouse-host-interface-role-review.md).
The `BOP.INC` and Hercules interface stay pending; neither is excluded based
on one selected configuration.

The current dependencies and map snapshots do not establish complete guest
dispatch, video rendering, input events, PIC/EOI ordering, failure or teardown.
No product or guest image was changed or executed by this checkpoint.
