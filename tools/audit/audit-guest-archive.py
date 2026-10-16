"""Read-only NE dependency census; generated evidence stays below build/."""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import zipfile


def image(data):
    result = {"format": "DOS/data", "module": None, "imports": []}
    if len(data) < 64 or data[:2] != b"MZ":
        return result
    n = struct.unpack_from("<I", data, 60)[0]
    if n + 64 > len(data):
        return result
    if data[n:n+2] != b"NE":
        result["format"] = "PE" if data[n:n+4] == b"PE\0\0" else "DOS/data"
        return result
    result["format"] = "NE"
    def word(offset):
        return struct.unpack_from("<H", data, n + offset)[0]
    def name(offset):
        length = data[offset]
        return data[offset+1:offset+1+length].decode("ascii", errors="strict").upper()
    result["module"] = name(n + word(38))
    refs, names = n + word(40), n + word(42)
    result["imports"] = [name(names + struct.unpack_from("<H", data, refs+2*i)[0])
                         for i in range(word(30))]
    return result


def entry(path, data):
    return {"path": str(path), "bytes": len(data),
            "sha256": hashlib.sha256(data).hexdigest(), **image(data)}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", required=True)
    args = parser.parse_args()
    repo = Path(__file__).resolve().parents[2]
    output = Path(args.output).resolve()
    if not output.is_relative_to(repo / "build") or output.exists():
        raise ValueError("require fresh build-owned evidence file")
    archive = repo / "assets/winnt.zip"
    with zipfile.ZipFile(archive) as z:
        packaged = [entry(i.filename, z.read(i)) for i in z.infolist() if not i.is_dir()]
    candidates = []
    for tree in ("wow16", "bin86"):
        for p in (repo / "src/mvdm" / tree).rglob("*"):
            if p.is_file() and p.suffix.lower() in (".dll", ".exe", ".drv", ".fon"):
                candidates.append(entry(p, p.read_bytes()))
    # Exact accepted application fixtures only, never copy the installation tree.
    for name in ("WINMINE.EXE", "SOL.EXE", "WRITE.EXE", "TASKMAN.EXE", "WINVER.EXE",
                 "EDIT.COM", "QBASIC.EXE", "WINMINE.HLP", "SOL.HLP", "WRITE.HLP",
                 "EDIT.HLP", "QBASIC.HLP"):
        p = Path("O:/winnt") / name
        if p.is_file():
            candidates.append(entry(p, p.read_bytes()))
    modules = {e["module"] for e in packaged if e["module"]}
    missing = sorted({m for e in packaged for m in e["imports"] if m not in modules})
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps({"archive_sha256": hashlib.sha256(archive.read_bytes()).hexdigest(),
                                  "packaged": packaged, "missing_static_modules": missing,
                                  "candidates": candidates}, indent=2), encoding="utf-8")
    print(json.dumps({"missing_static_modules": missing,
                      "packaged": len(packaged), "candidates": len(candidates)}))


if __name__ == "__main__":
    main()
