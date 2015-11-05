#include "cbson-util.h"

int luaL_checkudata_ex(lua_State *L, int ud, const char *tname)
{
  void *p = lua_touserdata(L, ud);
  if (p != NULL)
  {
    if (lua_getmetatable(L, ud))
    {
      lua_getfield(L, LUA_REGISTRYINDEX, tname);

      if (lua_rawequal(L, -1, -2))
      {
        lua_pop(L, 2);
        return 1;
      }
      lua_pop(L, 2);
      return 0;
    }
  }
  return 0;
}
