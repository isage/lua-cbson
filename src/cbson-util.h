#ifndef __CBSON_UTIL_H__
#define __CBSON_UTIL_H__

#include <lua.h>
int luaL_checkudata_ex(lua_State *L, int ud, const char *tname);

#endif
