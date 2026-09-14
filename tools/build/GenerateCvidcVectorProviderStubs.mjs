/* Test-only provider declarations for the focused C-VID binding link. */
import fs from 'node:fs';

const [output, binding] = process.argv.slice(2);
if (!output || !binding) throw new Error('usage: GenerateCvidcVectorProviderStubs.mjs <output> <binding.inc>');
const text = fs.readFileSync(binding, 'utf8');
const retained = new Set(['c_cpu_calc_q_ev_inst_for_time', 'c_sas_overwrite_memory']);
const providers = [...new Set([...text.matchAll(/X\([^,]+,\s*([A-Za-z_][A-Za-z0-9_]*)\)/g)]
  .map((entry) => entry[1])
  .filter((name) => !retained.has(name) && !name.startsWith('mvdm_cvidc_')))];
fs.writeFileSync(output, `/* Generated test-only inert provider symbols. */\n${providers.map((name) => `void ${name}(void) {}`).join('\n')}\n`);
