#!/usr/bin/env node
/*
 * Publish one architecture-qualified, formally linked NTVDM product binary.
 * This is deliberately a post-link operation: formal build roots remain
 * disposable, while build/output and the fixed local package root carry only
 * the two public product names.
 */
import crypto from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';

function fail(message) {
  process.stderr.write(`StageProductExecutable: ${message}\n`);
  process.exit(2);
}

function option(name) {
  const at = process.argv.indexOf(name);
  return at >= 0 ? process.argv[at + 1] : undefined;
}

const architecture = option('--architecture');
const input = option('--input');
const repositoryRoot = path.resolve(option('--repository-root') || process.cwd());
const packageRoot = path.resolve(option('--package-root') || 'O:/ntvdm64');

if (!['x86', 'x64'].includes(architecture)) fail('--architecture must be x86 or x64');
if (!input) fail('--input is required');

const source = path.resolve(input);
if (!fs.existsSync(source)) fail(`input does not exist: ${source}`);

const image = fs.readFileSync(source);
const peOffset = image.readUInt32LE(0x3c);
if (image.subarray(peOffset, peOffset + 4).toString('ascii') !== 'PE\0\0') {
  fail(`input is not a PE image: ${source}`);
}
const machine = image.readUInt16LE(peOffset + 4);
const expectedMachine = architecture === 'x86' ? 0x14c : 0x8664;
if (machine !== expectedMachine) {
  fail(`input PE machine 0x${machine.toString(16)} does not match ${architecture}`);
}

const productName = architecture === 'x86' ? 'ntvdm32.exe' : 'ntvdm64.exe';
const outputRoot = path.join(repositoryRoot, 'build', 'output');
const destinations = [
  path.join(outputRoot, productName),
  path.join(packageRoot, productName),
];

/* Runtime diagnostics belong under logs/, never beside published media and
 * executables. Creating the directory is harmless for a fresh package root. */
fs.mkdirSync(path.join(packageRoot, 'logs'), { recursive: true });

for (const destination of destinations) {
  fs.mkdirSync(path.dirname(destination), { recursive: true });
  fs.copyFileSync(source, destination);
}

/* These names are never product aliases. Remove only exact former publication
 * names; disposable build-run outputs are intentionally left alone. */
for (const root of [outputRoot, packageRoot]) {
  for (const retiredName of ['ntvdm.exe', 'ntvdm-child-route.exe']) {
    const retired = path.join(root, retiredName);
    if (fs.existsSync(retired)) fs.unlinkSync(retired);
  }
}

const sha256 = value => crypto.createHash('sha256').update(fs.readFileSync(value)).digest('hex');
for (const destination of destinations) {
  process.stdout.write(`${destination}\t${fs.statSync(destination).size}\t${sha256(destination)}\n`);
}
