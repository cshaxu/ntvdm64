# Original CGA/Hercules display interfaces — shard 3

Two currently pending entries are `softpc.new/host/inc/nt_cga.h` and `softpc.new/base/inc/herc.h`. `cga-herc-interface-observer.py` and its captured JSON compare five source/header files with both original editions, normalize retained Ninja C-dependency paths against the worker root, and record map and gvi object symbol evidence. Both headers, gvi.c and nt_cga.c match both references; current nt_graph.c differs. No build, guest edit or display interaction was performed.

`nt_cga.h` declares historical CGA character and window dimensions. `host/src/nt_graph.c:653,1700-1703,1895-1896` uses those dimensions when configuring or scaling a CGA display; `host/src/nt_cga.c` also includes the header. Normalized snapshot dependencies identify exactly those two selected object inputs. The observed worker map identifies original `nt_cga_colour_hi_graph_std` and `nt_init_cga_colour_hi_graph` in nt_cga.obj, and gvi_init/gvi_term in gvi.obj. Source dimensions and linked functions do not prove pixel output, mode changes or frontend cleanup.

`herc.h` declares Hercules init/term and port functions. Original `base/video/gvi.c:69-71,195-201,220-225,268-273,310-316` includes and calls them only under HERC. The normalized retained dependency snapshot has no exact herc.h line; current gvi.obj symbol table shows references to cga_init and vga_init but none to herc_init or herc_term. This is evidence for the observed object, not proof that Hercules is out of scope or that all alternate profiles have no implementation. Its capability decision requires the product policy and genuine selected/alternate provider evidence.

| file | proposed status | remaining proof |
|---|---|---|
| `nt_cga.h` | pending | selected source dependency and representative linked bodies established; actual CGA mode and rendering, failure, reset/adapter transitions and guest-visible semantics remain to verify. |
| `herc.h` | pending | historical conditional interface; observed object has no HERC references. Product support expectation and any other actual provider/guest execution must be assessed before exclusion. |

The retained dependency snapshot, observed map and object hashes are in JSON. No claim ties them to the latest released binary; graph observation is not execution. No restoration percentage is inferred.
