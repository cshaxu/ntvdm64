// Pin the accepted load-only image and its original prompt exit control flow.
// This verifies artifact/source identity, not an alternative guest executor.
import fs from 'node:fs';
import assert from 'node:assert/strict';
import crypto from 'node:crypto';
import path from 'node:path';
const [imagePath, originalPath] = process.argv.slice(2);
assert(imagePath && originalPath, 'usage: node command_exit_image_test.mjs COMMAND.COM original-tcode.asm');
const image = fs.readFileSync(imagePath);
const original = fs.readFileSync(originalPath);
const mirror = fs.readFileSync('src/mvdm-guest/dos/v86/cmd/command/tcode.asm');
assert(original.equals(mirror), 'selected tcode.asm must remain byte-exact OpenNT');
for (const name of ['command1.asm', 'command2.asm']) {
  assert(fs.readFileSync(path.join(path.dirname(originalPath), name)).equals(
    fs.readFileSync(`src/mvdm-guest/dos/v86/cmd/command/${name}`)),
  `${name} must remain byte-exact OpenNT`);
}
assert.equal(crypto.createHash('sha256').update(image).digest('hex'),
  '908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43');
function bytes(offset, hex, label) {
  const expected = Buffer.from(hex.replaceAll(' ', ''), 'hex');
  assert(image.subarray(offset, offset + expected.length).equals(expected), label);
}
// File offsets, verified against the selected image's i8086 disassembly.
bytes(0x2ca3, 'b0 00 b4 01', 'exec_comspec: AL=Start16(0), AH=FOR_SHELLOUT');
bytes(0x2d1b, 'b0 01 b4 01 e8 6e 00', 'reent_ret: AL=Return16(1), call Do16BitPrompt');
bytes(0x2d90, '06 50', 'Do16BitPrompt saves ES and entry AX');
bytes(0x2dc3, '0a c0 74 7e', 'recognized EXIT branches to d16_exit');
bytes(0x2e33, '26 c7 06 a7 02 00 00', 'child return clears guest RetCode');
bytes(0x2e45, '58 80 fc 00 74 03 e9 01 ff', 'd16_exit restores entry AX, jumps to reent_exit');
bytes(0x2d4f, '50', 'reent_exit preserves restored AX during cleanup');
bytes(0x2d8b, '58 b4 4c cd 21', 'INT 21h/4Ch exits with restored AL, not RetCode');
bytes(0xd4b, 'b4 4d cd 21 a3 a7 02', 'Exec_Wait obtains and stores actual child RetCode');
bytes(0xf79, '83 3e b1 02 ff 75 03 e9 cb fe', 'LodCom1: SingleCom=-1 jumps to FatalRet2');
bytes(0xe67, 'b8 00 4c cd 21', 'FatalRet2 explicitly exits zero, even with nonzero RetCode');
console.log('PASS: original prompt Start16=0 / Return16=1; resident /c exit=0; no normalization');
