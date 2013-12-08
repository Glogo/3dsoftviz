metrics = require"metrics"

function printNode(node, prefix)
  local tag = node.tag
      print(prefix .. tag)
      print(node.text)
      print("----------------------------------------------------")
    for _,v in ipairs(node.data) do
          printNode(v, "  " .. prefix)
    end
end

io.input("../share/3dsoftviz/scripts/test1.lua");
testFile = io.read("*all")
ast = metrics.processText(testFile);
printNode(ast, "")
