#include "MapEditor.h"
#include <vector>
#include <cstring>

using namespace std;

const int UNDO_MAX = 20;

struct UndoData
{
    int FloorMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
    int FloorRot[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

    int WallMapA[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
    int WallMapB[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
    int WallRotA[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
    int WallRotB[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

    int CornerMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
    int CornerRot[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

    int DecoMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
    int DecoRot[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

    int EventMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

    int startX;
    int startY;
    int startZ;

    int goalX;
    int goalY;
    int goalZ;
};

vector<UndoData> undoList;

void PushUndo()
{
    if ((int)undoList.size() >= UNDO_MAX)
    {
        undoList.erase(undoList.begin());
    }

    undoList.emplace_back();
    UndoData& data = undoList.back();

    memcpy(data.FloorMap, ::FloorMap, sizeof(::FloorMap));
    memcpy(data.FloorRot, ::FloorRot, sizeof(::FloorRot));

    memcpy(data.WallMapA, ::WallMapA, sizeof(::WallMapA));
    memcpy(data.WallMapB, ::WallMapB, sizeof(::WallMapB));
    memcpy(data.WallRotA, ::WallRotA, sizeof(::WallRotA));
    memcpy(data.WallRotB, ::WallRotB, sizeof(::WallRotB));

    memcpy(data.CornerMap, ::CornerMap, sizeof(::CornerMap));
    memcpy(data.CornerRot, ::CornerRot, sizeof(::CornerRot));

    memcpy(data.DecoMap, ::DecoMap, sizeof(::DecoMap));
    memcpy(data.DecoRot, ::DecoRot, sizeof(::DecoRot));

    memcpy(data.EventMap, ::EventMap, sizeof(::EventMap));

    data.startX = startX;
    data.startY = startY;
    data.startZ = startZ;

    data.goalX = goalX;
    data.goalY = goalY;
    data.goalZ = goalZ;
}

void UndoMap()
{
    if (undoList.empty())
        return;

    const UndoData& data = undoList.back();

    memcpy(::FloorMap, data.FloorMap, sizeof(::FloorMap));
    memcpy(::FloorRot, data.FloorRot, sizeof(::FloorRot));

    memcpy(::WallMapA, data.WallMapA, sizeof(::WallMapA));
    memcpy(::WallMapB, data.WallMapB, sizeof(::WallMapB));
    memcpy(::WallRotA, data.WallRotA, sizeof(::WallRotA));
    memcpy(::WallRotB, data.WallRotB, sizeof(::WallRotB));

    memcpy(::CornerMap, data.CornerMap, sizeof(::CornerMap));
    memcpy(::CornerRot, data.CornerRot, sizeof(::CornerRot));

    memcpy(::DecoMap, data.DecoMap, sizeof(::DecoMap));
    memcpy(::DecoRot, data.DecoRot, sizeof(::DecoRot));

    memcpy(::EventMap, data.EventMap, sizeof(::EventMap));

    startX = data.startX;
    startY = data.startY;
    startZ = data.startZ;

    goalX = data.goalX;
    goalY = data.goalY;
    goalZ = data.goalZ;

    undoList.pop_back();
}
