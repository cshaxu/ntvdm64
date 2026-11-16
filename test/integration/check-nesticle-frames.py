"""Assert NESticle's rendered menu and Escape response, without guest mutation.

Menu glyph witness: CPU/View/Settings/Misc, x=40..199, y=0..7 at 320x200.
Pinned from the independently verified P102/candidate startup image, excluding
the animated cursor at the left. No ROM or redistributable executable is used.
"""
import hashlib
import json
from pathlib import Path
import struct
import sys
import zlib

MENU = "23cc9e02d7f682bd9963a2e85a4863c92c09967f8b7a6b6fcc1e0d4d75a3eeae"


def frame(path):
    data = path.read_bytes()
    magic, dimensions, depth, rgb = data.split(b"\n", 3)
    width, height = map(int, dimensions.split())
    if magic != b"P6" or depth != b"255" or width % 320 or height % 200:
        raise ValueError(f"Unexpected graphics frame: {path}")
    if width // 320 != height // 200 or len(rgb) != width * height * 3:
        raise ValueError(f"Invalid graphics extent: {path}")
    scale = width // 320
    mask = bytes(int(min(rgb[(y*scale*width+x*scale)*3:(y*scale*width+x*scale)*3+3]) > 120)
                 for y in range(8) for x in range(40, 200))
    digest = hashlib.sha256(mask).hexdigest()
    # Portable PNG output for the delivery evidence; no imaging dependency.
    def chunk(kind, payload):
        return (struct.pack(">I", len(payload)) + kind + payload +
                struct.pack(">I", zlib.crc32(kind + payload)))
    rows = b"".join(b"\0" + rgb[y*width*3:(y+1)*width*3] for y in range(height))
    png = (b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 2, 0, 0, 0)) +
           chunk(b"IDAT", zlib.compress(rows)) + chunk(b"IEND", b""))
    path.with_suffix(".png").write_bytes(png)
    return {"path": str(path), "width": width, "height": height,
            "menu_mask_sha256": digest, "menu_present": digest == MENU,
            "menu_hidden": not any(mask)}


def main():
    prefix = sys.argv[1]
    rows = [frame(Path(prefix + ".graphics-" + phase + ".ppm"))
            for phase in ("before", "hidden", "restored")]
    passed = rows[0]["menu_present"] and rows[1]["menu_hidden"] and rows[2]["menu_present"]
    Path(prefix + ".graphics-result.json").write_text(
        json.dumps({"pass": passed, "frames": rows}, indent=2) + "\n", encoding="utf-8")
    if not passed:
        raise ValueError("NESticle menu pixels or Escape hide/restore did not match")
    print("PASS NESticle rendered menu pixels and Escape hide/restore")


if __name__ == "__main__":
    main()
