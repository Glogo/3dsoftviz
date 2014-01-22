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
  asts[v] = metrics.processText(testFile);
end

result = metrics.doGlobalMetrics(asts)
print("done computing global metrics")
--vardump(result)
a = result["statements"]["FunctionCall"][1]
--while a["parent"] ~= null and 
--    (a["tag"] ~= "LocalFunction" or a["tag"] ~= "GlobalFunction") do
  a = a["parent"]
--end
for k,v in pairs(a) do
  print(k,v)
end

print("------------------------")

--[[
halstead	table: 0xa30984b0
  	unique_operands	1475
  	DIF	60,508474576271
  	LTH	12654
  	time	449493,54318242
  	VOL	133714,88595511
  	operators	table: 0xa3097b50
  	EFF	8090883,7772835
  	BUG	44,571628651702
  	VOC	1517
  	number_of_operators	8404
  	operands	table: 0xa3097b78
  	unique_operators	42
  	number_of_operands	4250
moduleDefinitions	table: 0xa309e3b0
  	1	table: 0xaa317838
        halstead	table: 0xaa396d80
        moduleReferences	table: 0xaa37efa8
            metrics.captures.functiontree	table: 0xaa2e3b28
            metrics.captures.LOC	table: 0xaa2e3638
            metrics.rules	table: 0xaa2f7748
            metrics.captures.halstead	table: 0xaa2e39a0
            metrics.utils	table: 0xaa2e3850
            metrics.luadoc.captures	table: 0xaa2e3be8
            leg.parser	table: 0xaa2e3fb0
            metrics.captures.block	table: 0xaa2e37f0
            metrics.captures.cyclomatic	table: 0xaa2e3d68
            metrics.captures.statements	table: 0xaa2e3ca8
            metrics.captures.infoflow	table: 0xaa2e38e0
            metrics.captures.AST	table: 0xaa2e3730
        moduleName	metrics
        path  "../lib/..."
        moduleCalls	table: 0xaa37ee30
            lpeg	table: 0xaa3d12c8
            metrics.captures.halstead	table: 0xaa3d1388
            leg.grammar	table: 0xaa3d1328
        LOC	table: 0xaa395e48
        statements	table: 0xaa39a660
  	2	table: 0xa9c43458
  	3	table: 0xa9629018
  	4	table: 0xa84385f0
  	...
file_AST_list	table: 0xaad605c8
  	1	table: 0xaa317548
        AST by processText
  	2	table: 0xa9c432b0
  	3	table: 0xa9628e70
  	4	table: 0xa8438448
  	...
LOC	table: 0xa3097990
  	lines_blank	544
  	lines_comment	384
  	lines	2813
  	lines_code	1945
  	lines_nonempty	2269
statements	table: 0xa3098bc0
  	FunctionCall	table: 0xa3098cb0 //not assign function, _PrefixExpArgs
        1 table
            order	1
            position	1000
            tag	FunctionCall
            parent	table: 0xaa9bb568
            text	module ("metrics")
            data	table: 0xaa9badf8
            str	module ("metrics")
            key	FunctionCall
            metrics	table: 0xaa9bafe8
        ...
  	GenericFor	table: 0xa3098e50
  	LocalFunction	table: 0xa3099098
  	While	table: 0xa3099360
  	GlobalFunction	table: 0xa3098e08
  	If	table: 0xa3098ea0
  	LocalAssign	table: 0xa3098e78
  	NumericFor	table: 0xa309b348
  	Assign	table: 0xa3099070
functionDefinitions	table: 0xa3097348
  	1	table: 0xaa273690
        order	1
        position	522
        str	local function Cp(...)
	        return lpeg.Cp() * lpeg.C(...)
        end
        metrics	table: 0xaa284540
        text	local function Cp(...)
	        return lpeg.Cp() * lpeg.C(...)
        end
        path	"../lib/..."
        tag	LocalFunction
        data	table: 0xaa286a00
        isGlobal	false
        parent	table: 0xaa2c18c8
        name	Cp
        key	LocalFunction
        fcntype	local
  	2	table: 0xa50bb978
  	...
--]]
