#ifndef __CBSON_TIMESTAMP_H__
#define __CBSON_TIMESTAMP_H__

#include <lua.h>
#include <stdint.h>

#define TIMESTAMP_METATABLE "bson-timestamp metatable"

typedef struct {
  uint32_t timestamp;
  uint32_t increment;
} cbson_timestamp_t;

int cbson_timestamp_create(lua_State* L, uint32_t timestamp, uint32_t increment);
int cbson_timestamp_new(lua_State* L);
cbson_timestamp_t* check_cbson_timestamp(lua_State *L, int index);

extern const struct luaL_Reg cbson_timestamp_meta[];
extern const struct luaL_Reg cbson_timestamp_methods[];

#endif
