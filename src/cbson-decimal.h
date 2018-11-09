#ifndef __CBSON_DECIMAL_H__
#define __CBSON_DECIMAL_H__

#include <lua.h>
#include <bson.h>

#define DECIMAL_METATABLE "bson-decimal metatable"

typedef struct {
  bson_decimal128_t dec;
} cbson_decimal_t;

int cbson_decimal_create(lua_State* L, const bson_decimal128_t* decimal);
int cbson_decimal_new(lua_State* L);
cbson_decimal_t* check_cbson_decimal(lua_State *L, int index);

extern const struct luaL_Reg cbson_decimal_meta[];
extern const struct luaL_Reg cbson_decimal_methods[];

#endif
