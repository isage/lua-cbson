#ifndef __CBSON_CODE_H__
#define __CBSON_CODE_H__

#include <lua.h>

#define CODE_METATABLE "bson-code metatable"
#define CODEWSCOPE_METATABLE "bson-codewscope metatable"

typedef struct {
  char* code;
} cbson_code_t;

typedef struct {
  char* code;
} cbson_codewscope_t;

int cbson_code_create(lua_State* L, const char* code);
int cbson_code_new(lua_State* L);
cbson_code_t* check_cbson_code(lua_State *L, int index);

extern const struct luaL_Reg cbson_code_meta[];
extern const struct luaL_Reg cbson_code_methods[];

// CODEWSCOPE

int cbson_codewscope_create(lua_State* L, const char* code);
int cbson_codewscope_new(lua_State* L);
cbson_codewscope_t* check_cbson_codewscope(lua_State *L, int index);

extern const struct luaL_Reg cbson_codewscope_meta[];
extern const struct luaL_Reg cbson_codewscope_methods[];

#endif
