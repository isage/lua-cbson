luaunit = require('luaunit')

local function readAll(file)
    local f = io.open(file, "rb")
    local content = f:read("*all")
    f:close()
    return content
end

local regexes = { -- sometimes regex flags change positions
    ["/foo|bar/ims"] = true,
    ["/foo|bar/ism"] = true
}

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
        luaunit.assertEquals(self.data["bar"], self.cbson.int(12341))
    end

    function TestBSON:test04_Decode_double()
        luaunit.assertNotNil(self.data["baz"])
        luaunit.assertEquals(self.data["baz"], 123.456)
    end

    function TestBSON:test05_Decode_map()
        luaunit.assertNotNil(self.data["map"])
        luaunit.assertNotNil(self.data["map"]["a"])
        luaunit.assertEquals(self.data["map"]["a"], self.cbson.int(1))
    end

    function TestBSON:test06_Decode_array()
        luaunit.assertNotNil(self.data["array"])
        luaunit.assertNotNil(self.data["array"][1])
        luaunit.assertNotNil(self.data["array"][2])
        luaunit.assertNotNil(self.data["array"][3])
        luaunit.assertNotNil(self.data["array"][4])
        luaunit.assertEquals(self.data["array"][1], self.cbson.int(1))
        luaunit.assertEquals(self.data["array"][2], self.cbson.int(2))
        luaunit.assertEquals(self.data["array"][3], self.cbson.int(3))
        luaunit.assertEquals(self.data["array"][4], self.cbson.int(4))
    end

    function TestBSON:test07_Decode_null()
        luaunit.assertEquals(self.data["null"], self.cbson.null())
    end

    function TestBSON:test08_Decode_bool()
        luaunit.assertNotNil(self.data["boolean"])
        luaunit.assertTrue(self.data["boolean"])
    end

    function TestBSON:test09_Decode_oid()
        luaunit.assertNotNil(self.data["oid"])
        luaunit.assertIsUserdata(self.data["oid"])
        luaunit.assertEquals(tostring(self.data["oid"]), "000000000000000000000000")
        luaunit.assertEquals(self.data["oid"]:timestamp(), 0)
    end

    function TestBSON:test10_Decode_binary()
        luaunit.assertNotNil(self.data["binary"])
        luaunit.assertEquals(self.data["binary"]:type(), 0)
        luaunit.assertEquals(self.data["binary"]:data(), "ZGVhZGJlZWY=")
        luaunit.assertEquals(self.data["binary"]:raw(), "deadbeef")
        luaunit.assertEquals(self.data["binary"]:data("aGVsbG8="), "aGVsbG8=")
        luaunit.assertEquals(self.data["binary"]:raw(), "hello")
    end

    function TestBSON:test11_Decode_regex()
        luaunit.assertNotNil(self.data["regex"])
        luaunit.assertIsUserdata(self.data["regex"])
        luaunit.assertTrue(regexes[tostring(self.data["regex"])])
    end

    function TestBSON:test12_Decode_date()
        luaunit.assertNotNil(self.data["date"])
        luaunit.assertNotNil(self.data["date"])
        luaunit.assertEquals(self.data["date"], self.cbson.date(10000))
    end

    function TestBSON:test13_Decode_ref()
        luaunit.assertNotNil(self.data["ref"])
        luaunit.assertEquals(self.data["ref"]:ref(), "foo")
        luaunit.assertEquals(self.data["ref"]:id(), "000000000000000000000000")
    end

    function TestBSON:test14_Decode_undefined()
        luaunit.assertNotNil(self.data["undefined"])
        luaunit.assertEquals(tostring(self.data["undefined"]), "undefined")
    end

    function TestBSON:test15_Decode_minkey()
        luaunit.assertNotNil(self.data["minkey"])
        luaunit.assertEquals(tostring(self.data["minkey"]), "minkey")
    end

    function TestBSON:test16_Decode_maxkey()
        luaunit.assertNotNil(self.data["maxkey"])
        luaunit.assertEquals(tostring(self.data["maxkey"]), "maxkey")
    end

    function TestBSON:test17_Decode_timestamp()
        luaunit.assertNotNil(self.data["timestamp"])
        luaunit.assertEquals(self.data["timestamp"]:timestamp(),100)
        luaunit.assertEquals(self.data["timestamp"]:increment(),1000)
        luaunit.assertEquals(self.data["timestamp"]:increment(500),500)
        luaunit.assertEquals(self.data["timestamp"]:increment(),500)
    end

    function TestBSON:test18_Encode_string()
        local encoded = self.cbson.encode({foo = "bar"})
        luaunit.assertNotNil(encoded)
        local decoded = self.cbson.decode(encoded)
        luaunit.assertNotNil(decoded)
        luaunit.assertNotNil(decoded["foo"])
        luaunit.assertEquals(decoded["foo"], "bar")
    end

    function TestBSON:test19_Encode_int()
        local encoded = self.cbson.encode({foo = self.cbson.int(10)})
        luaunit.assertNotNil(encoded)
        local decoded = self.cbson.decode(encoded)
        luaunit.assertNotNil(decoded)
        luaunit.assertNotNil(decoded["foo"])
        luaunit.assertTrue(decoded["foo"] == self.cbson.int(10))
    end

    function TestBSON:test20_Encode_double()
        local encoded = self.cbson.encode({foo = 10.5})
        luaunit.assertNotNil(encoded)
        local decoded = self.cbson.decode(encoded)
        luaunit.assertNotNil(decoded)
        luaunit.assertNotNil(decoded["foo"])
        luaunit.assertTrue(decoded["foo"] == 10.5)
    end

    function TestBSON:test21_Encode_map()
        local encoded = self.cbson.encode({foo = { bar = 10.5}})
        luaunit.assertNotNil(encoded)
        local decoded = self.cbson.decode(encoded)
        luaunit.assertNotNil(decoded)
        luaunit.assertNotNil(decoded["foo"])
        luaunit.assertNotNil(decoded["foo"]["bar"])
        luaunit.assertTrue(decoded["foo"]["bar"] == 10.5)
    end

    function TestBSON:test22_Decode_decimal()
        luaunit.assertNotNil(self.data["dec"])
        luaunit.assertEquals(tostring(self.data["dec"]), "0.05")
    end

    function TestBSON:test23_Encode_decimal()
        local encoded = self.cbson.encode({foo = "0.05"})
        luaunit.assertNotNil(encoded)
        local decoded = self.cbson.decode(encoded)
        luaunit.assertNotNil(decoded)
        luaunit.assertNotNil(decoded["foo"])
        luaunit.assertTrue(decoded["foo"] == "0.05")
    end

    function TestBSON:test24_New_binary_empty()
        local b = self.cbson.binary()
        luaunit.assertTrue(b:data() == "")
        local b = self.cbson.binary("")
        luaunit.assertTrue(b:data() == "")
        local b = self.cbson.binary("ZGVhZGJlZWY=", 0)
        luaunit.assertTrue(b:data() == "ZGVhZGJlZWY=")
    end


