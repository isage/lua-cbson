#ifndef __CBSON_DATE_H__
#define __CBSON_DATE_H__

#include <lua.h>
#include <stdint.h>

#define DATE_METATABLE "bson-date metatable"

typedef int64_t cbson_date_t;

int64_t cbson_date_check(lua_State *L, int index);
int cbson_date_create(lua_State* L, int64_t val);
int cbson_date_new(lua_State* L);

extern const struct luaL_Reg cbson_date_meta[];
extern const struct luaL_Reg cbson_date_methods[];

#endif
