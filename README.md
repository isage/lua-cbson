#Lua-cbson

Lua bindings to MongoDB's libbson

##Table of Contents

* [FAQ](#faq)
* [Requirements](#requirements)
* [Installation](#installation)
* [Usage](#usage)
* [Authors](#authors)
* [Copyright and License](#copyright-and-license)

##FAQ

* Q: Is it compatible with cjson?  
  A: Hell no. All bson subtypes are defined as userdata. Use cbson.to_json and cbson.from_json
* Q: What about integers?  
  A: Lua, prior to 5.3 stores all numbers as double. So, cbson.encode({foo = 10}) will yield double bson type.  
     Use cbson.int datatype. It will automaticaly yield int32 or int64 depending on value.

##Requirements

* Lua (5.1/5.2) or LuaJit
* Cmake 2.8.12 or later
* Working C compiler
* libbson 1.2 or later

##Installation

As easy as  
```bash
mkdir build
cd build
cmake ..
make
make install
```

You can also use `make unittest` after make to run tests.  
By default module compiles with support for luajit  
For other Lua interpreters see cmake options.

##Usage

###Synopsis

```lua
local cbson = require "cbson"

local table_data = cbson.decode(bson_data)
local bson_data = cbson.encode(table_data)

local json_string = cbson.to_json(bson_data)
local bson_data = cbson.from_json(json_string)
```

###CBSON Functions

####`<table>decoded = cbson.decode(<binary>bson_data)`

Decodes binary BSON data to lua table.

####`<binary>bson_data = cbson.encode(<table>data)`

Encodes lua table to binary BSON data.

####`<binary>bson_data = cbson.from_json(<string>json)`

Encodes json string as binary BSON data.

####`<string>json_data = cbson.to_json(<binary>bson_data)`

Encodes binary BSON data as json string.

###Embed datatypes

####cbson.regex(<string>regex, <string>options)

```lua
local regex = cbson.regex("foo|bar","ism")
print( regex:regex() )
regex:regex("bar|foo")
regex:options("im")
print( regex:options() )
```

####cbson.oid(<string>oid)

```lua
local oid = cbson.oid("123456789012345678901234")
```

####cbson.binary(<string>base64_encoded_data)

```lua
local binary = cbson.binary("ZGVhZGJlZWY=")
print( binary:raw() ) -- "deadbeef"
binary:raw("hello")
print( binary:data() ) -- "aGVsbG8="
```

####cbson.symbol(<string>symbol)

```lua
local symbol = cbson.symbol("something")
```

####cbson.code(<string>code)

```lua
local code = cbson.code("code")
```

####cbson.codewscope(<string>code)

Scopes are not supported yet.

```lua
local code = cbson.codewcope("code")
```

####cbson.undefined()

```lua
local undef = cbson.undefined()
```

####cbson.minkey()

```lua
local minkey = cbson.minkey()
```

####cbson.maxkey()

```lua
local maxkey = cbson.maxkey()
```

####cbson.ref(<string>ref, <string>id)

```lua
local ref = cbson.ref("foo", "123456789012345678901234")
print( ref:ref() )
ref:ref("bar")
ref:id("432109876543210987654321")
```

####cbson.timestamp(<number>timestamp, <number>increment)

```lua
local timestamp = cbson.timestamp(100,1000)
timestamp:timestamp(500)
print( timestamp:timestamp() ) -- 500
timestamp:increment(100500)
print( timestamp:increment() ) -- 100500

```

####cbson.int(<number>value) or cbson.int(<int>value) or cbson.int(<string>value)

That's basically int64. It supports all arithmetic operations.  
Due to nature of lua numbers, you can pass big (> 2^53) numbers only as string.
It also have __tostring metamethod for opposite operation.

```lua
local int = cbson.int(10)
print(int) -- 10

local int = cbson.int("100")
print(int) -- 100

local int = cbson.int(10)
local int2 = cbson.int(int)
print(int2) -- 10

```

####cbson.date(<number>value) or cbson.date(<date>value) or cbson.date(<string>value)

See cbson.int. They are same, but you can't init date from int and vice versa (for now).

##Authors

Epifanov Ivan <isage.dna@gmail.com>

[Back to TOC](#table-of-contents)

##Copyright and License

This module is licensed under the WTFPL license.  
(See LICENSE)