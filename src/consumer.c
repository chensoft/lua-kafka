#include "kafka.h"

static rd_kafka_t* l_consumer_retrieve(lua_State *L)
{
    rd_kafka_t **rk = luaL_checkudata(L, 1, "kafka.consumer");
    if (!*rk)
        luaL_error(L, "invalid kafka handle");
    return *rk;
}

static int l_consumer_subscribe(lua_State *L)
{
    rd_kafka_t **rk = luaL_checkudata(L, 1, "kafka.consumer");
    int count = lua_gettop(L) - 1;
    if (!count)
        return l_kafka_error(L, "topic list empty");

    rd_kafka_topic_partition_list_t *list = rd_kafka_topic_partition_list_new(count);

    for (int i = 0; i < count; ++i)
    {
        const char *topic = lua_tostring(L, 2 + i);  // 1 is the consumer
        rd_kafka_topic_partition_list_add(list, topic, RD_KAFKA_PARTITION_UA);
    }

    int ret = l_kafka_check(L, rd_kafka_subscribe(*rk, list));
    rd_kafka_topic_partition_list_destroy(list);
    return ret;
}

static int l_consumer_poll(lua_State *L)
{
    rd_kafka_t **rk = luaL_checkudata(L, 1, "kafka.consumer");
    rd_kafka_message_t *rkm = rd_kafka_consumer_poll(*rk, (int)lua_tointeger(L, 2));

    if (!rkm)
        return l_kafka_error(L, "timeout");

    if (rkm->err)
    {
        l_kafka_error(L, rd_kafka_message_errstr(rkm));
        rd_kafka_message_destroy(rkm);
        return 2;
    }

    lua_newtable(L);

    lua_pushstring(L, rd_kafka_topic_name(rkm->rkt));
    lua_setfield(L, -2, "topic");

    lua_pushinteger(L, rkm->partition);
    lua_setfield(L, -2, "partition");

    if (rkm->payload)
    {
        lua_pushlstring(L, rkm->payload, rkm->len);
        lua_setfield(L, -2, "payload");
    }

    if (rkm->key)
    {
        lua_pushlstring(L, rkm->key, rkm->key_len);
        lua_setfield(L, -2, "key");
    }

    lua_pushinteger(L, rkm->offset);
    lua_setfield(L, -2, "offset");

    rd_kafka_message_destroy(rkm);

    return 1;
}

static int l_consumer_destroy(lua_State *L)
{
    rd_kafka_t **rk = luaL_checkudata(L, 1, "kafka.consumer");
    if (*rk)
    {
        rd_kafka_consumer_close(*rk);
        rd_kafka_destroy(*rk);
        *rk = 0;
    }
    return 0;
}

static const struct luaL_Reg funcs[] = {
        {"subscribe", l_consumer_subscribe},
        {"poll", l_consumer_poll},
        {"close", l_consumer_destroy},
        {NULL, NULL}
};

int l_consumer_create(lua_State *L)
{
    // setup metatable
    if (luaL_newmetatable(L, "kafka.consumer"))
    {
        lua_newtable(L);

        luaL_setfuncs(L, funcs, 0);
        lua_setfield(L, -2, "__index");
        lua_pushcfunction(L, l_consumer_destroy);
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
    rd_kafka_t *rk = rd_kafka_new(RD_KAFKA_CONSUMER, conf, err, sizeof(err));
    if (!rk)
    {
        rd_kafka_conf_destroy(conf);
        return l_kafka_error(L, err);
    }

    rd_kafka_poll_set_consumer(rk);

    // create lua userdata
    *(rd_kafka_t**)lua_newuserdata(L, sizeof(rd_kafka_t*)) = rk;
    luaL_getmetatable(L, "kafka.consumer");
    lua_setmetatable(L, -2);

    return 1;
}