#include <lua.h>
#include <lauxlib.h>
#include <bson.h>
#include <string.h>
#include <resolv.h>
#include <stdint.h>
#include <inttypes.h>

#ifndef CBSON_MODNAME
#define CBSON_MODNAME   "cbson"
#endif

#ifndef CBSON_VERSION
#define CBSON_VERSION   "1.0"
#endif

#ifndef BSON_MAX_RECURSION
#define BSON_MAX_RECURSION 100
#endif

#define INT64_METATABLE "bson-int64 metatable"
#define OID_METATABLE "bson-oid metatable"
#define REGEX_METATABLE "bson-regex metatable"
#define DATE_METATABLE "bson-date metatable"
#define TIMESTAMP_METATABLE "bson-timestamp metatable"
#define BINARY_METATABLE "bson-binary metatable"
#define REF_METATABLE "bson-ref metatable"
#define UNDEFINED_METATABLE "bson-undef metatable"
#define MINKEY_METATABLE "bson-minkey metatable"
#define MAXKEY_METATABLE "bson-maxkey metatable"
#define SYMBOL_METATABLE "bson-symbol metatable"
#define CODE_METATABLE "bson-code metatable"
#define CODEWSCOPE_METATABLE "bson-codewscope metatable"

typedef struct {
  char* regex;
  char* options;
} cbson_regex_t;

typedef struct {
  char* oid;
} cbson_oid_t;

typedef struct {
  char* code;
} cbson_code_t;

typedef struct {
  char* code;
} cbson_codewscope_t;

typedef struct {
  char* symbol;
} cbson_symbol_t;

typedef struct {
  uint32_t timestamp;
  uint32_t increment;
} cbson_timestamp_t;

typedef struct {
  int8_t type;
  char* data;
} cbson_binary_t;

typedef struct {
  char* ref;
  char* id;
} cbson_ref_t;

typedef struct {
  uint8_t unused;
} cbson_undefined_t;

typedef int64_t cbson_int64_t;
typedef int64_t cbson_date_t;

typedef cbson_undefined_t cbson_minkey_t;
typedef cbson_undefined_t cbson_maxkey_t;

#define DEFINE_CHECK(name, type) \
static cbson_##type##_t* check_cbson_##type (lua_State *L, int index) \
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

// REGEX

DEFINE_CHECK(REGEX, regex)

