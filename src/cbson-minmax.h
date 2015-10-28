#ifndef __CBSON_MINMAX_H__
#define __CBSON_MINMAX_H__

#include <lua.h>

#define UNDEFINED_METATABLE "bson-undef metatable"
#define MINKEY_METATABLE "bson-minkey metatable"
#define MAXKEY_METATABLE "bson-maxkey metatable"

typedef struct {
  char unused;
} cbson_undefined_t;

typedef cbson_undefined_t cbson_minkey_t;
typedef cbson_undefined_t cbson_maxkey_t;

int cbson_undefined_create(lua_State* L);
int cbson_undefined_new(lua_State* L);
cbson_undefined_t* check_cbson_undefined(lua_State *L, int index);

extern const struct luaL_Reg cbson_undefined_meta[];
extern const struct luaL_Reg cbson_undefined_methods[];

int cbson_minkey_create(lua_State* L);
int cbson_minkey_new(lua_State* L);
cbson_minkey_t* check_cbson_minkey(lua_State *L, int index);

extern const struct luaL_Reg cbson_minkey_meta[];
extern const struct luaL_Reg cbson_minkey_methods[];

int cbson_maxkey_create(lua_State* L);
int cbson_maxkey_new(lua_State* L);
cbson_maxkey_t* check_cbson_maxkey(lua_State *L, int index);

extern const struct luaL_Reg cbson_maxkey_meta[];
extern const struct luaL_Reg cbson_maxkey_methods[];

#endif