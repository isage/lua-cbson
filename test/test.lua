luaunit = require('luaunit')

local function readAll(file)
    local f = io.open(file, "rb")
    local content = f:read("*all")
    f:close()
    return content
end

TestBSON = {}

    function TestBSON:setUp() 
      self.cbson = require("cbson")
      self.data = self.cbson.decode(readAll("input.bson"))
    end

    function TestBSON:test01_Decode()
        luaunit.assertNotNil(self.data)
    end

    function TestBSON:test02_Decode_string()
        luaunit.assertNotNil(self.data["foo"])
        luaunit.assertEquals(self.data["foo"], "bar")
    end

    function TestBSON:test03_Decode_int()
        luaunit.assertNotNil(self.data["bar"])
        luaunit.assertEquals(self.data["bar"], 12341)
    end

    function TestBSON:test04_Decode_double()
        luaunit.assertNotNil(self.data["baz"])
        luaunit.assertEquals(self.data["baz"], 123.456)
    end

    function TestBSON:test05_Decode_map()
        luaunit.assertNotNil(self.data["map"])
        luaunit.assertEquals(self.data["map"], {a=1})
    end

    function TestBSON:test06_Decode_array()
        luaunit.assertNotNil(self.data["array"])
        luaunit.assertEquals(self.data["array"], {1,2,3,4})
    end

    function TestBSON:test07_Decode_null()
        luaunit.assertNil(self.data["null"])
    end

    function TestBSON:test08_Decode_bool()
        luaunit.assertNotNil(self.data["boolean"])
        luaunit.assertTrue(self.data["boolean"])
    end

    function TestBSON:test09_Decode_oid()
        luaunit.assertNotNil(self.data["oid"])
        luaunit.assertNotNil(self.data["oid"]["$oid"])
        luaunit.assertEquals(self.data["oid"]["$oid"], "000000000000000000000000")
    end

    function TestBSON:test10_Decode_binary()
        luaunit.assertNotNil(self.data["binary"])
        luaunit.assertNotNil(self.data["binary"]["$type"])
        luaunit.assertEquals(self.data["binary"]["$type"], "00")
        luaunit.assertNotNil(self.data["binary"]["$binary"])
        luaunit.assertEquals(self.data["binary"]["$binary"], "ZGVhZGJlZWY=")
    end

    function TestBSON:test11_Decode_regex()
        luaunit.assertNotNil(self.data["regex"])
        luaunit.assertNotNil(self.data["regex"]["$regex"])
        luaunit.assertEquals(self.data["regex"]["$regex"], "foo|bar")
        luaunit.assertNotNil(self.data["regex"]["$options"])
        luaunit.assertEquals(self.data["regex"]["$options"], "ism")
    end

    function TestBSON:test12_Decode_date()
        luaunit.assertNotNil(self.data["date"])
        luaunit.assertNotNil(self.data["date"]["$date"])
        luaunit.assertEquals(self.data["date"]["$date"], 10000)
    end

    function TestBSON:test13_Decode_ref()
        luaunit.assertNotNil(self.data["ref"])
        luaunit.assertNotNil(self.data["ref"]["$ref"])
        luaunit.assertEquals(self.data["ref"]["$ref"], "foo")
        luaunit.assertNotNil(self.data["ref"]["$id"])
        luaunit.assertEquals(self.data["ref"]["$id"], "000000000000000000000000")
    end

    function TestBSON:test14_Decode_undefined()
        luaunit.assertNotNil(self.data["undefined"])
        luaunit.assertNotNil(self.data["undefined"]["$undefined"])
        luaunit.assertTrue(self.data["undefined"]["$undefined"])
    end

    function TestBSON:test15_Decode_minkey()
        luaunit.assertNotNil(self.data["minkey"])
        luaunit.assertNotNil(self.data["minkey"]["$minKey"])
        luaunit.assertEquals(self.data["minkey"]["$minKey"], 1)
    end

    function TestBSON:test16_Decode_maxkey()
        luaunit.assertNotNil(self.data["maxkey"])
        luaunit.assertNotNil(self.data["maxkey"]["$maxKey"])
        luaunit.assertEquals(self.data["maxkey"]["$maxKey"], 1)
    end

    function TestBSON:test17_Decode_timestamp()
        luaunit.assertNotNil(self.data["timestamp"])
        luaunit.assertNotNil(self.data["timestamp"]["$timestamp"])
        luaunit.assertEquals(self.data["timestamp"]["$timestamp"], {t=100, i=1000})
    end

os.exit( luaunit.LuaUnit.run() )

