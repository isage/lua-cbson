package = "lua-cbson"
version = "git-1"
source = {
    url = "https://github.com/isage/lua-cbson/archive/master.zip",
    dir = "lua-cbson-master"
}
description = {
    summary = "Lua bindings for libbson",
    homepage = "https://github.com/isage/lua-cbson",
    maintainer = "isage.dna@gmail.com",
}
build = {
    type = "cmake",
    variables = {
        LUA_INCLUDE_DIR = "$(LUA_INCDIR)",
        LUA_LIBRARIES = "$(LUA_LIBDIR)",
        LUA_LIBRARY = "$(LUA_LIBDIR)/lua",
    }
}
