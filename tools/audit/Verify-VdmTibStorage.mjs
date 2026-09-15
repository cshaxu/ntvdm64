import fs from 'node:fs';
import assert from 'node:assert/strict';

// Inspect the actual MSVC x86 product, not merely the source declaration.
const [mapPath, objectPath] = process.argv.slice(2);
assert(mapPath && objectPath, 'usage: node Verify-VdmTibStorage.mjs product.map mvdm_vdm_tib.obj');
const map = fs.readFileSync(mapPath, 'utf8');
const bytes = fs.readFileSync(objectPath);
assert.equal(bytes.readUInt16LE(0), 0x14c, 'requires x86 COFF');
const symbolTable = bytes.readUInt32LE(8);
const symbolCount = bytes.readUInt32LE(12);
const strings = symbolTable + symbolCount * 18;
let storage;
for (let i = 0; i < symbolCount;) {
  const offset = symbolTable + i * 18;
  const nameOffset = bytes.readUInt32LE(offset) === 0
    ? strings + bytes.readUInt32LE(offset + 4) : offset;
  const limit = nameOffset === offset ? offset + 8 : bytes.length;
  let end = nameOffset;
  while (end < limit && bytes[end]) ++end;
  const name = bytes.toString('ascii', nameOffset, end);
  if (name === '_VdmTib') {
    const section = bytes.readInt16LE(offset + 12);
    assert(section > 0, 'VdmTib must be a real definition, not COMMON/undefined');
    assert.equal(bytes.readUInt32LE(offset + 8), 0, 'storage starts its section');
    const header = 20 + bytes.readUInt16LE(16) + (section - 1) * 40;
    storage = bytes.readUInt32LE(header + 16);
  }
  i += 1 + bytes[offset + 17];
}
assert(storage > 0xfc0, 'full VDM_TIB storage must include printer fields');
const row = map.split(/\r?\n/).find(line => /\s_VdmTib\s/.test(line));
assert(row && /mvdm_vdm_tib\.obj/.test(row) && !row.includes('<common>'),
  'product must select mvdm_vdm_tib.obj, never stubs.c COMMON storage');
const address = Number.parseInt(row.trim().split(/\s+/)[2], 16);
for (const line of map.split(/\r?\n/)) {
  const match = /^\s+\w+:\w+\s+(\S+)\s+([0-9a-f]{8})\s/i.exec(line);
  if (!match || match[1] === '_VdmTib') continue;
  const other = Number.parseInt(match[2], 16);
  assert(other < address || other >= address + storage,
    `${match[1]} overlaps VdmTib storage`);
}
assert(/\s_mvdm_monitor_bind_current_thread\s/.test(map), 'startup binding must be linked');
console.log(`PASS: VdmTib owner=mvdm_vdm_tib.obj bytes=${storage} address=0x${address.toString(16)}; no overlapping symbol`);
