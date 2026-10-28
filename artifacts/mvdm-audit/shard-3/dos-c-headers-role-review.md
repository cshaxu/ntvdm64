# Historical DOS C support headers — shard 3

Four files were selected from `shard-3/file-review.csv` with `review_class=pending`: `dos/v86/h/{assert,limits,malloc,share}.h`. `dos-c-headers-observer.py` and captured JSON compare each and related historical source/recipes to both original editions; all eight inspected files match both references. No guest, tool or product compilation was performed.

| file | proposed status | observed role and remaining boundary |
|---|---|---|
| `assert.h` | pending | Defines debug assertion/abort macro conditional on NDEBUG. Historical h2inc.c includes an `assert.h` by angle brackets, but its actual resolved compiler include path and NDEBUG mode have not been established. |
| `malloc.h` | pending | Historical C heap declarations/types. h2inc.c and COMMAND buildmsg.c include a file of this name; buildmsg's historical command makefile invokes buildmsg.exe to generate command.ctl. Include-name match is not proof these particular bytes were selected to build that tool, or that guest runtime heap uses this header. |
| `limits.h` | pending | Historical C integer limits reflect 16-bit unsigned int and 32-bit long. No direct exact include was found in examined v86 C/H source; transitive/toolchain includes and actual compiler choice remain unproved. Do not transplant its constants to a host build. |
| `share.h` | pending | Defines sopen file-sharing mode constants. No direct exact include was found in examined v86 C/H source; transitive/toolchain or other build use remains unproved. |

Original COMMAND makefile uses `buildmsg.exe` for command.ctl; it does not itself prove which buildmsg source, compiler or headers generated that executable. The current product uses immutable guest files and does not compile these headers as host product inputs, but neither their role nor guest C runtime capability can be excluded on that basis. Source comparison and name-only references are not provenance, executed behavior or a restoration percentage.
