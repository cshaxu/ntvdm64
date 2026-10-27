#include "basesrv.h"
#include <base_values.h>
#include <stdio.h>
#include <string.h>
int main(void) {
 BASE_API_MSG input, output; broker_vdm_check_values check; broker_vdm_get_values get;
 memset(&input,0,sizeof(input)); memset(&output,0,sizeof(output));
 input.u.CheckVDM.dwCreationFlags=0x2000;
 if (!OpenNtBaseEncodeValues(&input,BROKER_VDM_CHECK,&check,sizeof(check)) || check.creation_flags!=0x2000 || !OpenNtBaseDecodeValues(&check,sizeof(check),BROKER_VDM_CHECK,&output) || output.u.CheckVDM.dwCreationFlags!=0x2000) return 1;
 memset(&input,0,sizeof(input)); memset(&output,0,sizeof(output));
 input.u.GetNextVDMCommand.dwCreationFlags=0x2000;
 if (!OpenNtBaseEncodeValues(&input,BROKER_VDM_GET_NEXT,&get,sizeof(get)) || get.creation_flags!=0x2000 || !OpenNtBaseDecodeValues(&get,sizeof(get),BROKER_VDM_GET_NEXT,&output) || output.u.GetNextVDMCommand.dwCreationFlags!=0x2000) return 2;
 return 0;
}
