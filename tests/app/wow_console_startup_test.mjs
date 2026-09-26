// Source-boundary regression guard; this is not a WINMINE runtime test.
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';

const root = new URL('../../', import.meta.url);
const source = readFileSync(new URL('src/run16-exe/main.c', root), 'utf8');
const start = source.indexOf('static DWORD launch_vdm(');
const create = source.indexOf('if (!CreateProcessW(worker_path,', start);
assert(start >= 0 && create > start);
const beforeCreate = source.slice(start, create);
const match = beforeCreate.match(/if \(binary==BINARY_TYPE_WIN16 \|\| binary==BINARY_TYPE_SEPWOW\) \{([\s\S]*?)\} else if \(binary==BINARY_TYPE_DOS && task && !launcher_console_only\) \{([\s\S]*?)\}/);
assert(match, 'WOW-private and unchanged DOS-console branches required');
assert.match(match[1], /worker_creation_flags \|= CREATE_NO_WINDOW;/);
assert.doesNotMatch(match[1], /CREATE_NEW_CONSOLE|DETACHED_PROCESS|SW_HIDE|STARTF_USESHOWWINDOW/);
assert.match(match[2], /worker_creation_flags \|= CREATE_NEW_CONSOLE;/);
assert.doesNotMatch(match[2], /SW_HIDE|STARTF_USESHOWWINDOW/);
assert(beforeCreate.indexOf('BaseCheckVDM(') < match.index,
    'guest startup must be captured before worker-only visibility changes');
assert.doesNotMatch(beforeCreate.slice(0, match.index), /SW_HIDE/);
assert.doesNotMatch(match[1], /(?:^|\n)\s*startup\./,
    'do not hide the guest command startup record');
assert.match(source.slice(create, create + 320), /worker_creation_flags,/);
const wow = readFileSync(new URL('src/mvdm/wow32/wkman.c', root), 'utf8');
assert.match(wow, /VDMInfo\.StartupInfo\.dwFlags & STARTF_USESHOWWINDOW/);
assert.match(wow, /VDMInfo\.StartupInfo\.wShowWindow/);
console.log('PASS: WOW Console isolation flags; separate guest show state; DOS policy retained (source guard only)');
