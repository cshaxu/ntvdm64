# OLE server debug and retail image identities

The [observer](ole-server-image-observer.py) and [receipt](ole-server-image-observations.json) compare the retained debug and retail `OLESVR.DLL` images with both original source editions. The debug and retail files differ. The guest ZIP contains the exact retail image at `WINNT/SYSTEM32/OLESVR.DLL`; the exact installed `O:/winnt/system32/OLESVR.DLL` path was absent at this observation. The historic server makefile builds and binplaces an OLE server DLL, but does not prove that recipe produced the archived bytes.

The retail image is a partial guest-package identity, not proof of installation, actual application lookup, OLE server dispatch, object lifetime or error handling. The debug image remains pending: a retail archive match does not establish the debug profile's role. Search paths, dependencies and actual guest behavior need separate verification; do not replace or rebuild guest files.
