# WOW multimedia bridge and resource thunk image binding

The [observer](wow-media-resource-image-observer.py) and [snapshot](wow-media-resource-image-observations.json) bind four original multimedia and six resource function bodies to released WOW32 executable code, masking relocation words. The recorded source files match both original editions. This proves selected function-body identity only, not actual guest dispatch or lifetime behavior.

`wmmtbl2.h` registers `WMM32CallProc32`; the current project extension wraps the original renamed body, so the original body and project callback scope have separate responsibilities. Other bound bodies are the driver callback and memory/handle resolvers. Verify real Win16 callbacks, asynchronous devices, guest addresses, aliases and task retirement; existing synchronous fixtures do not cover them.

`wres32.c` selects original resource operations, dispatching guest resources and native resources through different providers. Project menu resource bindings reference these operations. Guest/native handle identity, invalid or reused resources, language variants, free/lock ordering and task exit still require real caller and teardown tests. A bound body does not establish that the relevant dispatch table reaches it.
