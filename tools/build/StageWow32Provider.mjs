#!/usr/bin/env node
/* Publish the formal WOW32 provider beside the product worker. */
import crypto from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';

function fail(message) {
  process.stderr.write(`StageWow32Provider: ${message}\n`);
  process.exit(2);
}

function option(name) {
  const at = process.argv.indexOf(name);
  return at >= 0 ? process.argv[at + 1] : undefined;
}

const input = option('--input');
const repositoryRoot = path.resolve(option('--repository-root') || process.cwd());
const packageRoot = path.resolve(option('--package-root') || 'O:/ntvdm64');
if (!input) fail('--input is required');

const source = path.resolve(input);
if (!fs.existsSync(source)) fail(`input does not exist: ${source}`);
const image = fs.readFileSync(source);
const peOffset = image.readUInt32LE(0x3c);
if (image.subarray(peOffset, peOffset + 4).toString('ascii') !== 'PE\0\0') {
  fail(`input is not a PE image: ${source}`);
}
if (image.readUInt16LE(peOffset + 4) !== 0x14c) fail('input is not x86');
/* The provider must bind to the public staged worker, not a descriptive build
 * filename or an original-only module name. */
if (!image.includes(Buffer.from('ntvdm32.exe\0', 'ascii'))) {
  fail('provider does not import the public ntvdm32.exe worker identity');
}

const outputRoot = path.join(repositoryRoot, 'build', 'output');
const destinations = [
  path.join(outputRoot, 'WOW32.DLL'),
  path.join(packageRoot, 'WOW32.DLL'),
];
for (const destination of destinations) {
  fs.mkdirSync(path.dirname(destination), { recursive: true });
  fs.copyFileSync(source, destination);
}
const sha256 = value => crypto.createHash('sha256').update(fs.readFileSync(value)).digest('hex');
for (const destination of destinations) {
  process.stdout.write(`${destination}\t${fs.statSync(destination).size}\t${sha256(destination)}\n`);
}
