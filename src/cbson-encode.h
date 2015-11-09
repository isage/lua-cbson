#ifndef __CBSON_ENCODE_H__
#define __CBSON_ENCODE_H__

#include <lua.h>

int cbson_encode(lua_State *L);
int cbson_encode_first(lua_State *L);
int cbson_from_json(lua_State *L);

#endif
