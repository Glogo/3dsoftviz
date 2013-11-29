#ifndef AST_H
#define AST_H

#include "Data/Node.h"
#include "Data/Graph.h"
#include "Diluculum/LuaVariable.hpp"
#include "Diluculum/LuaState.hpp"
#include "Data/Type.h"

namespace Metrics
{
    class AstTest {

    public:

        static void visualize(Data::Graph * graph);

        static void visualizeFiltered(Data::Graph * graph);

    private:
        static Diluculum::LuaState* luaState;

        static Data::Type * edgeType;
        static Data::Type * nodeType;

        static void traverseAst(Data::Graph * graph, Diluculum::LuaVariable luaNode, osg::ref_ptr<Data::Node> graphNode);

        static bool traverseAndFilterAst(Data::Graph * graph, Diluculum::LuaVariable luaNode, osg::ref_ptr<Data::Node> graphNode);

        static osg::ref_ptr<Data::Node> addLinkedNode(Data::Graph * graph, string name, osg::ref_ptr<Data::Node> graphNode);

    };
}

#endif // AST_H
