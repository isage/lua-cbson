#ifndef __CBSON_INT_H__
#define __CBSON_INT_H__

#include <lua.h>
#include <stdint.h>

#define INT64_METATABLE "bson-int64 metatable"

typedef int64_t cbson_int64_t;

int64_t cbson_int64_check(lua_State *L, int index);
int cbson_int64_create(lua_State* L, int64_t val);
int cbson_int64_new(lua_State* L);

extern const struct luaL_Reg cbson_int64_meta[];
extern const struct luaL_Reg cbson_int64_methods[];

#endif
