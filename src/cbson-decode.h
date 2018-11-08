#ifndef __CBSON_DECODE_H__
#define __CBSON_DECODE_H__

#include <lua.h>

int cbson_decode(lua_State *L);
int cbson_to_json(lua_State *L);
int cbson_to_relaxed_json(lua_State *L);

#endif