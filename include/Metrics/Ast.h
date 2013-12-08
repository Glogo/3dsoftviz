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

        static QList<osg::ref_ptr<Data::Node> > traverseAndFilterAst(Data::Graph * graph, Diluculum::LuaVariable luaNode, osg::ref_ptr<Data::Node> graphNode);

        static void traverseIf(Data::Graph * graph, Diluculum::LuaVariable luaNode, osg::ref_ptr<Data::Node> graphNode);

        static void renameGlobalFunction(Diluculum::LuaVariable luaNode, osg::ref_ptr<Data::Node> graphNode);

        static void renameLocalFunction(Diluculum::LuaVariable luaNode, osg::ref_ptr<Data::Node> graphNode);

        static void renameNumericFor(Diluculum::LuaVariable luaNode, osg::ref_ptr<Data::Node> graphNode);

        static void renameGenericFor(Diluculum::LuaVariable luaNode, osg::ref_ptr<Data::Node> graphNode);

        static void renameWhile(Diluculum::LuaVariable luaNode, osg::ref_ptr<Data::Node> graphNode);

        static void renameRepeat(Diluculum::LuaVariable luaNode, osg::ref_ptr<Data::Node> graphNode);

        static void createNodeTypes(Data::Graph * graph);

        static osg::ref_ptr<Data::Node> addLinkedNode(Data::Graph * graph, string name, osg::ref_ptr<Data::Node> graphNode);

        static QMap<QString, QString> * createSettings(Util::ApplicationConfig * appconf, string r, string g, string b);

        static bool isRelevantNode(string name);

        static void linkNodes(Data::Graph * graph, osg::ref_ptr<Data::Node> newNode,QList<osg::ref_ptr<Data::Node> > subNodes);
    };
}

#endif // AST_H
