#ifndef __CBSON_REGEX_H__
#define __CBSON_REGEX_H__

#include <lua.h>

#define REGEX_METATABLE "bson-regex metatable"

typedef struct {
  char* regex;
  char* options;
} cbson_regex_t;

int cbson_regex_create(lua_State* L, const char* regex, const char* options);
int cbson_regex_new(lua_State* L);
cbson_regex_t* check_cbson_regex(lua_State *L, int index);

extern const struct luaL_Reg cbson_regex_meta[];

extern const struct luaL_Reg cbson_regex_methods[];

#endif
