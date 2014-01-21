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
        createNodeTypes(graph);
        traverseAndFilterAst(graph, ast, NULL);
        luaState->~LuaState();
    }

    void AstTest::traverseAst(Data::Graph * graph, Diluculum::LuaVariable luaNode, osg::ref_ptr<Data::Node> graphNode){
        osg::ref_ptr<Data::Node> newNode;
        try {
            string s = luaNode["tag"].value().asString();
//            cout << s << endl;
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

    QList<osg::ref_ptr<Data::Node> > AstTest::traverseAndFilterAst(Data::Graph * graph, Diluculum::LuaVariable luaNode,
                                                           osg::ref_ptr<Data::Node> graphNode){
        QList<osg::ref_ptr<Data::Node> > result;
        osg::ref_ptr<Data::Node> newNode = NULL;
        try {
            string s = luaNode["tag"].value().asString();
            if (isRelevantNode(s)){
                QString tag = QString::fromStdString(s);
                QList<Data::Type *> types = graph->getTypesByName(QString::fromStdString(s));
                if (types.size() > 0){
                    newNode = graph->addNode(tag, types.at(0));
                } else
                    newNode = graph->addNode(tag, nodeType);
                if (s == "Assign" || s == "LocalAssign" || s == "COMMENT" || s == "FunctionCall"
                        || s == "LastStat"){
                    newNode.get()->setLabelText(QString::fromStdString(luaNode["text"].value().asString()));
                    newNode.get()->reloadConfig();
                }
                result.append(newNode);
            }

            Diluculum::LuaVariable data = luaNode["data"];
            int i = 1;
            if (s == "If"){
                traverseIf(graph, luaNode, newNode);
            } else{
                if (s == "GlobalFunction")
                    renameGlobalFunction(luaNode, newNode);
                if (s == "LocalFunction")
                    renameLocalFunction(luaNode, newNode);
                if (s == "NumericFor")
                    renameNumericFor(luaNode, newNode);
                if (s == "GenericFor")
                    renameGenericFor(luaNode, newNode);
                if (s == "While")
                    renameWhile(luaNode, newNode);
                if (s == "Repeat")
                    renameRepeat(luaNode, newNode);
                QList<osg::ref_ptr<Data::Node> > res;
                while (true){               
                    try {
                        data[i]["tag"].value();
                        QList<osg::ref_ptr<Data::Node> > subNodes = traverseAndFilterAst(graph, data[i], newNode);
                        if (!subNodes.isEmpty() && newNode != NULL) {
                            linkNodes(graph, newNode, subNodes);
                        }
                        res.append(subNodes);
                        i++;
                    } catch (Diluculum::TypeMismatchError e){
                        break;
                    }   
                }
                if (result.isEmpty()) result = res;
            }
            return result;     
        } catch (Diluculum::TypeMismatchError e){
            return result;
        }
    }

    void AstTest::traverseIf(Data::Graph * graph, Diluculum::LuaVariable luaNode,
                                                           osg::ref_ptr<Data::Node> graphNode){
        QString newName = QString::fromStdString("If " + luaNode["data"][3]["text"].value().asString());
        graphNode.get()->setLabelText(newName);
        graphNode.get()->reloadConfig();

        osg::ref_ptr<Data::Node> newNode = graph->addNode("then", nodeType);
        graph->addEdge(NULL, graphNode, newNode, AstTest::edgeType, true);
        QList<osg::ref_ptr<Data::Node> > subNodes = traverseAndFilterAst(graph, luaNode["data"][7], newNode);
        linkNodes(graph, newNode, subNodes);
        osg::ref_ptr<Data::Node> parent = newNode;
        int i = 8;
        while (true){
            try {
                if (luaNode["data"][i]["tag"].value().asString() == "keyword" &&
                        luaNode["data"][i]["text"].value().asString() == "elseif") {
                    newNode = graph->addNode(QString::fromStdString("elseif " + luaNode["data"][i+2]["text"].value().asString()),
                            nodeType);
                    graph->addEdge(NULL, parent, newNode, AstTest::edgeType, true);
                    subNodes = traverseAndFilterAst(graph, luaNode["data"][i+6], newNode);
                    linkNodes(graph, newNode, subNodes);
                    parent = newNode;
                    i += 7;
                    continue;
                }
                if (luaNode["data"][i]["tag"].value().asString() == "keyword" &&
                        luaNode["data"][i]["text"].value().asString() == "else") {
                    newNode = graph->addNode(QString::fromStdString("else"), nodeType);
                    graph->addEdge(NULL, parent, newNode, AstTest::edgeType, true);
                    subNodes = traverseAndFilterAst(graph, luaNode["data"][i+1], newNode);
                    linkNodes(graph, newNode, subNodes);
                    i += 2;
                    continue;
                }
                i++;
            } catch (Diluculum::TypeMismatchError e){
                break;
            }
        }
    }

    void AstTest::renameGlobalFunction(Diluculum::LuaVariable luaNode,
                                                           osg::ref_ptr<Data::Node> graphNode){
        string newName = "Global function: " + luaNode["data"][3]["text"].value().asString();
        newName = newName + "(" + luaNode["data"][5]["data"][3]["text"].value().asString() + ")";
        graphNode.get()->setLabelText(QString::fromStdString(newName));
        graphNode.get()->reloadConfig();
    }

    void AstTest::renameLocalFunction(Diluculum::LuaVariable luaNode,
                                                           osg::ref_ptr<Data::Node> graphNode){
        string newName = "Local function: " + luaNode["data"][5]["text"].value().asString();
        newName = newName + "(" + luaNode["data"][7]["data"][3]["text"].value().asString() + ")";
        graphNode.get()->setLabelText(QString::fromStdString(newName));
        graphNode.get()->reloadConfig();
    }

    void AstTest::renameNumericFor(Diluculum::LuaVariable luaNode,
                                                           osg::ref_ptr<Data::Node> graphNode){
        string newName = "for ";
        newName += luaNode["data"][3]["text"].value().asString();
        newName += " = " + luaNode["data"][7]["text"].value().asString();
        newName += ", " + luaNode["data"][11]["text"].value().asString();
        if (luaNode["data"][15]["tag"].value().asString() == "Exp")
            newName += ", " + luaNode["data"][15]["text"].value().asString();
        graphNode.get()->setLabelText(QString::fromStdString(newName));
        graphNode.get()->reloadConfig();
    }

    void AstTest::renameGenericFor(Diluculum::LuaVariable luaNode,
                                                           osg::ref_ptr<Data::Node> graphNode){
        string newName = "for ";
        int i = 1;
        bool first = true;
        while (true){
            try {
                luaNode["data"][3]["data"][i]["tag"].value();
                if (luaNode["data"][3]["data"][i]["tag"].value().asString() == "Name"){
                    if (first){
                        first = false;
                        newName += luaNode["data"][3]["data"][i]["text"].value().asString();
                    } else newName += ", " + luaNode["data"][3]["data"][i]["text"].value().asString();
                }
                i++;
            } catch (Diluculum::TypeMismatchError e){
                break;
            }
        }
        newName += " in ";
        i = 1;
        first = true;
        while (true){
            try {
                luaNode["data"][7]["data"][i]["tag"].value();
                if (luaNode["data"][7]["data"][i]["tag"].value().asString() == "Exp"){
                    if (first){
                        first = false;
                        newName += luaNode["data"][7]["data"][i]["text"].value().asString();
                    } else newName += ", " + luaNode["data"][7]["data"][i]["text"].value().asString();
                }
                i++;
            } catch (Diluculum::TypeMismatchError e){
                break;
            }
        }
        graphNode.get()->setLabelText(QString::fromStdString(newName));
        graphNode.get()->reloadConfig();
    }

    void AstTest::renameWhile(Diluculum::LuaVariable luaNode, osg::ref_ptr<Data::Node> graphNode){
        string newName = "while " + luaNode["data"][3]["text"].value().asString();
        graphNode.get()->setLabelText(QString::fromStdString(newName));
        graphNode.get()->reloadConfig();
    }

    void AstTest::renameRepeat(Diluculum::LuaVariable luaNode, osg::ref_ptr<Data::Node> graphNode){
        string newName = "repeat until " + luaNode["data"][7]["text"].value().asString();
        graphNode.get()->setLabelText(QString::fromStdString(newName));
        graphNode.get()->reloadConfig();
    }

    osg::ref_ptr<Data::Node> AstTest::addLinkedNode(Data::Graph * graph, string name, osg::ref_ptr<Data::Node> graphNode){
        osg::ref_ptr<Data::Node> newNode = graph->addNode(QString::fromStdString(name), nodeType);
        if (graphNode != NULL) graph->addEdge(NULL, graphNode, newNode, AstTest::edgeType, true);
        return newNode;
    }

    void AstTest::createNodeTypes(Data::Graph * graph){
        Util::ApplicationConfig * appConf = Util::ApplicationConfig::get();
        // STARTPOINT - lime
        graph->addType("STARTPOINT", createSettings(appConf, "0", "1", "0"));
        // If - orange
        graph->addType("If", createSettings(appConf, "1", "0.5", "0"));
        // Assign - blue
        graph->addType("Assign", createSettings(appConf, "0", "0", "1"));
        // LocalAssign - light blue
        graph->addType("LocalAssign", createSettings(appConf, "0.5", "0.5", "1"));
        // FunctionCall - yellow
        graph->addType("FunctionCall", createSettings(appConf, "1", "1", "0"));
        // COMMENT - purple
        graph->addType("COMMENT", createSettings(appConf, "1", "0", "1"));
        // LastStat - cyan
        graph->addType("LastStat", createSettings(appConf, "0", "1", "1"));
        // GlobalFunction - red
        graph->addType("GlobalFunction", createSettings(appConf, "1", "0", "0"));
        // LocalFunction - pink
        graph->addType("LocalFunction", createSettings(appConf, "1", "0.5", "0.5"));
        // NumericFor - green
        graph->addType("NumericFor", createSettings(appConf, "0", "0.5", "0"));
        // GenericFor - olive
        graph->addType("GenericFor", createSettings(appConf, "0.5", "0.5", "0"));
        // Do - brown
        graph->addType("Do", createSettings(appConf, "0.3", "0.2", "0"));
        // While - green
        graph->addType("While", createSettings(appConf, "0.8", "0.5", "0"));
        // Repeat - olive
        graph->addType("Repeat", createSettings(appConf, "0.8", "0.2", "0"));
    }

    QMap<QString, QString> * AstTest::createSettings(Util::ApplicationConfig * appconf, string r, string g, string b){
        QMap<QString, QString> * settings = new QMap<QString, QString>;
        settings->insert("scale", appconf->getValue("Viewer.Textures.DefaultNodeScale"));
        settings->insert("textureFile", appconf->getValue("Viewer.Textures.Node"));
        settings->insert("color.R", QString::fromStdString(r));
        settings->insert("color.G", QString::fromStdString(g));
        settings->insert("color.B", QString::fromStdString(b));
        settings->insert("color.A", "1");
        return settings;
    }

    bool AstTest::isRelevantNode(string s){
        return s == "STARTPOINT" || s == "Assign" || s == "FunctionCall" || s == "Do" || s == "While"
                || s == "Repeat" || s == "If" || s == "NumericFor" || s == "GenericFor"
                || s == "GlobalFunction" || s == "LocalFunction" || s == "LocalAssign"
                || s == "LastStat" || s == "COMMENT";
    }

    void AstTest::linkNodes(Data::Graph * graph, osg::ref_ptr<Data::Node> newNode,
                            QList<osg::ref_ptr<Data::Node> > subNodes){
        if (subNodes.count() == 1)
            graph->addEdge(NULL, newNode, subNodes[0], AstTest::edgeType, true);
        else
            for (int k = 0; k < subNodes.count(); k++){
                ostringstream os;
                os << k;
                osg::ref_ptr<Data::Node> pom = addLinkedNode(graph, os.str(),newNode);
                newNode = pom;
                graph->addEdge(NULL, newNode, subNodes[k], AstTest::edgeType, true);
            }
    }

    void AstTest::visualizeFunctionCall(Data::Graph * graph){
        Importer::GraphOperations * operations = new Importer::GraphOperations(*graph);
        operations->addDefaultTypes(AstTest::edgeType, AstTest::nodeType);

        luaState = new Diluculum::LuaState;
        luaState->doFile("../share/3dsoftviz/scripts/function_call.lua");
        Diluculum::LuaVariable ast = (*luaState)["result"];

        luaState->~LuaState();
    }

}


