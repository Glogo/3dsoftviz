#ifndef LUAINCIDENCE_H
#define LUAINCIDENCE_H

#include "LuaGraphObject.h"

#include "QPair"
#include "Q_LONG"

namespace Lua {

class LuaIncidence : public LuaGraphObject
{
public:
    void setEdgeNode(const qlonglong edgeId, const qlonglong nodeId);

    QPair<qlonglong, qlonglong> getEdgeNodePair() const;

    bool getOriented() const;
    void setOriented(bool value);

    bool getOutGoing() const;
    void setOutGoing(bool value);

private:
    QPair<qlonglong, qlonglong> edgeNodePair;
    bool oriented;
    bool outGoing;
};

}

#endif // LUAINCIDENCE_H