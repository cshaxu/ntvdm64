/* Actual checked adapter/session contract, not a CCPU/guest replacement.
 * Keep the prior mapping/observation matrix and add the complete inactive
 * alias predicate, nonempty owner separation and a non-gating microbenchmark. */
#define main physical_observation_contract
#include "physical_mapping_observation_test.c"
#undef main

static int unchanged(uint32_t address)
{
    uint32_t output = 0xdeadbeefu;
    SetLastError(0x74478301u);errno = EDOM;
    CHECK(!mvdm_softpc_physical_mapping_translate(address, &output));
    CHECK(output == address && GetLastError() == 0x74478301u && errno == EDOM);
    CHECK(!mvdm_softpc_physical_mapping_translate(address, NULL));
    CHECK(GetLastError() == 0x74478301u && errno == EDOM);
    return 0;
}

int main(void)
{
    const uint32_t probes[] = {0u, 0xfffffu, 0x100000u, 0x101fffu,
        0x102000u, 0x7fffffffu, UINT32_MAX};
    session first, second;
    uint32_t output, magic, i, invalid = 0;
    LARGE_INTEGER start, stop, frequency;
    CHECK(physical_observation_contract(1, NULL) == 0);
    for (i = 0; i < sizeof(probes) / sizeof(probes[0]); ++i)
        CHECK(unchanged(probes[i]) == 0); /* Empty and unbound. */
    session_initialize(&first, 44783u);session_initialize(&second, 44784u);
    CHECK(session_activate(&first) && session_activate(&second));
    CHECK(session_thread_bind(&first));
    for (i = 0; i < sizeof(probes) / sizeof(probes[0]); ++i)
        CHECK(unchanged(probes[i]) == 0);
    magic = first.magic;first.magic = 0;
    CHECK(unchanged(0x100000u) == 0);first.magic = magic;
    CHECK(VdmMapDosMemory(0x100u, 0x200u, 2u) == 0);
    CHECK(unchanged(0xfffffu) == 0 && unchanged(0x102000u) == 0);
    CHECK(mvdm_softpc_physical_mapping_translate(0x100000u, &output) && output == 0x200000u);
    CHECK(mvdm_softpc_physical_mapping_translate(0x101fffu, &output) && output == 0x201fffu);
    magic = first.magic;first.magic = 0;
    CHECK(unchanged(0x100000u) == 0);first.magic = magic;
    CHECK(session_thread_unbind(&first));
    CHECK(unchanged(0x100000u) == 0); /* Nonempty, no bound owner. */
    CHECK(session_thread_bind(&second));
    CHECK(unchanged(0x100000u) == 0); /* Nonempty, different owner. */
    CHECK(VdmMapDosMemory(0x500u, 0x600u, 1u) == 0);
    CHECK(mvdm_softpc_physical_mapping_translate(0x50000fu, &output) && output == 0x60000fu);
    CHECK(session_thread_unbind(&second) && session_thread_bind(&first));
    CHECK(unchanged(0x50000fu) == 0);
    CHECK(VdmMapDosMemory(0x100u, 0x300u, 2u) == 0);
    CHECK(mvdm_softpc_physical_mapping_translate(0x101fffu, &output) && output == 0x301fffu);
    CHECK(session_thread_unbind(&first) && session_thread_bind(&second));
    CHECK(session_dispose(&first)); /* Drops only first's pending alias. */
    CHECK(unchanged(0x100000u) == 0);
    CHECK(mvdm_softpc_physical_mapping_translate(0x50000fu, &output) && output == 0x60000fu);
    CHECK(VdmUnmapDosMemory(0x500u, 1u) == 0);
    CHECK(VdmUnmapDosMemory(0x500u, 1u) == (int32_t)0xc0000225u);
    CHECK(unchanged(0x50000fu) == 0);
    CHECK(QueryPerformanceFrequency(&frequency) && QueryPerformanceCounter(&start));
    for (i = 0; i < 2000000u; ++i) {
        uint32_t address = 0x100000u + (i & 4095u);
        invalid |= (uint32_t)mvdm_softpc_physical_mapping_translate(address, &output);
        invalid |= output ^ address;
    }
    CHECK(QueryPerformanceCounter(&stop));CHECK(!invalid);
    printf("EMPTY_ALIAS_BENCH iterations=2000000 elapsed_us=%.3f ns_per_call=%.3f invalid_results=%lu\n",
        (double)(stop.QuadPart-start.QuadPart)*1000000.0/frequency.QuadPart,
        (double)(stop.QuadPart-start.QuadPart)*1000000000.0/frequency.QuadPart/2000000.0,
        (unsigned long)invalid);
    CHECK(session_thread_unbind(&second) && session_dispose(&second));
    CHECK(unchanged(UINT32_MAX) == 0);
    puts("PHYSICAL_ALIAS_EMPTY_CONTRACT errors=0 empty=1 null=1 invalid_owner=1 bounds=1 multi_owner=1 remap=1 teardown=1 physical_retained=1");
    return 0;
}
