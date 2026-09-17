# BaseSrv boundary bindings

This directory contains only the reached worker-side COMMAND helpers:
image classification, copied guest-state capture and the bounded native-child
bridge. They are linked as `worker-command-bindings.lib`.

BaseSrv command records, DOS/WOW selection, re-entry and wait ownership are
the selected original `opennt-host/base/win32/server/srvvdm.c` implementation.
The retired local `base_vdm_client`, `base_vdm_local` and `base_vdm_broker`
providers are intentionally absent: they were a single-process replacement for
that original owner and are not a fallback runtime path.
