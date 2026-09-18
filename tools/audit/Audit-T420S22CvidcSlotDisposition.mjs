import fs from 'node:fs';
import path from 'node:path';

const [rootArgument = '.', mapArgument] = process.argv.slice(2);
if (!mapArgument) {
  throw new Error('usage: Audit-T420S22CvidcSlotDisposition.mjs <root> <current-formal-ntvdm.map>');
}

const root = path.resolve(rootArgument);
const read = (relative) => fs.readFileSync(path.join(root, relative), 'utf8');
const binding = read('build/M0-T420/S21/formal-x86-004/generated/cvidc_cpu_binding.inc');
const map = fs.readFileSync(path.resolve(root, mapArgument), 'utf8');
const adapterProviders = new Set([
  'mvdm_cvidc_get_jump_calibration',
  'mvdm_cvidc_get_jump_restart',
  'mvdm_cvidc_set_jump_restart',
]);

const slots = [...binding.matchAll(/^\s*X\(([A-Za-z0-9_]+),\s*([A-Za-z0-9_]+|0)\)\s*\\?$/gm)]
  .map(([, slot, provider]) => ({ slot, provider }));
if (slots.length !== 209) {
  throw new Error(`expected 209 selected C-VID CPU slots, found ${slots.length}`);
}

const nullSlots = slots.filter(({ provider }) => provider === '0').map(({ slot }) => slot);
const expectedNull = new Set([
  'ClearHwInt', 'InitIOS', 'DefineInb', 'DefineInw', 'DefineInd',
  'DefineOutb', 'DefineOutw', 'DefineOutd', 'SetSTATUS', 'SetAC', 'GetAC',
  'GetET', 'GetNE', 'GetWP', 'GetSadInfoTable', 'GetCpuState',
  'SetCpuState', 'InitNanoCpu', 'PrepareBlocksToCompile', 'SetRegConstraint',
  'GrowRecPool', 'BpiCompileBPI', 'TrashIntelRegisters',
  'FmDeleteAllStructures', 'ConstraintsFromUnivEpcPtr',
  'ConstraintsFromUnivHandle',
]);
if (nullSlots.length !== expectedNull.size ||
    nullSlots.some((slot) => !expectedNull.has(slot))) {
  throw new Error(`selected profile-null slots drifted: ${nullSlots.join(', ')}`);
}

const providerSlots = slots.filter(({ provider }) => provider !== '0');
const adapterSlots = providerSlots.filter(({ provider }) => adapterProviders.has(provider));
if (adapterSlots.length !== adapterProviders.size ||
    adapterSlots.some(({ provider }) => !adapterProviders.has(provider))) {
  throw new Error('C-VID adapter endpoint disposition drifted');
}

for (const { slot, provider } of providerSlots) {
  const symbol = `_${provider}`;
  if (!map.includes(symbol)) {
    throw new Error(`${slot}: provider ${provider} is not present in the current formal ntvdm map`);
  }
}

const binder = read('src/ntvdm-exe/softpc/mvdm_cvidc_vector_binding.c');
if (/static\s+IUH\s+mvdm_cvidc_jump_restart/.test(binder) ||
    !/extern\s+ULONG\s+qevJumpRestart\s*;/.test(binder)) {
  throw new Error('C-VID binder must not own a second quick-event restart value');
}

console.log(JSON.stringify({
  selected_slots: slots.length,
  original_provider_slots: providerSlots.length - adapterSlots.length,
  adapter_provider_slots: adapterSlots.length,
  profile_null_slots: nullSlots.length,
  profile_null: nullSlots,
  adapter_provider_names: adapterSlots.map(({ provider }) => provider),
}));
