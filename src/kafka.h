#pragma once

#include <lauxlib.h>
#include "rdkafka.h"

// utilities
static int l_kafka_error(lua_State *L, const char *err)
{
    lua_pushnil(L);
    lua_pushstring(L, err);
    return 2;
}

static int l_kafka_check(lua_State *L, rd_kafka_resp_err_t err)
{
    if (err == RD_KAFKA_RESP_ERR_NO_ERROR)
        return 0;

    return l_kafka_error(L, rd_kafka_err2str(err));
}

// internal
int l_producer_create(lua_State *L);
int l_consumer_create(lua_State *L);