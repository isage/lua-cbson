#ifndef __CBSON_DECODE_H__
#define __CBSON_DECODE_H__

#include <lua.h>

int bson_decode(lua_State *L);
int bson_to_json(lua_State *L);

#endif