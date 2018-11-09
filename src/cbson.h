#ifndef __CBSON_H__
#define __CBSON_H__

#ifndef CBSON_MODNAME
#define CBSON_MODNAME   "cbson"
#endif

#ifndef CBSON_VERSION
#define CBSON_VERSION   "1.1"
#endif

#ifndef BSON_MAX_RECURSION
#define BSON_MAX_RECURSION 100
#endif


#define DEFINE_CHECK(name, type) \
cbson_##type##_t* check_cbson_##type (lua_State *L, int index) \
{ \
   if (lua_isuserdata(L, index) && luaL_checkudata(L, index, name##_METATABLE)) \
   { \
      return (cbson_##type##_t*)lua_touserdata(L, index); \
   } \
   else \
   { \
      lua_pushstring(L, "Invalid operand. Expected 'cbson." #type "'"); \
      lua_error(L); \
      return NULL; \
   } \
}


#endif