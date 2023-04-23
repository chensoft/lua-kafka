#include "kafka.h"

static const luaL_Reg funcs[] = {
        {"producer", l_producer_create},
        {"consumer", l_consumer_create},
        {NULL, NULL}
};

#ifdef _MSC_VER
__declspec(dllexport) int luaopen_kafka(lua_State *L)
#else
int luaopen_kafka(lua_State *L)
#endif
{
    luaL_checkversion(L);
    luaL_newmetatable(L, "kafka");
    luaL_newlib(L, funcs);
    return 1;
}