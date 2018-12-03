#ifndef __CBSON_BINARY_H__
#define __CBSON_BINARY_H__

#include <lua.h>
#include <stdint.h>

#include "cbson.h"
#include "cbson-binary.h"

#define BINARY_METATABLE "bson-binary metatable"

typedef struct {
  int8_t type;
  unsigned int size;
  char* data;
} cbson_binary_t;

cbson_binary_t* cbson_binary_create(lua_State* L, uint8_t type, const char* binary, unsigned int size);
int cbson_binary_new(lua_State* L);
cbson_binary_t* check_cbson_binary(lua_State *L, int index);

extern const struct luaL_Reg cbson_binary_meta[];
extern const struct luaL_Reg cbson_binary_methods[];

#endif
