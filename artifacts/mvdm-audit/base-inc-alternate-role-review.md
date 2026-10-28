# Alternate host interface headers

The six headers are unchanged against the identified OpenNT source and occur in
both audit catalogs. The selected NTVDM build has 638 valid dependency blocks;
none references these headers, and a source include search finds no direct
reference. The observation binds selected NT production objects to the released
image, but does not establish their runtime behavior.

| Header | Excluded role | Remaining obligation |
| --- | --- | --- |
| `host_fio.h` | Unix host file I/O interface | DOS file operations and failures via DEM remain open. |
| `unix_asy.h` | Unix asynchronous host interface | NT event and asynchronous lifecycle remain open. |
| `unix_cnf.h` | Unix host configuration interface | Selected configuration behavior remains open. |
| `unix_loc.h` | Unix host local definitions | Selected NT host dependencies remain open. |
| `swinhelp.h` | SoftWindows HyperHelp interface | Guest help functionality remains separately in scope. |
| `swinmgre.h` | SoftWindows CPU management interface | Selected NT CPU execution remains separately in scope. |

Only these alternative interfaces are excluded. Linked NT event, configuration,
CPU and DEM entries establish ownership, not semantic equivalence or full
coverage. The original header text remains intact; a restoration percentage for
an excluded alternative is not meaningful.