TestBSONEncode = {}

    function TestBSONEncode:setUp() 
      self.cbson = require("cbson")
      self.data = self.cbson.decode(self.cbson.encode(self.cbson.decode(readAll("input.bson"))))
    end

    function TestBSONEncode:test01_Decode()
        luaunit.assertNotNil(self.data)
    end

    function TestBSONEncode:test02_Decode_string()
        luaunit.assertNotNil(self.data["foo"])
        luaunit.assertEquals(self.data["foo"], "bar")
    end

    function TestBSONEncode:test03_Decode_int()
        luaunit.assertNotNil(self.data["bar"])
        luaunit.assertEquals(tostring(self.data["bar"]), "12341")
    end

    function TestBSONEncode:test04_Decode_double()
        luaunit.assertNotNil(self.data["baz"])
        luaunit.assertEquals(self.data["baz"], 123.456)
    end

    function TestBSONEncode:test05_Decode_map()
        luaunit.assertNotNil(self.data["map"])
        luaunit.assertNotNil(self.data["map"]["a"])
        luaunit.assertEquals(tostring(self.data["map"]["a"]), "1")
    end

    function TestBSONEncode:test06_Decode_array()
        luaunit.assertNotNil(self.data["array"])
        luaunit.assertNotNil(self.data["array"][1])
        luaunit.assertNotNil(self.data["array"][2])
        luaunit.assertNotNil(self.data["array"][3])
        luaunit.assertNotNil(self.data["array"][4])
        luaunit.assertEquals(tostring(self.data["array"][1]), "1")
        luaunit.assertEquals(tostring(self.data["array"][2]), "2")
        luaunit.assertEquals(tostring(self.data["array"][3]), "3")
        luaunit.assertEquals(tostring(self.data["array"][4]), "4")
    end

    function TestBSONEncode:test07_Decode_null()
        luaunit.assertEquals(self.data["null"], self.cbson.null())
    end

    function TestBSONEncode:test08_Decode_bool()
        luaunit.assertNotNil(self.data["boolean"])
        luaunit.assertTrue(self.data["boolean"])
    end

    function TestBSONEncode:test09_Decode_oid()
        luaunit.assertNotNil(self.data["oid"])
        luaunit.assertIsUserdata(self.data["oid"])
        luaunit.assertEquals(tostring(self.data["oid"]), "000000000000000000000000")
    end

    function TestBSONEncode:test10_Decode_binary()
        luaunit.assertNotNil(self.data["binary"])
        luaunit.assertEquals(self.data["binary"]:type(), 0)
        luaunit.assertEquals(self.data["binary"]:data(), "ZGVhZGJlZWY=")
        luaunit.assertEquals(self.data["binary"]:raw(), "deadbeef")
        luaunit.assertEquals(self.data["binary"]:data("aGVsbG8="), "aGVsbG8=")
        luaunit.assertEquals(self.data["binary"]:raw(), "hello")
    end

    function TestBSONEncode:test11_Decode_regex()
        luaunit.assertNotNil(self.data["regex"])
        luaunit.assertIsUserdata(self.data["regex"])
        luaunit.assertTrue(regexes[tostring(self.data["regex"])])
    end

    function TestBSONEncode:test12_Decode_date()
        luaunit.assertNotNil(self.data["date"])
        luaunit.assertNotNil(self.data["date"])
        luaunit.assertEquals(tostring(self.data["date"]), "10000")
    end

    function TestBSONEncode:test13_Decode_ref()
        luaunit.assertNotNil(self.data["ref"])
        luaunit.assertEquals(self.data["ref"]:ref(), "foo")
        luaunit.assertEquals(self.data["ref"]:id(), "000000000000000000000000")
    end

    function TestBSONEncode:test14_Decode_undefined()
        luaunit.assertNotNil(self.data["undefined"])
        luaunit.assertEquals(tostring(self.data["undefined"]), "undefined")
    end

    function TestBSONEncode:test15_Decode_minkey()
        luaunit.assertNotNil(self.data["minkey"])
        luaunit.assertEquals(tostring(self.data["minkey"]), "minkey")
    end

    function TestBSONEncode:test16_Decode_maxkey()
        luaunit.assertNotNil(self.data["maxkey"])
        luaunit.assertEquals(tostring(self.data["maxkey"]), "maxkey")
    end

    function TestBSONEncode:test17_Decode_timestamp()
        luaunit.assertNotNil(self.data["timestamp"])
        luaunit.assertEquals(self.data["timestamp"]:timestamp(),100)
        luaunit.assertEquals(self.data["timestamp"]:increment(),1000)
        luaunit.assertEquals(self.data["timestamp"]:increment(500),500)
        luaunit.assertEquals(self.data["timestamp"]:increment(),500)
    end

    function TestBSONEncode:test18_Decode_decimal()
        luaunit.assertNotNil(self.data["dec"])
        luaunit.assertEquals(tostring(self.data["dec"]), "0.05")
    end

os.exit( luaunit.LuaUnit.run() )

