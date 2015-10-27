#Lua-cbson

Lua bindings to MongoDB's libbson

##Table of Contents

* [Requirements](#requirements)
* [Installation](#installation)
* [Usage](#usage)
* [Authors](#authors)
* [Copyright and License](#copyright-and-license)

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
```

##Authors

Epifanov Ivan <isage.dna@gmail.com>

[Back to TOC](#table-of-contents)

##Copyright and License

This module is licensed under the WTFPL license.  
(See LICENSE)