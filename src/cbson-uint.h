#ifndef __CBSON_UINT_H__
#define __CBSON_UINT_H__

#include <lua.h>
#include <stdint.h>

#define UINT64_METATABLE "bson-uint64 metatable"

typedef uint64_t cbson_uint64_t;

uint64_t cbson_uint64_check(lua_State *L, int index);
int cbson_uint64_create(lua_State* L, uint64_t val);
int cbson_uint64_new(lua_State* L);
int cbson_uint64_to_raw(lua_State* L);
int cbson_uint64_from_raw(lua_State* L);

extern const struct luaL_Reg cbson_uint64_meta[];
extern const struct luaL_Reg cbson_uint64_methods[];

#endif
