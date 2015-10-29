#!/usr/bin/env luajit

bson = require("cbson")

function readAll(file)
    local f = io.open(file, "rb")
    local current = f:seek()
    local size = f:seek("end")
    f:seek("set", current) 
    local content = f:read("*all")
    f:close()
    return content, size
end

if not arg[1] then
  print("Usage: mongodump-decode.lua <filename.bson>")
  return -1
end

local data, size = readAll(arg[1]) -- 
local pos = 1  -- lua indexes start from 1

while pos <= size do
  local s1,s2,s3,s4 = data:byte(pos, pos+3)  -- four bytes
  local bson_size = s4*16777216+s3*65536+s2*256+s1  -- hackish int calculation

  local dt = data:sub(pos,pos+bson_size-1)  -- get bson data according to size

  print(bson.to_json(dt))  -- try to decode

  pos = pos + bson_size  -- advance
end
