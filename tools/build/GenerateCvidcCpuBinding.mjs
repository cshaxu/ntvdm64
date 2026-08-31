/* Recover an omitted generated CCPU-to-C-VID binding from original metadata. */
import fs from 'node:fs';
import path from 'node:path';

const [output, sourceRoot] = process.argv.slice(2);
if (!output || !sourceRoot) throw new Error('usage: GenerateCvidcCpuBinding.mjs <output> <softpc-base-root>');
const metadata = fs.readFileSync(path.join(sourceRoot, 'cvidc', 'c2cpusad.h'), 'utf8');
const groups = [...metadata.matchAll(/char\s*\*\s*(\w+)\[\]\s*=\s*\{([\s\S]*?)\};/g)];
const slots = Object.fromEntries(groups.slice(0, 2).map(([, name, body]) => [
  name, [...body.matchAll(/"([A-Za-z0-9_]+)"/g)].map((match) => match[1]),
]));
const publicSpecial = {
  Simulate: 'c_cpu_simulate', Interrupt: 'c_cpu_interrupt', ClearHwInt: 'c_cpu_clearHwInt', EndOfApplication: 'c_cpu_EOA_hook', Terminate: 'c_cpu_terminate', Initialise: 'c_cpu_init',
  SetQuickEventCount: 'c_cpu_q_ev_set_count', GetQuickEventCount: 'c_cpu_q_ev_get_count', CalcQuickEventInstTime: 'c_cpu_calc_q_ev_inst_for_time', InitIOS: 'c_cpu_init_ios_in',
  DefineInb: 'c_cpu_define_inb', DefineInw: 'c_cpu_define_inw', DefineInd: 'c_cpu_define_ind', DefineOutb: 'c_cpu_define_outb', DefineOutw: 'c_cpu_define_outw', DefineOutd: 'c_cpu_define_outd',
  GetJumpCalibrateVal: 'mvdm_cvidc_get_jump_calibration', GetJumpInitialVal: 'mvdm_cvidc_get_jump_restart', SetJumpInitialVal: 'mvdm_cvidc_set_jump_restart', SetEOIEnable: 'setEOIEnableAddr',
  SetAddProfileData: 'setAddProfileDataPtr', SetMaxProfileData: 'setMaxProfileDataAddr', GetAddProfileDataAddr: 'getAddProfileDataAddr', PurgeLostIretHookLine: 'PurgeLostIretHookLine',
  ClearHwInt: '0', InitIOS: '0', DefineInb: '0', DefineInw: '0', DefineInd: '0', DefineOutb: '0', DefineOutw: '0', DefineOutd: '0',
  SetSTATUS: '0', SetAC: '0', GetAC: '0', GetET: '0', GetNE: '0', GetWP: '0',
};
const privateSpecial = {
  InitNanoCpu: 'c_InitNanoCpu', PrepareBlocksToCompile: 'c_PrepareBlocksToCompile',
  GrowRecPool: '0', BpiCompileBPI: '0', FmDeleteAllStructures: '0',
  GetSadInfoTable: '0', GetCpuState: '0', SetCpuState: '0', SetRegConstraint: '0', TrashIntelRegisters: '0',
  InitNanoCpu: '0', PrepareBlocksToCompile: '0', ConstraintsFromUnivEpcPtr: '0', ConstraintsFromUnivHandle: '0',
};
const provider = (name, special) => special[name] ?? `c_${name[0].toLowerCase()}${name.slice(1)}`;
const emit = (macro, names, special) => {
  const slash = String.fromCharCode(92);
  return `#define ${macro}(X) ${slash}\n${names.map((name, index) => `    X(${name}, ${provider(name, special)})${index + 1 === names.length ? '' : ` ${slash}\n`}`).join('')}\n`;
};
const text = [
  '/* Generated from original cvidc/c2cpusad.h. Do not edit. */\n',
  `/* public=${slots.CpuVectorNames.length}; private=${slots.CpuPrivateVectorNames.length} */\n`,
  emit('MVDM_CVIDC_CPU_PUBLIC_SLOTS', slots.CpuVectorNames, publicSpecial),
  emit('MVDM_CVIDC_CPU_PRIVATE_SLOTS', slots.CpuPrivateVectorNames, privateSpecial),
].join('');
fs.mkdirSync(path.dirname(output), { recursive: true });
if (!fs.existsSync(output) || fs.readFileSync(output, 'utf8') !== text) fs.writeFileSync(output, text);
