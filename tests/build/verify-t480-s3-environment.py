"""Compare six actual Base client consumers after original structure recovery."""
import hashlib
import importlib.util
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.dont_write_bytecode = True
spec = importlib.util.spec_from_file_location(
    "restoration", Path(__file__).with_name("verify-t480-s3-restoration.py"))
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


def main():
    work = Path(sys.argv[1]).resolve()
    assert work.is_relative_to(ROOT / "build/t480-s3")
    phase = sys.argv[2] if len(sys.argv) > 2 else 'after'
    assert phase in ('after', 'final')
    digest = hashlib.sha256(
        (ROOT / "src/non-mvdm/base/win32/client/vdm.c").read_bytes()).hexdigest()
    checked = []
    for arch in ("x86", "x64"):
        before = module.read(work / f"vdm-before-{arch}/result.json")
        after = module.read(work / f"vdm-{phase}-{arch}/result.json")
        assert len(before) == len(after) == 3
        for old, new in zip(before, after):
            assert old["object"] == new["object"]
            assert old["command"] == new["command"]
            assert old["exit_code"] == new["exit_code"] == 0
            assert new["source_sha256"] == digest
            assert module.coff(work / f"vdm-before-{arch}" / old["file"]) == module.coff(
                work / f"vdm-{phase}-{arch}" / new["file"]), old["object"]
            checked.append(old["object"])
    (work / ("environment-test.json" if phase == "after" else "environment-final-test.json")).write_text(json.dumps({
        "passed": True, "source_sha256": digest, "objects": checked,
        "contract": "Identical code, data, relocations and symbols; metadata exclusions as documented by the COFF reader",
    }, indent=2))
    print("PASS all six actual Base client COFF contracts")


if __name__ == "__main__":
    main()
