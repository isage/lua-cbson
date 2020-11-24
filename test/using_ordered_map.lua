-- XXX problem with luajit interpreter and it's hash funciton: when we create
-- some map, then pairs in the table can has not same order like we exactly set.
-- So we need use ordered_map for set order of keys in document exactly like we
-- want to
--
-- Why ordered_map can be needed? So it can be needed for sorting output in mongo.
-- There uses json documents for sorting and order of keys in the documents have
-- meaning
--
local cbson = require("cbson")

local ordered_map_mt = cbson.ordered_map_mt

local json_sample = '{ "alpha" : "alpha", "bravo" : "bravo" }'
local json_sample2 = '{ "bravo" : "bravo", "alpha" : "alpha" }'

-- first case
local data =
  setmetatable({{alpha = "alpha"}, {bravo = "bravo"}}, ordered_map_mt)
local out = cbson.encode(data)
local json = cbson.to_json(out)
if json ~= json_sample then
  error(string.format("expected: %s but get %s", json_sample, json))
end

-- second case
local data2 = setmetatable({{bravo = "bravo"}, {alpha = "alpha"}},
                           ordered_map_mt)
local out2 = cbson.encode(data2)
local json2 = cbson.to_json(out2)
if json2 ~= json_sample2 then
  error(string.format("expected: %s but get %s", json_sample2, json2))
end

print("ok")
