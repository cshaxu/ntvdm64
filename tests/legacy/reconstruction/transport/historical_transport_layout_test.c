#include <stdint.h>
#include <string.h>

#include "historical_transport_v2.h"

int main(void)
{
    app_historical_transport_v2_request request;
    app_historical_transport_v2_response response;

    memset(&request, 0, sizeof(request));
    memset(&response, 0, sizeof(response));
    request.magic = APP_HISTORICAL_TRANSPORT_V2_MAGIC;
    request.abi_version = APP_HISTORICAL_TRANSPORT_V2_ABI;
    request.struct_bytes = (uint32_t)sizeof(request);
    request.session_token = 0x13579bdfu;
    request.sequence = 1u;
    request.selector = 0x50u;
    request.service_byte = 0x14u;
    request.state.eax = 0xc0de5a00u;
    request.state.edi = 0x000008b0u;
    request.state.cs = 0x0070u;
    request.state.ip = 0x0478u;
    request.state.flags = 0x0002u;

    if (request.magic != APP_HISTORICAL_TRANSPORT_V2_MAGIC ||
        request.abi_version != APP_HISTORICAL_TRANSPORT_V2_ABI ||
        request.struct_bytes != 68u || request.session_token != 0x13579bdfu ||
        request.sequence != 1u || request.selector != 0x50u ||
        request.service_byte != 0x14u || request.state.edi != 0x8b0u ||
        request.state.cs != 0x70u || request.state.ip != 0x478u ||
        request.state.flags != 0x2u) {
        return 1;
    }

    response.magic = APP_HISTORICAL_TRANSPORT_V2_MAGIC;
    response.abi_version = APP_HISTORICAL_TRANSPORT_V2_ABI;
    response.struct_bytes = (uint32_t)sizeof(response);
    response.session_token = request.session_token;
    response.sequence = request.sequence;
    response.disposition = APP_HISTORICAL_TRANSPORT_V2_COMPLETED;
    response.state = request.state;
    response.state.ip = 0x0479u;
    if (response.struct_bytes != 72u || response.session_token != request.session_token ||
        response.sequence != request.sequence || response.state.ip != 0x479u ||
        response.state.edi != 0x8b0u) {
        return 2;
    }
    return 0;
}
