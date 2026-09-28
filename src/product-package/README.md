# product-package

This root contains worker startup package/media configuration and its fixture.
Its API depends on ntvdm-exe/session and updates worker-owned media roots; it is
not a cross-executable shared package service. S10 proposes moving this binding
into ntvdm-exe without changing package/search policy.
