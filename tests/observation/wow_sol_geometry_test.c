#include "wow_sol_geometry.h"
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { printf("FAIL line %d\n", __LINE__); return 1; } } while (0)
int main(void)
{
    CHECK(sol_geometry_result(585,384,601,443,1300,740,1)==0);
    CHECK(sol_geometry_result(431,281,447,340,447,340,1)==1);
    CHECK(sol_geometry_result(431,384,447,443,447,740,1)==1);
    CHECK(sol_geometry_result(585,281,601,340,1300,340,1)==1);
    CHECK(sol_geometry_result(431,281,447,340,600,340,1)==2);
    CHECK(sol_geometry_result(431,281,447,340,447,500,1)==2);
    CHECK(sol_geometry_result(431,281,447,340,447,340,0)==2);
    puts("PASS SOL integration geometry classification");
    return 0;
}
