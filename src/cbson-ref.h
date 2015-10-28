#ifndef __CBSON_REF_H__
#define __CBSON_REF_H__

#include <lua.h>

#define REF_METATABLE "bson-ref metatable"

typedef struct {
  char* ref;
  char* id;
} cbson_ref_t;

int cbson_ref_create(lua_State* L, const char* ref, const char* id);
int cbson_ref_new(lua_State* L);
cbson_ref_t* check_cbson_ref(lua_State *L, int index);

extern const struct luaL_Reg cbson_ref_meta[];
extern const struct luaL_Reg cbson_ref_methods[];

#endif
