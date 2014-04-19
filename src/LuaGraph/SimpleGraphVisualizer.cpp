#include "LuaGraph/SimpleGraphVisualizer.h"
#include "LuaGraph/HyperGraphVisualizer.h"
#include "LuaGraph/LuaGraphVisualizer.h"
#include "LuaGraph/LuaGraph.h"
#include "Importer/GraphOperations.h"
#include "limits"

Lua::SimpleGraphVisualizer::SimpleGraphVisualizer(Data::Graph *graph)
    : Lua::LuaGraphVisualizer(graph){}

void Lua::SimpleGraphVisualizer::visualize()
{
    Lua::LuaGraph *g = Lua::LuaGraph::loadGraph();
    g->printGraph();

    for (QMap<qlonglong, Lua::LuaNode *>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i){
        osg::ref_ptr<Data::Node> n = currentGraph->addNode(i.key() , i.value()->getLabel(), nodeType);
        setNodeParams(n, i.value(), osg::Vec4f(1,0,0,1), 8);
    }

    for (QMap<qlonglong, Lua::LuaEdge *>::iterator i = g->getEdges()->begin(); i != g->getEdges()->end(); ++i){
        if (i.value()->getIncidences().size() != 2) throw new std::runtime_error("Not a simple graph");
        LuaIncidence * const incid1 = g->getIncidences()->value(i.value()->getIncidences()[0]);
        LuaIncidence * const incid2 = g->getIncidences()->value(i.value()->getIncidences()[1]);
        osg::ref_ptr<Data::Node> srcNode = currentGraph->getNodes()->value(incid1->getEdgeNodePair().second);
        osg::ref_ptr<Data::Node> dstNode = currentGraph->getNodes()->value(incid2->getEdgeNodePair().second);
        if (incid1->getOriented()){
            if (incid1->getOutGoing()){
                currentGraph->addEdge(i.key(), i.value()->getLabel(), dstNode, srcNode, edgeType, true);
            } else {
                currentGraph->addEdge(i.key(), i.value()->getLabel(), srcNode, dstNode, edgeType, true);
            }
        } else {
            currentGraph->addEdge(i.key(), i.value()->getLabel(), srcNode, dstNode, edgeType, false);
        }
    }
    g->setObserver(this);

    QString metaNodeName = "metaNode";
    QString metaEdgeName = "metaEdge";
    osg::ref_ptr<Data::Node> filesAnchor = currentGraph->addNode(std::numeric_limits<qlonglong>::max(),metaNodeName, currentGraph->getNodeMetaType(), osg::Vec3(0, 0, 1000));

    for (QMap<qlonglong, Lua::LuaNode *>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i){
        if (i.value()->getParams()["root"]== true){
            std::cout << "found root " << i.key() << std::endl;
            std::cout << "connecting to files meta node " << filesAnchor->getId() << std::endl;
            osg::ref_ptr<Data::Node> root = currentGraph->getNodes()->value(i.key());
            osg::ref_ptr<Data::Edge> me = currentGraph->addEdge(metaEdgeName, root, filesAnchor, currentGraph->getEdgeMetaType(), false);
            std::cout << "meta edge id "<< me->getId() << std::endl;
            break;
        }
    }
}

void Lua::SimpleGraphVisualizer::onUpdate()
{
    std::cout << "Graph update called" << std::endl;
}



