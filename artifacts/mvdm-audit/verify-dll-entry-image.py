"""Read-only selected bridge body correspondence, not DLL lifecycle proof."""
import argparse
import hashlib
import json
import sys
from pathlib import Path

sys.dont_write_bytecode = True
sys.path.insert(0,str(Path(__file__).resolve().parents[2] / 'tests/observation'))
from verify_ccpu_sign_flag_image import Coff, Image

parser = argparse.ArgumentParser()
parser.add_argument('--object',type=Path,required=True)
parser.add_argument('--image',type=Path,required=True)
parser.add_argument('--output',type=Path,required=True)
args = parser.parse_args()
obj, image = Coff(args.object), Image(args.image)
body, relocs = obj.function('_DllMain@12')
assert all(kind == 20 for _, _, kind in relocs)
matches = image.find(body,[i for offset,_,_ in relocs for i in range(offset,offset+4)])
assert len(matches) == 1
result = dict(result='pass',object_sha256=hashlib.sha256(obj.data).hexdigest().upper(),
              image_sha256=hashlib.sha256(image.data).hexdigest().upper(),
              function_bytes=len(body),matched_image_va=image.va(matches[0]),
              masked_relative_calls=[dict(offset=offset,symbol=symbol) for offset,symbol,_ in relocs],
              scope='Complete bridge body with call relocations masked; not original attach failure or full loader behavior')
args.output.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
print(json.dumps(result))