static int cbson_regex_create(lua_State* L, const char* regex, const char* options)
{
  cbson_regex_t* ud = lua_newuserdata(L, sizeof(cbson_regex_t));

  ud->regex = malloc(strlen(regex)+1);
  ud->options = malloc(strlen(options)+1);
  strcpy(ud->regex, regex);
  strcpy(ud->options, options);

  luaL_getmetatable(L, REGEX_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

static int cbson_regex_new(lua_State* L)
{
  const char* regex = luaL_checkstring(L, 1);
  const char* options = luaL_checkstring(L, 2);

  return cbson_regex_create(L,regex, options);
}

static int cbson_regex_destroy(lua_State* L)
{
  cbson_regex_t* a = check_cbson_regex(L, 1);

  free(a->regex);
  free(a->options);

  return 0;
}

static int cbson_regex_tostring(lua_State* L)
{
  cbson_regex_t* a = check_cbson_regex(L, 1);

  lua_pushfstring(L, "/%s/%s", a->regex, a->options);
  return 1;
}

static int cbson_regex_regex(lua_State* L)
{
  cbson_regex_t* a = check_cbson_regex(L, 1);
  const char* regex = luaL_optstring(L, 2, NULL);

  if (regex)
  {
    free(a->regex);
    a->regex = malloc(strlen(regex)+1);
    strcpy(a->regex, regex);
  }

  lua_pushstring(L, a->regex);
  return 1;
}

static int cbson_regex_options(lua_State* L)
{
  cbson_regex_t* a = check_cbson_regex(L, 1);
  const char* opt = luaL_optstring(L, 2, NULL);

  if (opt)
  {
    free(a->options);
    a->options = malloc(strlen(opt)+1);
    strcpy(a->options, opt);
  }

  lua_pushstring(L, a->options);
  return 1;
}


static const struct luaL_Reg cbson_regex_meta[] = {
  {"__tostring", cbson_regex_tostring},
  {"__gc",       cbson_regex_destroy},
  {NULL, NULL}
};

static const struct luaL_Reg cbson_regex_methods[] = {
  {"regex",   cbson_regex_regex},
  {"options", cbson_regex_options},
  {NULL, NULL}
};

// OID

DEFINE_CHECK(OID, oid)

static int cbson_oid_create(lua_State* L, const char* oid)
{
  if (strlen(oid) != 24)
  {
    lua_pushstring(L, "Invalid operand. OID should be hex-string 24-chars long");
    lua_error(L);
    return 1;
  }

  cbson_oid_t* ud = lua_newuserdata(L, sizeof(cbson_oid_t));

  ud->oid = malloc(strlen(oid)+1);
  strcpy(ud->oid, oid);

  luaL_getmetatable(L, OID_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

static int cbson_oid_new(lua_State* L)
{
  const char* oid = luaL_checkstring(L, 1);

  return cbson_oid_create(L, oid);
}

static int cbson_oid_destroy(lua_State* L)
{
  cbson_oid_t* a = check_cbson_oid(L, 1);

  free(a->oid);

  return 0;
}

static int cbson_oid_tostring(lua_State* L)
{
  cbson_oid_t* a = check_cbson_oid(L, 1);

  lua_pushstring(L, a->oid);
  return 1;
}


static const struct luaL_Reg cbson_oid_meta[] = {
  {"__tostring", cbson_oid_tostring},
  {"__gc",       cbson_oid_destroy},
  {NULL, NULL}
};

static const struct luaL_Reg cbson_oid_methods[] = {
  {NULL, NULL}
};


// BINARY

DEFINE_CHECK(BINARY, binary)

static int cbson_binary_create(lua_State* L, uint8_t type, const char* binary)
{
  cbson_binary_t* ud = lua_newuserdata(L, sizeof(cbson_binary_t));

  ud->data = malloc(strlen(binary)+1);
  ud->type = type;
  strcpy(ud->data, binary);

  luaL_getmetatable(L, BINARY_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

static int cbson_binary_new(lua_State* L)
{
  uint8_t type = luaL_checknumber(L, 1);
  const char* binary = luaL_checkstring(L, 2);

  return cbson_binary_create(L, type, binary);
}

static int cbson_binary_destroy(lua_State* L)
{
  cbson_binary_t* a = check_cbson_binary(L, 1);

  free(a->data);

  return 0;
}

static int cbson_binary_tostring(lua_State* L)
{
  cbson_binary_t* a = check_cbson_binary(L, 1);

  lua_pushfstring(L, "%d:%s", a->type, a->data);
  return 1;
}

static int cbson_binary_data(lua_State* L)
{
  cbson_binary_t* a = check_cbson_binary(L, 1);
  const char* binary = luaL_optstring(L, 2, NULL);

  if (binary)
  {
    free(a->data);
    a->data = malloc(strlen(binary)+1);
    strcpy(a->data, binary);
  }

  lua_pushstring(L, a->data);
  return 1;
}

static int cbson_binary_raw(lua_State* L)
{
  cbson_binary_t* a = check_cbson_binary(L, 1);
  const char* binary = luaL_optstring(L, 2, NULL);
  int binary_len = luaL_optnumber(L, 3, -1);

  if (binary && binary_len >=0)
  {
    free(a->data);

    size_t b64_len;
    char *b64;

    b64_len = (binary_len / 3 + 1) * 4 + 1;
    b64 = malloc(b64_len);
    b64_ntop((unsigned const char*)binary, binary_len, b64, b64_len);

    a->data = malloc(strlen(b64)+1);
    strcpy(a->data, b64);
    free(b64);
  }

  binary_len = b64_pton (a->data, NULL, 0);
  unsigned char* buf=malloc(binary_len+1);
  b64_pton(a->data, buf, binary_len+1);

  lua_pushlstring(L, (char*)buf, binary_len);

  free(buf);
  return 1;
}

static int cbson_binary_type(lua_State* L)
{
  cbson_binary_t* a = check_cbson_binary(L, 1);
  int8_t type = luaL_optint(L, 2, -1);

  if (type>=0)
  {
    a->type = type;
  }

  lua_pushnumber(L, a->type);
  return 1;
}


static const struct luaL_Reg cbson_binary_meta[] = {
  {"__tostring", cbson_binary_tostring},
  {"__gc",       cbson_binary_destroy},
  {NULL, NULL}
};

static const struct luaL_Reg cbson_binary_methods[] = {
  {"data",   cbson_binary_data},
  {"type",   cbson_binary_type},
  {"raw",    cbson_binary_raw},
  {NULL, NULL}
};

// SYMBOL

DEFINE_CHECK(SYMBOL, symbol)

static int cbson_symbol_create(lua_State* L, const char* symbol)
{
  cbson_symbol_t* ud = lua_newuserdata(L, sizeof(cbson_symbol_t));

  ud->symbol = malloc(strlen(symbol)+1);
  strcpy(ud->symbol, symbol);

  luaL_getmetatable(L, SYMBOL_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

static int cbson_symbol_new(lua_State* L)
{
  const char* symbol = luaL_checkstring(L, 1);

  return cbson_symbol_create(L, symbol);
}

static int cbson_symbol_destroy(lua_State* L)
{
  cbson_symbol_t* a = check_cbson_symbol(L, 1);

  free(a->symbol);

  return 0;
}

static int cbson_symbol_tostring(lua_State* L)
{
  cbson_symbol_t* a = check_cbson_symbol(L, 1);

  lua_pushstring(L, a->symbol);
  return 1;
}

static const struct luaL_Reg cbson_symbol_meta[] = {
  {"__tostring", cbson_symbol_tostring},
  {"__gc",       cbson_symbol_destroy},
  {NULL, NULL}
};

static const struct luaL_Reg cbson_symbol_methods[] = {
  {NULL, NULL}
};

// CODE

DEFINE_CHECK(CODE, code)

static int cbson_code_create(lua_State* L, const char* code)
{
  cbson_code_t* ud = lua_newuserdata(L, sizeof(cbson_code_t));

  ud->code = malloc(strlen(code)+1);
  strcpy(ud->code, code);

  luaL_getmetatable(L, CODE_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

static int cbson_code_new(lua_State* L)
{
  const char* code = luaL_checkstring(L, 1);

  return cbson_code_create(L, code);
}

static int cbson_code_destroy(lua_State* L)
{
  cbson_code_t* a = check_cbson_code(L, 1);

  free(a->code);

  return 0;
}

static int cbson_code_tostring(lua_State* L)
{
  cbson_code_t* a = check_cbson_code(L, 1);

  lua_pushstring(L, a->code);
  return 1;
}

static const struct luaL_Reg cbson_code_meta[] = {
  {"__tostring", cbson_code_tostring},
  {"__gc",       cbson_code_destroy},
  {NULL, NULL}
};

static const struct luaL_Reg cbson_code_methods[] = {
  {NULL, NULL}
};

// CODEWSCOPE

DEFINE_CHECK(CODEWSCOPE, codewscope)

static int cbson_codewscope_create(lua_State* L, const char* code)
{
  cbson_codewscope_t* ud = lua_newuserdata(L, sizeof(cbson_codewscope_t));

  ud->code = malloc(strlen(code)+1);
  strcpy(ud->code, code);

  luaL_getmetatable(L, CODEWSCOPE_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

static int cbson_codewscope_new(lua_State* L)
{
  const char* code = luaL_checkstring(L, 1);

  return cbson_codewscope_create(L, code);
}

static int cbson_codewscope_destroy(lua_State* L)
{
  cbson_codewscope_t* a = check_cbson_codewscope(L, 1);

  free(a->code);

  return 0;
}

static int cbson_codewscope_tostring(lua_State* L)
{
  cbson_codewscope_t* a = check_cbson_codewscope(L, 1);

  lua_pushstring(L, a->code);
  return 1;
}

static const struct luaL_Reg cbson_codewscope_meta[] = {
  {"__tostring", cbson_codewscope_tostring},
  {"__gc",       cbson_codewscope_destroy},
  {NULL, NULL}
};

static const struct luaL_Reg cbson_codewscope_methods[] = {
  {NULL, NULL}
};

// UNDEFINED

DEFINE_CHECK(UNDEFINED, undefined)

static int cbson_undefined_create(lua_State* L)
{
  lua_newuserdata(L, sizeof(cbson_undefined_t));

  luaL_getmetatable(L, UNDEFINED_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

static int cbson_undefined_new(lua_State* L)
{
  return cbson_undefined_create(L);
}

static int cbson_undefined_tostring(lua_State* L)
{
  check_cbson_undefined(L, 1);

  lua_pushstring(L, "undefined");
  return 1;
}

static const struct luaL_Reg cbson_undefined_meta[] = {
  {"__tostring", cbson_undefined_tostring},
  {NULL, NULL}
};

static const struct luaL_Reg cbson_undefined_methods[] = {
  {NULL, NULL}
};

// MINKEY

DEFINE_CHECK(MINKEY, minkey)

static int cbson_minkey_create(lua_State* L)
{
  lua_newuserdata(L, sizeof(cbson_minkey_t));

  luaL_getmetatable(L, MINKEY_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

static int cbson_minkey_new(lua_State* L)
{
  return cbson_minkey_create(L);
}

static int cbson_minkey_tostring(lua_State* L)
{
  check_cbson_minkey(L, 1);

  lua_pushstring(L, "minkey");
  return 1;
}

static const struct luaL_Reg cbson_minkey_meta[] = {
  {"__tostring", cbson_minkey_tostring},
  {NULL, NULL}
};

static const struct luaL_Reg cbson_minkey_methods[] = {
  {NULL, NULL}
};

// MAXKEY


DEFINE_CHECK(MAXKEY, maxkey)

static int cbson_maxkey_create(lua_State* L)
{
  lua_newuserdata(L, sizeof(cbson_maxkey_t));

  luaL_getmetatable(L, MAXKEY_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

static int cbson_maxkey_new(lua_State* L)
{
  return cbson_maxkey_create(L);
}

static int cbson_maxkey_tostring(lua_State* L)
{
  check_cbson_maxkey(L, 1);

  lua_pushstring(L, "maxkey");
  return 1;
}

static const struct luaL_Reg cbson_maxkey_meta[] = {
  {"__tostring", cbson_maxkey_tostring},
  {NULL, NULL}
};

static const struct luaL_Reg cbson_maxkey_methods[] = {
  {NULL, NULL}
};


// REF

DEFINE_CHECK(REF, ref)

static int cbson_ref_create(lua_State* L, const char* ref, const char* id)
{
  cbson_ref_t* ud = lua_newuserdata(L, sizeof(cbson_ref_t));

  ud->ref = malloc(strlen(ref)+1);
  ud->id = malloc(strlen(id)+1);
  strcpy(ud->ref, ref);
  strcpy(ud->id, id);

  luaL_getmetatable(L, REF_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

static int cbson_ref_new(lua_State* L)
{
  const char* ref = luaL_checkstring(L, 1);
  const char* id = luaL_checkstring(L, 2);

  return cbson_ref_create(L,ref, id);
}

static int cbson_ref_destroy(lua_State* L)
{
  cbson_ref_t* a = check_cbson_ref(L, 1);

  free(a->ref);
  free(a->id);

  return 0;
}

static int cbson_ref_tostring(lua_State* L)
{
  cbson_ref_t* a = check_cbson_ref(L, 1);

  lua_pushfstring(L, "%s/%s", a->ref, a->id);
  return 1;
}

static int cbson_ref_ref(lua_State* L)
{
  cbson_ref_t* a = check_cbson_ref(L, 1);
  const char* ref = luaL_optstring(L, 2, NULL);

  if (ref)
  {
    free(a->ref);
    a->ref = malloc(strlen(ref)+1);
    strcpy(a->ref, ref);
  }

  lua_pushstring(L, a->ref);
  return 1;
}

static int cbson_ref_id(lua_State* L)
{
  cbson_ref_t* a = check_cbson_ref(L, 1);
  const char* id = luaL_optstring(L, 2, NULL);

  if (id)
  {
    free(a->id);
    a->id = malloc(strlen(id)+1);
    strcpy(a->id, id);
  }

  lua_pushstring(L, a->id);
  return 1;
}


static const struct luaL_Reg cbson_ref_meta[] = {
  {"__tostring", cbson_ref_tostring},
  {"__gc",       cbson_ref_destroy},
  {NULL, NULL}
};

static const struct luaL_Reg cbson_ref_methods[] = {
  {"ref",   cbson_ref_ref},
  {"id", cbson_ref_id},
  {NULL, NULL}
};

// TIMESTAMP

DEFINE_CHECK(TIMESTAMP, timestamp)

static int cbson_timestamp_create(lua_State* L, uint32_t timestamp, uint32_t increment)
{
  cbson_timestamp_t* ud = lua_newuserdata(L, sizeof(cbson_timestamp_t));

  ud->timestamp = timestamp;
  ud->increment = increment;

  luaL_getmetatable(L, TIMESTAMP_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

static int cbson_timestamp_new(lua_State* L)
{
  uint32_t timestamp = luaL_checknumber(L, 1);
  uint32_t increment = luaL_checknumber(L, 2);

  return cbson_timestamp_create(L, timestamp, increment);
}

static int cbson_timestamp_tostring(lua_State* L)
{
  cbson_timestamp_t* a = check_cbson_timestamp(L, 1);

  char buffer[(sizeof(int32_t)*8+1)*2];
  sprintf(buffer, "%"PRId32" %"PRId32, a->timestamp, a->increment);

  lua_pushstring(L, buffer);
  return 1;
}

static int cbson_timestamp_timestamp(lua_State* L)
{
  cbson_timestamp_t* a = check_cbson_timestamp(L, 1);
  uint32_t timestamp = luaL_optint(L, 2, 0);

  if (lua_gettop(L) == 2)
  {
    a->timestamp = timestamp;
  }

  lua_pushnumber(L, a->timestamp);
  return 1;
}

static int cbson_timestamp_increment(lua_State* L)
{
  cbson_timestamp_t* a = check_cbson_timestamp(L, 1);
  uint32_t increment = luaL_optint(L, 2, 0);

  if (lua_gettop(L) == 2)
  {
    a->increment = increment;
  }

  lua_pushnumber(L, a->increment);
  return 1;
}


static const struct luaL_Reg cbson_timestamp_meta[] = {
  {"__tostring", cbson_timestamp_tostring},
  {NULL, NULL}
};

static const struct luaL_Reg cbson_timestamp_methods[] = {
  {"timestamp", cbson_timestamp_timestamp},
  {"increment", cbson_timestamp_increment},
  {NULL, NULL}
};

// INT64

//DEFINE_CHECK(INT64, int64)

static int64_t cbson_int64_check (lua_State *L, int index)
{
  if (lua_isuserdata(L, index) && luaL_checkudata(L, index, INT64_METATABLE))
  {
    return *(int64_t*)lua_touserdata(L, index);
  }
  else if (lua_isnumber(L, index))
  {
    return lua_tonumber(L, index);
  }
  else if (lua_isstring(L, index))
  {
    return strtoll(lua_tostring(L, index), NULL, 10);
  }
  else
  {
    lua_pushstring(L, "Invalid operand. Expected 'cbson.int', 'string' or 'number'");
    lua_error(L);
    return 0;
  }
}

static int cbson_int64_create(lua_State* L, int64_t val)
{
  cbson_int64_t* ud = lua_newuserdata(L, sizeof(cbson_int64_t));

  *ud = val;

  luaL_getmetatable(L, INT64_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

static int cbson_int64_new(lua_State* L)
{
  return cbson_int64_create(L, cbson_int64_check(L,1));
}

static int cbson_int64_tostring(lua_State* L)
{
  cbson_int64_t a = cbson_int64_check(L, 1);

  char buffer[sizeof(int64_t)*8+1];
  sprintf(buffer, "%"PRId64, a);

  lua_pushstring(L, buffer);
  return 1;
}

static int64_t cbson_pow(int64_t ap, int32_t n)
{
  int64_t pow;

  if(n != 0)
  {
    if (n<0)
    {
      if ((ap!=1)&&(ap!=-1))
      {
        return(0);
      }

      n = -n;
    }

    pow=1;

    for( ; ; )
    {
      if(n & 01)
      {
        pow = pow * ap;
      }

      if(n >>= 1)
      {
        ap = ap * ap;
      }
      else
      {
        return(pow);
      }
    }
  }
  else return(1);
}

static int cbson_int64_add (lua_State* L) { return cbson_int64_create( L, cbson_int64_check(L,1) + cbson_int64_check(L,2) ); }
static int cbson_int64_sub (lua_State* L) { return cbson_int64_create( L, cbson_int64_check(L,1) - cbson_int64_check(L,2) ); }
static int cbson_int64_mul (lua_State* L) { return cbson_int64_create( L, cbson_int64_check(L,1) * cbson_int64_check(L,2) ); }
static int cbson_int64_div (lua_State* L) { return cbson_int64_create( L, cbson_int64_check(L,1) / cbson_int64_check(L,2) ); }
static int cbson_int64_mod (lua_State* L) { return cbson_int64_create( L, cbson_int64_check(L,1) % cbson_int64_check(L,2) ); }
static int cbson_int64_pow (lua_State* L) { return cbson_int64_create( L, cbson_pow( cbson_int64_check(L,1), cbson_int64_check(L,2) ) ); }
static int cbson_int64_unm (lua_State* L) { return cbson_int64_create( L, -cbson_int64_check(L,1) ); }
static int cbson_int64_eq  (lua_State* L) { lua_pushboolean( L, cbson_int64_check(L,1) == cbson_int64_check(L,2) ); return 1; }
static int cbson_int64_lt  (lua_State* L) { lua_pushboolean( L, cbson_int64_check(L,1) <  cbson_int64_check(L,2) ); return 1; }
static int cbson_int64_le  (lua_State* L) { lua_pushboolean( L, cbson_int64_check(L,1) <= cbson_int64_check(L,2) ); return 1; }

static const struct luaL_Reg cbson_int64_meta[] = {
  { "__add",     cbson_int64_add },
  { "__sub",     cbson_int64_sub },
  { "__mul",     cbson_int64_mul },
  { "__div",     cbson_int64_div },
  { "__mod",     cbson_int64_mod },
  { "__pow",     cbson_int64_pow },
  { "__unm",     cbson_int64_unm },
  { "__eq",      cbson_int64_eq  },
  { "__lt",      cbson_int64_lt  },
  { "__le",      cbson_int64_le  },
  {"__tostring", cbson_int64_tostring},
  {NULL, NULL}
};

static const struct luaL_Reg cbson_int64_methods[] = {
  {NULL, NULL}
};


// DATE

static int64_t cbson_date_check (lua_State *L, int index)
{
  if (lua_isuserdata(L, index) && luaL_checkudata(L, index, DATE_METATABLE))
  {
    return *(int64_t*)lua_touserdata(L, index);
  }
  else if (lua_isnumber(L, index))
  {
    return lua_tonumber(L, index);
  }
  else if (lua_isstring(L, index))
  {
    return strtoll(lua_tostring(L, index), NULL, 10);
  }
  else
  {
    lua_pushstring(L, "Invalid operand. Expected 'cbson.date', 'string' or 'number'");
    lua_error(L);
    return 0;
  }
}

static int cbson_date_create(lua_State* L, int64_t val)
{
  cbson_date_t* ud = lua_newuserdata(L, sizeof(cbson_date_t));

  *ud = val;

  luaL_getmetatable(L, DATE_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

static int cbson_date_new(lua_State* L)
{
  return cbson_date_create(L, cbson_date_check(L,1));
}

static int cbson_date_tostring(lua_State* L)
{
  cbson_date_t a = cbson_date_check(L, 1);

  char buffer[sizeof(int64_t)*8+1];
  sprintf(buffer, "%"PRId64, a);

  lua_pushstring(L, buffer);
  return 1;
}

static int cbson_date_add (lua_State* L) { return cbson_date_create( L, cbson_date_check(L,1) + cbson_date_check(L,2) ); }
static int cbson_date_sub (lua_State* L) { return cbson_date_create( L, cbson_date_check(L,1) - cbson_date_check(L,2) ); }
static int cbson_date_mul (lua_State* L) { return cbson_date_create( L, cbson_date_check(L,1) * cbson_date_check(L,2) ); }
static int cbson_date_div (lua_State* L) { return cbson_date_create( L, cbson_date_check(L,1) / cbson_date_check(L,2) ); }
static int cbson_date_mod (lua_State* L) { return cbson_date_create( L, cbson_date_check(L,1) % cbson_date_check(L,2) ); }
static int cbson_date_pow (lua_State* L) { return cbson_date_create( L, cbson_pow( cbson_date_check(L,1), cbson_date_check(L,2) ) ); }
static int cbson_date_unm (lua_State* L) { return cbson_date_create( L, -cbson_date_check(L,1) ); }
static int cbson_date_eq  (lua_State* L) { lua_pushboolean( L, cbson_date_check(L,1) == cbson_date_check(L,2) ); return 1; }
static int cbson_date_lt  (lua_State* L) { lua_pushboolean( L, cbson_date_check(L,1) <  cbson_date_check(L,2) ); return 1; }
static int cbson_date_le  (lua_State* L) { lua_pushboolean( L, cbson_date_check(L,1) <= cbson_date_check(L,2) ); return 1; }

static const struct luaL_Reg cbson_date_meta[] = {
  { "__add",     cbson_date_add },
  { "__sub",     cbson_date_sub },
  { "__mul",     cbson_date_mul },
  { "__div",     cbson_date_div },
  { "__mod",     cbson_date_mod },
  { "__pow",     cbson_date_pow },
  { "__unm",     cbson_date_unm },
  { "__eq",      cbson_date_eq  },
  { "__lt",      cbson_date_lt  },
  { "__le",      cbson_date_le  },
  {"__tostring", cbson_date_tostring},
  {NULL, NULL}
};

static const struct luaL_Reg cbson_date_methods[] = {
  {NULL, NULL}
};



// decoding

typedef struct {
  uint32_t count;
  bool keys;
  uint32_t depth;
  lua_State *L;
} cbson_state_t;

static bool cbson_visit_document(const bson_iter_t *iter, const char *key, const bson_t *v_document, void *data);
static bool cbson_visit_array(const bson_iter_t *iter, const char *key, const bson_t *v_array, void *data);

static bool cbson_visit_before(const bson_iter_t *iter, const char *key, void *data)
{
  cbson_state_t *s = data;

  s->count++;

  if (s->keys)
  {
    lua_pushstring(s->L, key);
  }

  return false;
}

static bool cbson_visit_after(const bson_iter_t *iter, const char *key, void *data)
{
  cbson_state_t *s = data;

  if (s->keys)
  {
    lua_rawset(s->L, -3);
  }
  else
  {
    lua_rawseti(s->L, -2, s->count);
  }

  return false;
}

static bool cbson_visit_utf8(const bson_iter_t *iter, const char *key, size_t v_utf8_len, const char *v_utf8, void *data)
{
  cbson_state_t *s = data;

  lua_pushlstring(s->L, v_utf8, v_utf8_len);

  return false;
}

static bool cbson_visit_int32(const bson_iter_t *iter, const char *key, int32_t v_int32, void *data)
{
  cbson_state_t *s = data;

  cbson_int64_create(s->L, v_int32);

  return false;
}

static bool cbson_visit_int64(const bson_iter_t *iter, const char *key, int64_t v_int64, void *data)
{
  cbson_state_t *s = data;
  
  cbson_int64_create(s->L, v_int64);

  return false;
}

static bool cbson_visit_double(const bson_iter_t *iter, const char *key, double v_double, void *data)
{
  cbson_state_t *s = data;

  lua_pushnumber(s->L, v_double);

  return false;
}

static bool cbson_visit_undefined(const bson_iter_t *iter, const char *key, void *data)
{
  cbson_state_t *s = data;

  cbson_undefined_create(s->L);

  return false;
}

static bool cbson_visit_null(const bson_iter_t *iter, const char *key, void *data)
{
  cbson_state_t *s = data;

  lua_pushnil(s->L);

  return false;
}

static bool cbson_visit_bool(const bson_iter_t *iter, const char *key, bool v_bool, void *data)
{
  cbson_state_t *s = data;

  lua_pushboolean(s->L, v_bool);

  return false;
}

static bool cbson_visit_oid(const bson_iter_t *iter, const char *key, const bson_oid_t *oid, void *data)
{
  cbson_state_t *s = data;
  char str[25];

  bson_oid_to_string (oid, str);

  cbson_oid_create(s->L, str);

  return false;
}

static bool cbson_visit_regex(const bson_iter_t *iter, const char *key, const  char *v_reg, const char *v_options, void *data)
{
  cbson_state_t *s = data;

  cbson_regex_create(s->L, v_reg, v_options);

  return false;
}

static bool cbson_visit_date_time(const bson_iter_t *iter, const char *key, int64_t msec_since_epoch, void *data)
{
  cbson_state_t *s = data;

  cbson_date_create(s->L, msec_since_epoch);

  return false;
}

static bool cbson_visit_minkey(const bson_iter_t *iter, const char *key, void *data)
{
  cbson_state_t *s = data;

  cbson_minkey_create(s->L);

  return false;
}

static bool cbson_visit_maxkey(const bson_iter_t *iter, const char *key, void *data)
{
  cbson_state_t *s = data;

  cbson_maxkey_create(s->L);

  return false;
}

static bool cbson_visit_code(const bson_iter_t *iter, const char *key, size_t v_code_len, const char *v_code, void *data)
{
  cbson_state_t *s = data;

  cbson_code_create(s->L, v_code);

  return false;
}

static bool cbson_visit_symbol(const bson_iter_t *iter, const char *key, size_t v_symbol_len, const char *v_symbol, void *data)
{
  cbson_state_t *s = data;

  cbson_symbol_create(s->L, v_symbol);

  return false;
}

static bool cbson_visit_codewscope(const bson_iter_t *iter, const char *key, size_t v_code_len, const char *v_code, const bson_t *v_scope, void *data)
{
  cbson_state_t *s = data;

  cbson_codewscope_create(s->L, v_code);

  return false;
}

static bool cbson_visit_dbpointer(const bson_iter_t *iter, const char *key, size_t v_collection_len, const char *v_collection, const bson_oid_t *v_oid, void *data)
{
  cbson_state_t *s = data;
  char str[25]="";

  if (v_oid)
  {
    bson_oid_to_string (v_oid, str);
  }

  cbson_ref_create(s->L, v_collection, str);

  return false;
}

static bool cbson_visit_binary(const bson_iter_t *iter, const char *key, bson_subtype_t v_subtype, size_t v_binary_len, const uint8_t *v_binary, void *data)
{
  cbson_state_t *s = data;
  size_t b64_len;
  char *b64;

  b64_len = (v_binary_len / 3 + 1) * 4 + 1;
  b64 = malloc(b64_len);
  b64_ntop(v_binary, v_binary_len, b64, b64_len);

  cbson_binary_create(s->L, v_subtype, b64);

  free(b64);

  return false;
}

static bool cbson_visit_timestamp(const bson_iter_t *iter, const char *key, uint32_t v_timestamp, uint32_t v_increment, void *data)
{
  cbson_state_t *s = data;

  cbson_timestamp_create(s->L, v_timestamp, v_increment);

  return false;
}

static const bson_visitor_t cbson_visitors = {
  cbson_visit_before,
  cbson_visit_after,
  NULL, /* visit_corrupt */
  cbson_visit_double,
  cbson_visit_utf8,
  cbson_visit_document,
  cbson_visit_array,
  cbson_visit_binary,
  cbson_visit_undefined,
  cbson_visit_oid,
  cbson_visit_bool,
  cbson_visit_date_time,
  cbson_visit_null,
  cbson_visit_regex,
  cbson_visit_dbpointer,
  cbson_visit_code,
  cbson_visit_symbol,
  cbson_visit_codewscope,
  cbson_visit_int32,
  cbson_visit_timestamp,
  cbson_visit_int64,
  cbson_visit_maxkey,
  cbson_visit_minkey
};

static bool cbson_visit_document(const bson_iter_t *iter, const char *key, const bson_t *v_document, void *data)
{
  bson_iter_t iter_new;
  cbson_state_t *s = data;
  cbson_state_t cs = {0, true};

  if (s->depth >= BSON_MAX_RECURSION)
  {
    lua_pushlstring(s->L, "...", 3);
    return false;
  }

  if (bson_iter_init(&iter_new, v_document))
  {
    lua_newtable(s->L);
    cs.depth = s->depth + 1;
    cs.L = s->L;
    bson_iter_visit_all(&iter_new, &cbson_visitors, &cs);
  }
  else
  {
    luaL_error(s->L, "Can't init bson iterator.");
  }

  return false;
}

static bool cbson_visit_array(const bson_iter_t *iter, const char *key, const bson_t *v_array, void *data)
{
  bson_iter_t iter_new;
  cbson_state_t *s = data;
  cbson_state_t cs = {0, false};

  if (s->depth >= BSON_MAX_RECURSION)
  {
    lua_pushlstring(s->L, "...", 3);
    return false;
  }

  if (bson_iter_init(&iter_new, v_array))
  {
    lua_newtable(s->L);
    cs.depth = s->depth + 1;
    cs.L = s->L;
    bson_iter_visit_all(&iter_new, &cbson_visitors, &cs);
  }
  else
  {
    luaL_error(s->L, "Can't init bson iterator.");
  }

  return false;
}


static int bson_decode(lua_State *L)
{
  bson_t *bson;
  cbson_state_t s = {0, true, 0};
  bson_iter_t iter;
  size_t len;

  const uint8_t* data = (uint8_t*)luaL_checklstring(L, 1, &len);

  bson = bson_new_from_data(data, len);

  if (bson)
  {
    lua_pushnil(L);
    if (bson_iter_init(&iter, bson))
    {
      lua_newtable(L);
      s.L = L;
      bson_iter_visit_all(&iter, &cbson_visitors, &s);
    }
    else
    {
      luaL_error(L, "Can't init bson iterator.");
    }
  }
  else
  {
    luaL_error(L, "Can't init bson from data.");
  }

  bson_destroy(bson);
  return 1;
}


// encoding
static int is_array(lua_State *L, int index)
{
  double k;

  lua_pushvalue(L, index);
  lua_pushnil(L);

  while (lua_next(L, -2))
  {
     if (lua_type(L, -2) == LUA_TNUMBER && (k = lua_tonumber(L, -2)))
     {
       if ((float)(int)(float)(k) == (float)k && k >= 1) // probably int
       {
         lua_pop(L, 1); // pop value
         continue;
       }
     }

     lua_pop(L, 3); // pop value, key, table copy
     return 0;
  }

  lua_pop(L, 1); // pop table copy
  return 1;
}

static int luaL_checkudata_ex(lua_State *L, int ud, const char *tname)
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


static void iterate_table(lua_State *L, int index, bson_t* bson, int use_keys, int level)
{
  lua_pushvalue(L, index);
  // stack: -1 => table

  lua_pushnil(L);
  // stack: -1 => nil; -2 => table

  int k=0;

  while (lua_next(L, -2))
  {
    // stack: -1 => value; -2 => key; -3 => table
    lua_pushvalue(L, -2);
    // stack: -1 => key; -2 => value; -3 => key; -4 => table

    const char *key;

    if (!use_keys)
    {
      char ckey[512];
      snprintf(ckey, 512, "%d", k);
      key = ckey;
    }
    else
    {
      key = lua_tostring(L, -1);
    }

    switch(lua_type(L, -2))
    {
      case LUA_TTABLE:
      {
        int is_a=is_array(L, -2);

        if (is_a)
        {
          bson_t child;
          //start array
          BSON_APPEND_ARRAY_BEGIN(bson, key, &child);
          iterate_table(L, -2, &child, 0, level+1);
          bson_append_array_end(bson, &child);
        }
        else
        {
          bson_t child;
          //start map
          BSON_APPEND_DOCUMENT_BEGIN(bson, key, &child);
          iterate_table(L, -2, &child, 1, level+1);
          bson_append_document_end(bson, &child);
        }
        break;
      }

      case LUA_TNIL:
      {
        BSON_APPEND_NULL(bson, key);
        break;
      }

      case LUA_TNUMBER:
      {
        BSON_APPEND_DOUBLE(bson, key, lua_tonumber(L, -2));
        break;
      }

      case LUA_TBOOLEAN:
      {
        BSON_APPEND_BOOL(bson, key, lua_toboolean(L, -2));
        break;
      }

      case LUA_TSTRING:
      {
        BSON_APPEND_UTF8(bson, key, lua_tostring(L, -2));
        break;
      }

      case LUA_TUSERDATA:
      {
        // switch userdata type
        if (luaL_checkudata_ex(L, -2, REGEX_METATABLE))
        {
          cbson_regex_t* regex = check_cbson_regex(L, -2);

          BSON_APPEND_REGEX (bson, key, regex->regex, regex->options);
        }
        else if (luaL_checkudata_ex(L, -2, OID_METATABLE))
        {
          cbson_oid_t* oid = check_cbson_oid(L, -2);
          bson_oid_t boid;

          bson_oid_init_from_string (&boid, oid->oid);

          BSON_APPEND_OID (bson, key, &boid);
        }
        else if (luaL_checkudata_ex(L, -2, BINARY_METATABLE))
        {
          cbson_binary_t* bin = check_cbson_binary(L, -2);

          size_t binary_len = b64_pton (bin->data, NULL, 0);
          unsigned char* buf=malloc(binary_len+1);
          b64_pton(bin->data, buf, binary_len+1);

          BSON_APPEND_BINARY(bson, key, bin->type, buf, binary_len);

          free(buf);
        }
        else if (luaL_checkudata_ex(L, -2, SYMBOL_METATABLE))
        {
          cbson_symbol_t* sym = check_cbson_symbol(L, -2);

          BSON_APPEND_SYMBOL(bson, key, sym->symbol);
        }
        else if (luaL_checkudata_ex(L, -2, REF_METATABLE))
        {
          cbson_ref_t* ref = check_cbson_ref(L, -2);

          bson_oid_t boid;
          bson_oid_init_from_string (&boid, ref->id);

          BSON_APPEND_DBPOINTER(bson, key, ref->ref, &boid);
        }
        else if (luaL_checkudata_ex(L, -2, MINKEY_METATABLE))
        {
          check_cbson_minkey(L, -2);
          BSON_APPEND_MINKEY(bson, key);
        }
        else if (luaL_checkudata_ex(L, -2, MAXKEY_METATABLE))
        {
          check_cbson_maxkey(L, -2);
          BSON_APPEND_MAXKEY(bson, key);
        }
        else if (luaL_checkudata_ex(L, -2, TIMESTAMP_METATABLE))
        {
          cbson_timestamp_t* time = check_cbson_timestamp(L, -2);
          BSON_APPEND_TIMESTAMP(bson, key, time->timestamp, time->increment);
        }
        else if (luaL_checkudata_ex(L, -2, INT64_METATABLE))
        {
          cbson_int64_t i = cbson_int64_check(L, -2);
          if (i < INT32_MIN || i > INT32_MAX)
          {
            BSON_APPEND_INT64(bson, key, i);
          }
          else
          {
            BSON_APPEND_INT32(bson, key, (int32_t)i);
          }
        }
        else if (luaL_checkudata_ex(L, -2, CODE_METATABLE))
        {
          cbson_code_t* code = check_cbson_code(L, -2);
          BSON_APPEND_CODE(bson, key, code->code);
        }
        else if (luaL_checkudata_ex(L, -2, CODEWSCOPE_METATABLE))
        {
          cbson_codewscope_t* code = check_cbson_codewscope(L, -2);
          BSON_APPEND_CODE_WITH_SCOPE(bson, key, code->code, NULL);
        }
        else if (luaL_checkudata_ex(L, -2, UNDEFINED_METATABLE))
        {
          check_cbson_undefined(L, -2);
          BSON_APPEND_UNDEFINED(bson, key);
        }
        else if (luaL_checkudata_ex(L, -2, DATE_METATABLE))
        {
          BSON_APPEND_DATE_TIME(bson, key, cbson_date_check(L, -2));
        }
        break;
      }
      case LUA_TFUNCTION:
      case LUA_TTHREAD:
      case LUA_TLIGHTUSERDATA:
      default:
        break; // or bail out?
    }

    lua_pop(L, 2);
    // stack: -1 => key; -2 => table
    k++;
  }
  // stack: -1 => table
  lua_pop(L, 1);
}

static int bson_encode(lua_State *L)
{
  bson_t bson = BSON_INITIALIZER;

  luaL_checktype(L, 1, LUA_TTABLE);

  iterate_table(L, 1,  &bson, 1, 0);

  const uint8_t* data=bson_get_data(&bson);
  lua_pushlstring(L, (const char*)data, bson.len);
  return 1;
}

#if LUA_VERSION_NUM >= 502

#define DECLARE_CLASS(state, name, type) \
  luaL_newmetatable(state, name##_METATABLE); \
  luaL_setfuncs(state, cbson_##type##_meta , 0); \
\
  lua_newtable(state); \
  luaL_setfuncs(state, cbson_##type##_methods , 0); \
\
  lua_setfield(state, -2, "__index")

#else

#define DECLARE_CLASS(state, name, type) \
  luaL_newmetatable(state, name##_METATABLE); \
  luaL_register(state, NULL, cbson_##type##_meta); \
\
  lua_newtable(state); \
  luaL_register(state, NULL, cbson_##type##_methods); \
\
  lua_setfield(state, -2, "__index")

#endif


int luaopen_cbson(lua_State *L)
{
  luaL_Reg cbsonlib[] = {
    { "decode",     bson_decode },
    { "encode",     bson_encode },
    { "regex",      cbson_regex_new },
    { "oid",        cbson_oid_new },
    { "binary",     cbson_binary_new },
    { "symbol",     cbson_symbol_new },
    { "code",       cbson_code_new },
    { "codewscope", cbson_codewscope_new },
    { "undefined",  cbson_undefined_new },
    { "minkey",     cbson_minkey_new },
    { "maxkey",     cbson_maxkey_new },
    { "ref",        cbson_ref_new },
    { "timestamp",  cbson_timestamp_new },
    { "int",        cbson_int64_new },
    { "date",       cbson_date_new },
    { NULL, NULL }
  };

  // types
  DECLARE_CLASS(L, REGEX,      regex);
  DECLARE_CLASS(L, OID,        oid);
  DECLARE_CLASS(L, BINARY,     binary);
  DECLARE_CLASS(L, SYMBOL,     symbol);
  DECLARE_CLASS(L, CODE,       code);
  DECLARE_CLASS(L, CODEWSCOPE, codewscope);
  DECLARE_CLASS(L, UNDEFINED,  undefined);
  DECLARE_CLASS(L, MINKEY,     minkey);
  DECLARE_CLASS(L, MAXKEY,     maxkey);
  DECLARE_CLASS(L, REF,        ref);
  DECLARE_CLASS(L, TIMESTAMP,  timestamp);
  DECLARE_CLASS(L, INT64,      int64);
  DECLARE_CLASS(L, DATE,       date);

  // cbson module
  lua_newtable(L);

#if LUA_VERSION_NUM >= 502
  luaL_setfuncs(L,  cbsonlib, 0);
#else
  luaL_register(L, NULL, cbsonlib);
#endif

  lua_pushliteral(L, CBSON_MODNAME);
  lua_setfield(L, -2, "_NAME");
  lua_pushliteral(L, CBSON_VERSION);
  lua_setfield(L, -2, "_VERSION");

  return 1;
}
