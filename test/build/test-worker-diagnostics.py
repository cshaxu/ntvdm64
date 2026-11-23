"""Check native diagnostic link selections; this script never builds binaries."""
import argparse
import hashlib
import json
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument("--root", type=Path, required=True)
args = parser.parse_args()
variants = {
    "environment": ["command_environment_trace"],
    "wow-environment": ["wow_initial_environment_trace"],
    "command-dispatch": ["command_dispatch_trace"],
    "thread-exit": ["wow_thread_exit_trace"],
    "exception": ["dpmi_exception_trace"],
    "dpmi": ["dpmi_dispatch_trace"],
    "dpmi-lifecycle": ["dpmi_dispatch_trace"],
    "pm-allocation-failure": ["dpmi_pm_allocation_failure"],
    "dpmi-pointer": ["dpmi_pointer_trace"],
    "dpmi-ioctl": ["dpmi_ioctl_boundary"],
    "dpmi-xmem-failure": ["dpmi_xmem_failure", "dpmi_dispatch_trace"],
    "command-client": ["ntvdm_command_client_diagnostic"],
}
results = []
for name, owners in variants.items():
    image = args.root / name / "ntvdm.exe"
    data = image.read_bytes()
    assert data[:2] == b"MZ", name
    mapping = image.with_suffix(".exe.map").read_text()
    for owner in owners:
        assert f":{owner}.obj" in mapping, (name, owner)
    if name == "command-client":
        for retained in ("frontend_rpc_client", "base_dispatch"):
            assert f":{retained}.obj" in mapping, (name, retained)
    results.append({"variant": name, "owners": owners,
                    "sha256": hashlib.sha256(data).hexdigest()})
for name, entry in {
    "win101-guard": "win101_guard_entry",
    "win31-guard": "win101_guard_entry",
    "win101-palette": "win101_palette_trace_entry",
    "win31-console": "win31_console_trace_entry",
}.items():
    image = args.root / name / "ntvdm.exe"
    data = image.read_bytes()
    assert data[:2] == b"MZ", name
    mapping = image.with_suffix(".exe.map").read_text()
    assert f"_{entry}" in mapping, (name, entry)
    assert "observer.obj" in mapping, name
    assert "startup_hooks.obj" in mapping, name
    results.append({"variant": name, "entry": entry,
                    "sha256": hashlib.sha256(data).hexdigest()})
print(json.dumps({"linked_variants": len(results), "runtime_tested": False,
                  "results": results}, indent=2))
