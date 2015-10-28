#ifndef __CBSON_SYMBOL_H__
#define __CBSON_SYMBOL_H__

#include <lua.h>

#define SYMBOL_METATABLE "bson-symbol metatable"

typedef struct {
  char* symbol;
} cbson_symbol_t;

int cbson_symbol_create(lua_State* L, const char* symbol);
int cbson_symbol_new(lua_State* L);
cbson_symbol_t* check_cbson_symbol(lua_State *L, int index);

extern const struct luaL_Reg cbson_symbol_meta[];

extern const struct luaL_Reg cbson_symbol_methods[];

#endif
