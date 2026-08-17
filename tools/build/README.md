# Build Tools

This directory contains tracked build entry points.  Each tool has one
declared responsibility and may create or consume only disposable output below
`build/<task-id>/<run-id>/`; it never publishes compiler output to `tools/` or
`artifacts/`.

`Invoke-DosBoxBatchWithCompletion.ps1` is the T225 S8 host-process wrapper for
an already staged DOS batch.  It neither interprets guest/build semantics nor
supplies tool arguments: the batch owns those.  The wrapper only mounts the
declared disposable stage in DOSBox and waits for its declared completion
marker.

Build-T225S8PrimaryNtio.ps1 reproduces the original NTIO tools16 closure in a caller-selected fresh stage: OpenNT BUILDIDX/NOSRVBLD, MASM, LINK16 and RELOC. It verifies the generated message classes and locked NTIO.SYS identity before writing the stage manifest.
