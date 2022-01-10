#include "kafka.h"

static rd_kafka_t* l_producer_retrieve(lua_State *L)
{
    rd_kafka_t **rk = luaL_checkudata(L, 1, "kafka.producer");
    if (!*rk)
        luaL_error(L, "invalid kafka handle");
    return *rk;
}

static int l_producer_produce(lua_State *L)
{
    rd_kafka_t *rk = l_producer_retrieve(L);
    const char *topic = luaL_checkstring(L, 2);
    size_t length = 0;
    const char *payload = luaL_checklstring(L, 3, &length);
    return l_kafka_check(L, rd_kafka_producev(rk, RD_KAFKA_V_TOPIC(topic), RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY), RD_KAFKA_V_VALUE((void*)payload, length), RD_KAFKA_V_END));
}

static int l_producer_poll(lua_State *L)
{
    rd_kafka_t *rk = l_producer_retrieve(L);
    lua_pushinteger(L, rd_kafka_poll(rk, (int)lua_tointeger(L, 2)));
    return 1;
}

static int l_producer_flush(lua_State *L)
{
    rd_kafka_t *rk = l_producer_retrieve(L);
    return l_kafka_check(L, rd_kafka_flush(rk, (int)lua_tointeger(L, 2)));
}

static int l_producer_destroy(lua_State *L)
{
    rd_kafka_t **rk = luaL_checkudata(L, 1, "kafka.producer");
    if (*rk)
    {
        rd_kafka_destroy(*rk);
        *rk = 0;
    }
    return 0;
}

static const struct luaL_Reg funcs[] = {
        {"produce", l_producer_produce},
        {"poll", l_producer_poll},
        {"flush", l_producer_flush},
        {"close", l_producer_destroy},
        {NULL, NULL}
};

int l_producer_create(lua_State *L)
{
    // setup metatable
    if (luaL_newmetatable(L, "kafka.producer"))
    {
        lua_newtable(L);

        luaL_setfuncs(L, funcs, 0);
        lua_setfield(L, -2, "__index");
        lua_pushcfunction(L, l_producer_destroy);
        lua_setfield(L, -2, "__gc");

        lua_pop(L, 1);
    }

    luaL_checktype(L, 1, LUA_TTABLE);

    // parse configuration
    rd_kafka_conf_t *conf = rd_kafka_conf_new();
    char err[512];

    lua_pushnil(L);

    while (lua_next(L, -2) != 0)
    {
        if (!lua_isstring(L, -2))
        {
            rd_kafka_conf_destroy(conf);
            return l_kafka_error(L, "key must be a string");
        }

        if (!lua_isstring(L, -1))
        {
            rd_kafka_conf_destroy(conf);
            return l_kafka_error(L, "val must be a string");
        }

        const char *key = lua_tostring(L, -2);
        const char *val = lua_tostring(L, -1);

        if (rd_kafka_conf_set(conf, key, val, err, sizeof(err)) != RD_KAFKA_CONF_OK)
        {
            rd_kafka_conf_destroy(conf);
            return l_kafka_error(L, err);
        }

        lua_pop(L, 1);
    }

    // create kafka client
    rd_kafka_t *rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, err, sizeof(err));
    if (!rk)
    {
        rd_kafka_conf_destroy(conf);
        return l_kafka_error(L, err);
    }

    // create lua userdata
    *(rd_kafka_t**)lua_newuserdata(L, sizeof(rd_kafka_t*)) = rk;
    luaL_getmetatable(L, "kafka.producer");
    lua_setmetatable(L, -2);

    return 1;
}