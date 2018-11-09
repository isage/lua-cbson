# Lua-cbson

Lua bindings to MongoDB's libbson

## Table of Contents

* [FAQ](#faq)
* [Requirements](#requirements)
* [Installation](#installation)
* [Usage](#usage)
* [Authors](#authors)
* [Copyright and License](#copyright-and-license)

## FAQ

* Q: Is it compatible with cjson?  
  A: Hell no. All bson subtypes are defined as userdata. Use cbson.to_json and cbson.from_json
* Q: What about integers?  
  A: Lua, prior to 5.3 stores all numbers as double. So, cbson.encode({foo = 10}) will yield double bson type.  
     Use cbson.int datatype. It will automaticaly yield int32 or int64 depending on value.

## Requirements

* Lua (5.1/5.2) or LuaJit
* Cmake 2.8.12 or later
* Working C compiler
* libbson 1.7 or later

## Installation

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

## Usage

### Synopsis

```lua
local cbson = require "cbson"

local table_data = cbson.decode(bson_data)
local bson_data = cbson.encode(table_data)

local json_string = cbson.to_json(bson_data)
local json_string = cbson.to_relaxed_json(bson_data)
local bson_data = cbson.from_json(json_string)
```

### CBSON Functions

#### `<table>decoded = cbson.decode(<binary>bson_data)`

Decodes binary BSON data to lua table.

#### `<binary>bson_data = cbson.encode(<table>data)`

Encodes lua table to binary BSON data.

#### `<binary>bson_data = cbson.encode_first(<string>first_key, <table>data)`

Encodes lua table to binary BSON data, putting first_key value at start of bson.  
(required for mongodb commands)
Make sure, that given key exists, otherwise it'll add this key with NULL value.

#### `<binary>bson_data = cbson.from_json(<string>json)`

Encodes json string as binary BSON data.


#### `<string>json_data = cbson.to_json(<binary>bson_data)`

Encodes binary BSON data as json string.

#### `<string>json_data = cbson.to_relaxed_json(<binary>bson_data)`

Encodes binary BSON data as [relaxed json](https://github.com/mongodb/specifications/blob/master/source/extended-json.rst#relaxed-extended-json-example) string.

### Embed datatypes

#### `cbson.regex(<string>regex, <string>options)`

```lua
local regex = cbson.regex("foo|bar","ism")
print( regex:regex() )
regex:regex("bar|foo")
regex:options("im")
print( regex:options() )
```

#### `cbson.oid(<string>oid)`

Unique object id with 24 hex numbers.

```lua
local oid = cbson.oid("1234567890ABCDEF01234567")
```

Get time from `oid` (first 8 hex numbers):

```lua
local timestamp = oid:timestamp()
print(timestamp) -- 305419896
```

#### `cbson.binary(<string>base64_encoded_data[, <int> type])`

```lua
local binary = cbson.binary("ZGVhZGJlZWY=")
print( binary:raw() ) -- returns raw binary data "deadbeef"
binary:set_raw("hello") -- binary:raw("hello") works fine
print( binary:data() ) -- returns base64 encoded data "aGVsbG8="
binary:set_data("ZGVhZGJlZWY=")
```

Maximum size of binary string is `4,294,967,295` bytes.

#### `cbson.symbol(<string>symbol)`

```lua
local symbol = cbson.symbol("something")
```

#### `cbson.code(<string>code)`

```lua
local code = cbson.code("code")
```

#### `cbson.codewscope(<string>code)`

Scopes are not supported yet.

```lua
local code = cbson.codewcope("code")
```

#### `cbson.undefined()`

```lua
local undef = cbson.undefined()
```

#### `cbson.null()`

```lua
local null = cbson.null()
```

#### `cbson.array()`

```lua
local empty_array = cbson.array()
```

#### `cbson.minkey()`

```lua
local minkey = cbson.minkey()
```

#### `cbson.maxkey()`

```lua
local maxkey = cbson.maxkey()
```

#### `cbson.ref(<string>ref, <string>id)`

```lua
local ref = cbson.ref("foo", "123456789012345678901234")
print( ref:ref() )
ref:ref("bar")
ref:id("432109876543210987654321")
```

#### `cbson.timestamp(<number>timestamp, <number>increment)`

BSON has a special [`timestamp`](https://docs.mongodb.com/manual/reference/bson-types/#timestamps) type for _internal_ 
MongoDB use and is not associated with the regular `date` type. 

```lua
local timestamp = cbson.timestamp(100,1000)
timestamp:timestamp(500)
print( timestamp:timestamp() ) -- 500
timestamp:increment(100500)
print( timestamp:increment() ) -- 100500

```

#### `cbson.int(<number>value) or cbson.int(<int>value) or cbson.int(<string>value)`

That's basically int64. It supports all arithmetic operations.  
Due to nature of lua numbers, you can pass big (> 2^53) numbers only as string.
It also have __tostring metamethod for opposite operation.

There's also helper methods `int_to_raw/uint_to_raw(<int>value, <number>bytes, <bool>big_endian)`  
and `raw_to_int/raw_to_uint(<string>data, <bool>big_endian)`  
for converting between number and in-memory representation.

```lua
local int = cbson.int(10)
print(int) -- 10

local int = cbson.int("100")
print(int) -- 100

local int = cbson.int("100")
print(int:number() + 20) -- 120

local int = cbson.int(10)
local int2 = cbson.int(int)
print(int2) -- 10
```

#### `cbson.uint(<number>value) or cbson.uint(<uint>value) or cbson.uint(<string>value)`

See cbson.int. They are same (except sign), and you can init one from another.

#### `cbson.date(<number>value) or cbson.date(<date>value) or cbson.date(<string>value)`

BSON Date is a 64-bit integer that represents the number of milliseconds since the Unix epoch (Jan 1, 1970).
This results in a representable date range of about 290 million years into the past and future.

#### `cbson.decimal(<string>value)`

Since version `1.1`.


It's [decimal128](https://github.com/mongodb/specifications/blob/master/source/bson-decimal128/decimal128.rst).
The Decimal128 supports 34 decimal digits of precision,
a max value of approximately `10^6145`, and min value of approximately `-10^6145`

```lua
local dec = cbson.decimal("1.0")
print(dec) -- 1.0

local dec = cbson.decimal("0.0005")
print(dec) -- 0.0005
```

## Authors

Epifanov Ivan <isage.dna@gmail.com>

[Back to TOC](#table-of-contents)

## Copyright and License

This module is licensed under the WTFPL license.  
(See LICENSE)
