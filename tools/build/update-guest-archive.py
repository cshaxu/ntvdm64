"""Add owner-approved original guest inputs without rebuilding/replacing them.

Run in an owned background process. Existing members remain byte-identical.
The inventory is produced by tools/audit/audit-guest-archive.py beforehand.
"""
import argparse
import hashlib
import json
from pathlib import Path
import runpy
import shutil
import zipfile

REQUIRED = [
    "wow16/winsock/WINSOCK.DLL", "wow16/mmsystem/MMSYSTEM.DLL",
    "wow16/timer/TIMER.DRV", "wow16/toolhelp/TOOLHELP.DLL",
    "wow16/ole/server/retail/OLESVR.DLL", "wow16/mciole/MCIOLE16.DLL",
    "wow16/rasapi16/RASAPI16.DLL", "bin86/ctl3dv2.dll",
    "bin86/lanman.drv", "bin86/lzexpand.dll", "bin86/netapi.dll",
    "bin86/pmspl.dll", "bin86/ver.dll", "bin86/winhelp.exe",
    "bin86/i386/win87em.dll",
]
APPS = ["WINMINE.EXE", "SOL.EXE", "WRITE.EXE", "TASKMAN.EXE", "WINVER.EXE",
        "EDIT.COM", "QBASIC.EXE", "WINMINE.HLP", "SOL.HLP", "WRITE.HLP",
        "EDIT.HLP", "QBASIC.HLP"]


def digest(data):
    return hashlib.sha256(data).hexdigest()


def main():
    p = argparse.ArgumentParser()
    p.add_argument("--inventory", required=True)
    p.add_argument("--output", required=True)
    p.add_argument("--publish", action="store_true")
    a = p.parse_args()
    repo = Path(__file__).resolve().parents[2]
    out = Path(a.output).resolve()
    if not out.is_relative_to(repo / "build") or out.exists():
        raise ValueError("require fresh build-owned directory")
    inventory = json.loads(Path(a.inventory).read_text(encoding="utf-8"))
    archive = repo / "assets/winnt.zip"
    old_bytes = archive.read_bytes()
    if digest(old_bytes) != inventory["archive_sha256"]:
        raise ValueError("archive changed since audit")
    known = {str(Path(e["path"]).resolve()).lower(): e for e in inventory["candidates"]}
    additions = {}
    provenance = {}
    for source, target in [*(
            (repo / "src/mvdm" / name, "WINNT/SYSTEM32/" + Path(name).name.upper())
            for name in REQUIRED), *((Path("O:/winnt") / name, "WINNT/" + name) for name in APPS)]:
        data = source.read_bytes()
        record = known[str(source.resolve()).lower()]
        if digest(data) != record["sha256"] or record["format"] == "PE":
            raise ValueError(f"changed or native input: {source}")
        additions[target] = data
        provenance[target] = str(source.relative_to(repo)) if source.is_relative_to(repo) else str(source)
    out.mkdir(parents=True)
    shutil.copy2(archive, out / "winnt.before.zip")
    candidate = out / "winnt.zip"
    before = {}
    with zipfile.ZipFile(archive) as src, zipfile.ZipFile(candidate, "w", zipfile.ZIP_DEFLATED) as dst:
        for info in src.infolist():
            data = src.read(info)
            if info.filename.casefold() in {name.casefold() for name in before}:
                raise ValueError("duplicate baseline member")
            before[info.filename] = digest(data)
            dst.writestr(info, data)
        existing = {name.casefold() for name in before}
        for name, data in additions.items():
            if name.casefold() in existing:
                raise ValueError(f"refuse replacement: {name}")
            dst.writestr(name, data)
    parse = runpy.run_path(str(repo / "tools/audit/audit-guest-archive.py"))["image"]
    rows = []
    with zipfile.ZipFile(candidate) as z:
        if z.testzip() is not None:
            raise ValueError("archive CRC failure")
        for info in z.infolist():
            data = z.read(info)
            if info.filename in before and digest(data) != before[info.filename]:
                raise ValueError("baseline member changed")
            if info.is_dir():
                continue
            rows.append({"path": info.filename, "bytes": len(data), "sha256": digest(data),
                         "source": provenance.get(info.filename, "preserved-approved-baseline-zip"),
                         **parse(data)})
    modules = {r["module"] for r in rows if r["module"]}
    missing = sorted({m for r in rows for m in r["imports"] if m not in modules})
    if missing:
        raise ValueError(f"unresolved NE imports: {missing}")
    manifest = {"schema": "ntvdm64.guest-archive.v1", "baseline_sha256": digest(old_bytes),
                "archive_sha256": digest(candidate.read_bytes()), "added": sorted(additions),
                "missing_static_modules": missing, "files": rows,
                "acceptance": "byte-identical original inputs and static closure; not full runtime capability"}
    manifest_file = out / "winnt-guest-manifest.json"
    manifest_file.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    if a.publish:
        if digest(archive.read_bytes()) != digest(old_bytes):
            raise ValueError("baseline changed before publication")
        shutil.copy2(candidate, archive)
        shutil.copy2(manifest_file, repo / "assets/winnt-guest-manifest.json")
        if digest(archive.read_bytes()) != manifest["archive_sha256"]:
            raise ValueError("published archive mismatch; backup retained")
    print(json.dumps({"added": len(additions), "files": len(rows), "missing": missing,
                      "published": a.publish, "sha256": manifest["archive_sha256"]}))


if __name__ == "__main__":
    main()
