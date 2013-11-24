#include <QApplication>

#include "Manager/Manager.h"
#include "Core/Core.h"
#include "Util/Cleaner.h"

// lua development
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

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

static Diluculum::LuaState* luaState;

void traverseAst(Diluculum::LuaVariable node, string prefix = ""){
    try {
        string tag = node["tag"].value().asString();
        cout << prefix << tag << endl;
    } catch (Diluculum::TypeMismatchError e){
        return;
    }
    Diluculum::LuaVariable data = node["data"];
    int i = 1;
    while (true){
        try {
            data[i]["tag"].value();
            traverseAst(data[i], prefix + "  ");
            i++;
        } catch (Diluculum::TypeMismatchError e){
            break;
        }
    }

}

int main(int argc, char *argv[])
{
    luaState = new Diluculum::LuaState;
    luaState->doFile("../share/3dsoftviz/scripts/main.lua");
    Diluculum::LuaVariable ast = (*luaState)["ast"];
//    traverseAst(ast);



    luaState->~LuaState();

    QApplication app(argc, argv);
    new Cleaner(&app);
    AppCore::Core::getInstance(&app);
    Manager::GraphManager::getInstance();
}
