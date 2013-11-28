#include "Metrics/Ast.h"
#include "Importer/GraphOperations.h"

// include diluculum header files
#include "Diluculum/CppObject.hpp"
#include "Diluculum/LuaExceptions.hpp"
#include "Diluculum/LuaFunction.hpp"
#include "Diluculum/LuaState.hpp"
#include "Diluculum/LuaUserData.hpp"
#include "Diluculum/LuaUtils.hpp"
#include "Diluculum/LuaValue.hpp"
#include "Diluculum/LuaVariable.hpp"
#include "Diluculum/Types.hpp"


namespace Metrics
{

    Diluculum::LuaState* AstTest::luaState;

    Data::Type * AstTest::edgeType;
    Data::Type * AstTest::nodeType;

    void AstTest::visualize(Data::Graph * graph){
        Importer::GraphOperations * operations = new Importer::GraphOperations(*graph);
        operations->addDefaultTypes(AstTest::edgeType, AstTest::nodeType);

        luaState = new Diluculum::LuaState;
        luaState->doFile("../share/3dsoftviz/scripts/main.lua");
        Diluculum::LuaVariable ast = (*luaState)["ast"];
        traverseAst(graph, ast, NULL);

        luaState->~LuaState();
    }

    void AstTest::visualizeFiltered(Data::Graph * graph){
        Importer::GraphOperations * operations = new Importer::GraphOperations(*graph);
        operations->addDefaultTypes(AstTest::edgeType, AstTest::nodeType);

        luaState = new Diluculum::LuaState;
        luaState->doFile("../share/3dsoftviz/scripts/main.lua");
        Diluculum::LuaVariable ast = (*luaState)["ast"];
        traverseAndFilterAst(graph, ast, NULL);

        luaState->~LuaState();
    }

    void AstTest::traverseAst(Data::Graph * graph, Diluculum::LuaVariable luaNode, osg::ref_ptr<Data::Node> graphNode){
        osg::ref_ptr<Data::Node> newNode;
        try {
            string s = luaNode["tag"].value().asString();
            cout << s << endl;
            QString tag = QString::fromStdString(s);
            newNode = graph->addNode(tag, nodeType);
            if (graphNode != NULL) graph->addEdge(NULL, graphNode, newNode, AstTest::edgeType, true);
        } catch (Diluculum::TypeMismatchError e){
            return;
        }
        Diluculum::LuaVariable data = luaNode["data"];
        int i = 1;
        while (true){
            try {
                data[i]["tag"].value();
                traverseAst(graph, data[i], newNode);
                i++;
            } catch (Diluculum::TypeMismatchError e){
                break;
            }
        }
    }

    void AstTest::traverseAndFilterAst(Data::Graph * graph, Diluculum::LuaVariable luaNode, osg::ref_ptr<Data::Node> graphNode){
        osg::ref_ptr<Data::Node> newNode;
        try {
            string s = luaNode["tag"].value().asString();
            cout << s << endl;
            QString tag = QString::fromStdString(s);
            newNode = graph->addNode(tag, nodeType);
            if (graphNode != NULL) graph->addEdge(NULL, graphNode, newNode, AstTest::edgeType, true);
        } catch (Diluculum::TypeMismatchError e){
            return;
        }
        Diluculum::LuaVariable data = luaNode["data"];
        int i = 1;
        while (true){
            try {
                data[i]["tag"].value();
                traverseAst(graph, data[i], newNode);
                i++;
            } catch (Diluculum::TypeMismatchError e){
                break;
            }
        }

    }

}


