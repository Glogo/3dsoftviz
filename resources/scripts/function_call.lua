metrics = require"metrics"

function vardump(value, depth, key, visited)
  local linePrefix = ""
  local spaces = ""
  
  if key ~= nil then
    linePrefix = "["..tostring(key).."] = "
  end
  
  if depth == nil then
    depth = 0
  else
    depth = depth + 1
    for i=1, depth do spaces = spaces .. "  " end
  end

  if visited == nil then 
    visited = {}
  end
  
  if type(value) == 'table' and visited[value] == nil then
    visited[value] = true    
    mTable = getmetatable(value)
    if mTable == nil then
      print(spaces ..linePrefix.."(table) "..tostring(value))
    else
      print(spaces .."(metatable) ")
        value = mTable
    end		
    for tableKey, tableValue in pairs(value) do
      vardump(tableValue, depth, tableKey, visited)
    end
  elseif type(value)	== 'function' or 
      type(value)	== 'thread' or 
      type(value)	== 'userdata' or
      value		== nil
  then
    print(spaces..tostring(value))
  else
    print(spaces..linePrefix.."("..type(value)..") "..tostring(value))
  end
end

files = {
  "../lib/lua/metrics/init.lua",
  "../lib/lua/metrics/rules.lua",
  "../lib/lua/metrics/utils.lua",
  "../lib/lua/metrics/captures/AST.lua",
  "../lib/lua/metrics/captures/block.lua",
  "../lib/lua/metrics/captures/cyclomatic.lua",
  "../lib/lua/metrics/captures/functiontree.lua",
  "../lib/lua/metrics/captures/halstead.lua",
  "../lib/lua/metrics/captures/infoflow.lua",
  "../lib/lua/metrics/captures/LOC.lua",
  "../lib/lua/metrics/captures/statements.lua",
  "../lib/lua/metrics/luadoc/captures.lua",
  "../lib/lua/metrics/luadoc/commentParser.lua"
}
asts = {}
for i,v in ipairs(files) do
  io.input(v);
  testFile = io.read("*all")
  asts[i] = metrics.processText(testFile);
end

result = metrics.doGlobalMetrics(asts)
print("done computing global metrics")
vardump(result)

