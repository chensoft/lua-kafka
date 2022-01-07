#include "kafka.h"

static const luaL_Reg funcs[] = {
        {"producer", l_producer_create},
        {NULL, NULL}
};

int luaopen_kafka(lua_State *L)
{
    luaL_checkversion(L);
    luaL_newmetatable(L, "kafka");
    luaL_newlib(L, funcs);
    return 1;
}