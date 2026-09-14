#include "vdm_payload.h"
#include "vdm_message.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { printf("FAIL line %d: %s\n",__LINE__,#x); return 1; } } while (0)
int main(void)
{
    {
        broker_vdm_message_header request={BROKER_VDM_MESSAGE_VERSION,32,BROKER_VDM_CHECK,1,7,0,0,0};
        broker_vdm_message_header decoded,saved;
        uint32_t op;
        for (op=BROKER_VDM_CHECK;op<=BROKER_VDM_WOWEXEC;++op) {
            request.operation=op;
            CHECK(broker_vdm_message_read(&request,sizeof(request),7,0,&decoded));
        }
        memset(&decoded,0xa5,sizeof(decoded)); saved=decoded;
        CHECK(!broker_vdm_message_read(&request,31,7,0,&decoded));
        CHECK(!broker_vdm_message_read(&request,32,8,0,&decoded));
        CHECK(!broker_vdm_message_read(&request,32,7,1,&decoded));
        request.version=2; CHECK(!broker_vdm_message_read(&request,32,7,0,&decoded)); request.version=1;
        request.payload_bytes=UINT32_MAX; CHECK(!broker_vdm_message_read(&request,32,7,0,&decoded)); request.payload_bytes=0;
        request.operation=0; CHECK(!broker_vdm_message_read(&request,32,7,0,&decoded)); request.operation=BROKER_VDM_CHECK;
        request.request_id=0; CHECK(!broker_vdm_message_read(&request,32,7,0,&decoded)); request.request_id=1;
        request.status=5; CHECK(!broker_vdm_message_read(&request,32,7,0,&decoded));
        CHECK(!memcmp(&decoded,&saved,sizeof(saved)));
        request.reply=1; CHECK(broker_vdm_message_read(&request,32,7,1,&decoded) && decoded.status==5);
        puts("PASS: eleven operation envelopes, version/length/direction/generation negatives and explicit reply status");
    }
    broker_vdm_payload_input input[BROKER_VDM_PAYLOAD_FIELDS]={0};
    unsigned char output[256], saved[256];
    broker_vdm_payload_span span;
    uint32_t required, i;
    const char text[]="COMMAND.COM\r\n";
    input[BROKER_VDM_COMMAND].present=1;
    input[BROKER_VDM_COMMAND].length=sizeof(text);
    input[BROKER_VDM_COMMAND].data_bytes=sizeof(text);
    input[BROKER_VDM_COMMAND].data=text;
    /* Present output buffer with no copied input, and NULL buffer with a
     * nonzero native length, must not collapse into one representation. */
    input[BROKER_VDM_TITLE].present=1;
    input[BROKER_VDM_TITLE].length=1000;
    input[BROKER_VDM_RESERVED].length=77;
    CHECK(broker_vdm_payload_encode(input,NULL,0,&required));
    CHECK(required==128+sizeof(text));
    memset(output,0xa5,sizeof(output));
    memcpy(saved,output,sizeof(output));
    CHECK(!broker_vdm_payload_encode(input,output,required-1,&required));
    CHECK(!memcmp(output,saved,sizeof(output)));
    CHECK(broker_vdm_payload_encode(input,output,sizeof(output),&required));
    CHECK(broker_vdm_payload_validate(output,required));
    CHECK(!memcmp(output+128,text,sizeof(text)));
    memcpy(&span,output+16*BROKER_VDM_TITLE,16);
    CHECK(span.present==1 && span.length==1000 && !span.data_bytes && !span.offset);
    memcpy(&span,output+16*BROKER_VDM_RESERVED,16);
    CHECK(!span.present && span.length==77 && !span.data_bytes);
    memcpy(saved,output,sizeof(output));
    for (i=0;i<required;++i) CHECK(!broker_vdm_payload_validate(output,i));
    CHECK(!broker_vdm_payload_validate(output,required+1));
    memcpy(&span,output,16);
    span.offset=127; memcpy(output,&span,16);
    CHECK(!broker_vdm_payload_validate(output,required));
    span.offset=128; span.data_bytes=UINT32_MAX; span.length=UINT32_MAX;
    memcpy(output,&span,16);
    CHECK(!broker_vdm_payload_validate(output,required));
    memcpy(output,saved,sizeof(output));
    input[0].length=UINT32_MAX; input[0].data_bytes=UINT32_MAX;
    CHECK(!broker_vdm_payload_encode(input,output,sizeof(output),&required));
    CHECK(required==0 && !memcmp(output,saved,sizeof(output)));
    input[0].length=sizeof(text); input[0].data_bytes=sizeof(text); input[0].present=0;
    CHECK(!broker_vdm_payload_encode(input,output,sizeof(output),&required));
    input[0].present=1; input[0].data=NULL;
    CHECK(!broker_vdm_payload_encode(input,output,sizeof(output),&required));
    for (i=0;i<BROKER_VDM_PAYLOAD_FIELDS;++i) {
        input[i].present=1; input[i].length=sizeof(text);
        input[i].data_bytes=sizeof(text); input[i].data=text;
    }
    CHECK(broker_vdm_payload_encode(input,output,sizeof(output),&required));
    CHECK(broker_vdm_payload_validate(output,required));
    for (i=0;i<BROKER_VDM_PAYLOAD_FIELDS;++i) {
        memcpy(&span,output+16*i,16);
        CHECK(span.offset==128+i*sizeof(text));
        CHECK(!memcmp(output+span.offset,text,sizeof(text)));
    }
    memcpy(&span,output+16,16); span.offset=128; memcpy(output+16,&span,16);
    CHECK(!broker_vdm_payload_validate(output,required));
    input[0].present=2;
    CHECK(!broker_vdm_payload_encode(input,output,sizeof(output),&required));
    input[0].present=1; input[0].length=0;
    CHECK(broker_vdm_payload_encode(input,output,sizeof(output),&required));
    CHECK(broker_vdm_payload_validate(output,required));
    puts("PASS: all eight copied VDM fields, independent presence/capacity, short-output nonmutation, truncation/overlap/overflow rejection");
    return 0;
}
