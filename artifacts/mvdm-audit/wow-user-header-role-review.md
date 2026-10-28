# WOW32 USER declaration roles

Five original headers are included by the selected WOW32 build and their
representative provider symbols appear in the current map; they remain
partial. Clipboard, communications, dialogs, mouse input and placement require
separate guest-call, callback, error and object-lifetime verification. See the
[per-file source review](shard-2/wow32-user-header-review.md) for each scope.

Dependency and map presence do not prove every declaration is used or that an
actual guest call reaches its provider. No product or guest image was changed.
