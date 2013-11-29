metrics = require"metrics"

function printNode(node, prefix)
  local tag = node.tag
  --if tag ~= "IGNORED" and tag ~= "SPACE" and tag ~= "NEWLINE" then
    --if tag == "Stat" or tag == "LastStat" then
      print(prefix .. tag)
      print(prefix .. node.text)
      print("----------------------------------------------------")
    --end
    for _,v in ipairs(node.data) do
          printNode(v, "  " .. prefix)
    end
  --end
end

io.input("../share/3dsoftviz/scripts/test1.lua");
testFile = io.read("*all")
ast = metrics.processText(testFile);
--printNode(ast, "")
