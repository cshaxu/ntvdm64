# DOS and Win16 guest package image family

The [observer](guest-package-image-family-observer.py) and [receipt](guest-package-image-family-observations.json) compare five retained original guest files against both source editions and exact guest ZIP members. All five match the archive. FASTOPEN.EXE and WOWEXEC.EXE also match the exact installed System32 copies. The inspected System32 paths for MMSYSTEM.DLL, RASAPI16.DLL and WINSOCK.DLL were absent; this is not an exhaustive search or proof of a load failure.

The `wow16/test/shell/WOWEXEC.EXE` file matches the package and installed WOWEXEC bytes despite its source-tree directory name; that identity does not prove that the adjacent test source produced it. The five roles are partial image identity only. DOS cache behavior, multimedia, RAS and socket providers, guest loader choice, callbacks, errors and teardown require their independent original owner and runtime evidence. No guest or installation was changed.
