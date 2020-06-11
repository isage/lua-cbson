local cjson = require("cjson")
local cbson = require("cbson")

cjson.decode_array_with_array_mt(true)
cbson.set_array_mt(cjson.array_mt)

local json = [[
{
  "test_cases": [
    {"object": {
      "some_data": "some_data"
    }},
    {"array": [1, 2, 3]},
    {"empty_obj": {}},
    {"empty_arr": []}
  ]
}
]]

local function main()
  local t = cjson.decode(json)

  local control = cjson.encode(t)

  local bson = cbson.encode(t)
  local out = cbson.decode(bson)

  local check = cjson.encode(out)

  if control ~= check then
    print(control)
    print(check)
    error("array metatable test failed")
  end
end

main()
