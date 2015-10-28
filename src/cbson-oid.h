#ifndef __CBSON_OID_H__
#define __CBSON_OID_H__

#include <lua.h>

#define OID_METATABLE "bson-oid metatable"

typedef struct {
  char* oid;
} cbson_oid_t;

int cbson_oid_create(lua_State* L, const char* oid);
int cbson_oid_new(lua_State* L);
cbson_oid_t* check_cbson_oid(lua_State *L, int index);

extern const struct luaL_Reg cbson_oid_meta[];
extern const struct luaL_Reg cbson_oid_methods[];

#endif
