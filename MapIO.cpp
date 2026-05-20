#include "MapEditor.h"
#include <fstream>
#include <string>
#include <vector>

using namespace std;

#pragma region  共通 

static vector<string> SplitCSV(const string& line)
{
    vector<string> result;
    string item;

    for (char c : line)
    {
        if (c == ',')
        {
            result.push_back(item);
            item.clear();
        }
        else
        {
            item += c;
        }
    }

    result.push_back(item);
    return result;
}

static bool IsSaveLoadPosValid(int y, int z, int x)
{
    return
        y >= 0 && y < BLOCK_NUM_Y &&
        z >= 0 && z < BLOCK_NUM_Z &&
        x >= 0 && x < BLOCK_NUM_X;
}

static void ClearEventIdForSave(int eventId)
{
    // 2026-05-13: START/GOAL保存時に古いイベントマーカーが複数残らないよう追加。
    for (int y = 0; y < BLOCK_NUM_Y; y++)
        for (int z = 0; z < BLOCK_NUM_Z; z++)
            for (int x = 0; x < BLOCK_NUM_X; x++)
                if (EventMap[y][z][x] == eventId)
                    EventMap[y][z][x] = -1;
}

static void SyncEventMarkersFromStartGoal()
{
    // 2026-05-13: START/GOAL座標と[EVENT]保存内容がズレないよう、保存直前に同期するため追加。
    if (IsSaveLoadPosValid(startY, startZ, startX))
    {
        ClearEventIdForSave(0);
        EventMap[startY][startZ][startX] = 0;
    }

    if (IsSaveLoadPosValid(goalY, goalZ, goalX))
    {
        ClearEventIdForSave(1);
        EventMap[goalY][goalZ][goalX] = 1;
    }
}
#pragma endregion


#pragma region  保存 

void SaveMap()
{
    SyncEventMarkersFromStartGoal();

    char fileName[64];
    sprintf_s(fileName, "map%d.csv", currentMapIndex);

    ofstream ofs(fileName);

    if (!ofs)
        return;

    ofs << "#MAP3D\n";
    ofs << "SIZE," << BLOCK_NUM_X << "," << BLOCK_NUM_Y << "," << BLOCK_NUM_Z << "\n";
    ofs << "START," << startX << "," << startY << "," << startZ << "\n";
    ofs << "GOAL," << goalX << "," << goalY << "," << goalZ << "\n";

    ofs << "\n[FLOOR]\n";
    ofs << "y,z,x,id,rot\n";
    for (int y = 0; y < BLOCK_NUM_Y; y++)
        for (int z = 0; z < BLOCK_NUM_Z; z++)
            for (int x = 0; x < BLOCK_NUM_X; x++)
                if (FloorMap[y][z][x] >= 0)
                    ofs << y << "," << z << "," << x << "," << FloorMap[y][z][x] << "," << FloorRot[y][z][x] << "\n";

    ofs << "\n[WALL_A]\n";
    ofs << "y,z,x,id,rot\n";
    for (int y = 0; y < BLOCK_NUM_Y; y++)
        for (int z = 0; z < BLOCK_NUM_Z; z++)
            for (int x = 0; x < BLOCK_NUM_X; x++)
                if (WallMapA[y][z][x] >= 0)
                    ofs << y << "," << z << "," << x << "," << WallMapA[y][z][x] << "," << WallRotA[y][z][x] << "\n";

    ofs << "\n[WALL_B]\n";
    ofs << "y,z,x,id,rot\n";
    for (int y = 0; y < BLOCK_NUM_Y; y++)
        for (int z = 0; z < BLOCK_NUM_Z; z++)
            for (int x = 0; x < BLOCK_NUM_X; x++)
                if (WallMapB[y][z][x] >= 0)
                    ofs << y << "," << z << "," << x << "," << WallMapB[y][z][x] << "," << WallRotB[y][z][x] << "\n";
    // 2026-05-13: コーナー当たり判定の長さ/厚み/奥行も保存するためCSV列を追加。
    ofs << "\n[CORNER]\n";
    ofs << "y,z,x,id,rot,scale,thickness,offset\n";

    for (int y = 0; y < BLOCK_NUM_Y; y++)
    {
        for (int z = 0; z < BLOCK_NUM_Z; z++)
        {
            for (int x = 0; x < BLOCK_NUM_X; x++)
            {
                if (CornerMap[y][z][x] >= 0)
                {
                    ofs << y << "," << z << "," << x << ","
                        << CornerMap[y][z][x] << ","
                        << CornerRot[y][z][x] << ","
                        << CollisionCornerScaleMap[y][z][x] << ","
                        << CollisionCornerThicknessMap[y][z][x] << ","
                        << CollisionCornerOffsetMap[y][z][x]
                        << "\n";
                }
            }
        }
    }
    ofs << "\n[DECO]\n";
    ofs << "y,z,x,id,rot\n";
    for (int y = 0; y < BLOCK_NUM_Y; y++)
        for (int z = 0; z < BLOCK_NUM_Z; z++)
            for (int x = 0; x < BLOCK_NUM_X; x++)
                if (DecoMap[y][z][x] >= 0)
                    ofs << y << "," << z << "," << x << "," << DecoMap[y][z][x] << "," << DecoRot[y][z][x] << "\n";

    ofs << "\n[COLLISION]\n";
    ofs << "y,z,x,block\n";
    // 2026-05-11: エディターで置いた手動当たり判定をLoaderでも再現するため保存を追加。
    for (int y = 0; y < BLOCK_NUM_Y; y++)
        for (int z = 0; z < BLOCK_NUM_Z; z++)
            for (int x = 0; x < BLOCK_NUM_X; x++)
                if (CollisionMap[y][z][x] >= 0)
                    ofs << y << "," << z << "," << x << "," << CollisionMap[y][z][x] << "\n";
    ofs << "\n[COLLISION_EDGE]\n";
    ofs << "y,z,x,mask,s0,s1,s2,s3,t0,t1,t2,t3\n";
    // 2026-05-11: Loaderの壁ライン判定と同じ辺単位の手動当たり判定を保存するため追加。
    for (int y = 0; y < BLOCK_NUM_Y; y++)
        for (int z = 0; z < BLOCK_NUM_Z; z++)
            for (int x = 0; x < BLOCK_NUM_X; x++)
                if (CollisionEdgeMap[y][z][x] >= 0)
                    ofs << y << "," << z << "," << x << "," << CollisionEdgeMap[y][z][x] << "," << CollisionEdgeScaleMap[y][z][x][0] << "," << CollisionEdgeScaleMap[y][z][x][1] << "," << CollisionEdgeScaleMap[y][z][x][2] << "," << CollisionEdgeScaleMap[y][z][x][3] << "," << CollisionEdgeThicknessMap[y][z][x][0] << "," << CollisionEdgeThicknessMap[y][z][x][1] << "," << CollisionEdgeThicknessMap[y][z][x][2] << "," << CollisionEdgeThicknessMap[y][z][x][3] << "\n";
    ofs << "\n[EVENT]\n";
    ofs << "y,z,x,id\n";
    for (int y = 0; y < BLOCK_NUM_Y; y++)
        for (int z = 0; z < BLOCK_NUM_Z; z++)
            for (int x = 0; x < BLOCK_NUM_X; x++)
                if (EventMap[y][z][x] >= 0)
                    ofs << y << "," << z << "," << x << "," << EventMap[y][z][x] << "\n";

    ofs.close();
}

#pragma endregion


#pragma region  読込 

void LoadMap(int mapIndex)
{
    currentMapIndex = mapIndex;

    ResetAllMap();

    SyncEventMarkersFromStartGoal();

    char fileName[64];
    sprintf_s(fileName, "map%d.csv", currentMapIndex);

    ifstream ifs(fileName);

    if (!ifs)
        return;

    string section;
    string line;

    while (getline(ifs, line))
    {
        if (line.empty())
            continue;

        if (line[0] == '#')
            continue;

        if (line[0] == '[')
        {
            section = line;
            continue;
        }

        vector<string> cols = SplitCSV(line);

        if (cols.empty())
            continue;

        if (cols[0] == "SIZE")
            continue;

        if (cols[0] == "START" && cols.size() >= 4)
        {
            startX = stoi(cols[1]);
            startY = stoi(cols[2]);
            startZ = stoi(cols[3]);
            continue;
        }

        if (cols[0] == "GOAL" && cols.size() >= 4)
        {
            goalX = stoi(cols[1]);
            goalY = stoi(cols[2]);
            goalZ = stoi(cols[3]);
            continue;
        }

        if (cols[0] == "y")
            continue;

        if (section == "[FLOOR]" && cols.size() >= 5)
        {
            int y = stoi(cols[0]);
            int z = stoi(cols[1]);
            int x = stoi(cols[2]);

            if (IsSaveLoadPosValid(y, z, x))
            {
                FloorMap[y][z][x] = stoi(cols[3]);
                FloorRot[y][z][x] = stoi(cols[4]);
            }
        }
        else if (section == "[WALL_A]" && cols.size() >= 5)
        {
            int y = stoi(cols[0]);
            int z = stoi(cols[1]);
            int x = stoi(cols[2]);

            if (IsSaveLoadPosValid(y, z, x))
            {
                WallMapA[y][z][x] = stoi(cols[3]);
                WallRotA[y][z][x] = stoi(cols[4]);
            }
        }
        else if (section == "[WALL_B]" && cols.size() >= 5)
        {
            int y = stoi(cols[0]);
            int z = stoi(cols[1]);
            int x = stoi(cols[2]);

            if (IsSaveLoadPosValid(y, z, x))
            {
                WallMapB[y][z][x] = stoi(cols[3]);
                WallRotB[y][z][x] = stoi(cols[4]);
            }
        }
        else if (section == "[CORNER]" && cols.size() >= 5)
        {
            int y = stoi(cols[0]);
            int z = stoi(cols[1]);
            int x = stoi(cols[2]);

            if (IsSaveLoadPosValid(y, z, x))
            {
                CornerMap[y][z][x] = stoi(cols[3]);
                CornerRot[y][z][x] = stoi(cols[4]);

                // 2026-05-13: コーナー当たり判定の長さ/厚みをCSVから復元するため追加。
                CollisionCornerScaleMap[y][z][x] =
                    cols.size() > 5 ? stoi(cols[5]) : 100;

                CollisionCornerThicknessMap[y][z][x] =
                    cols.size() > 6 ? stoi(cols[6]) : 100;

                // 2026-05-13: コーナー当たり判定の奥行オフセットもCSVから復元するため追加。
                CollisionCornerOffsetMap[y][z][x] =
                    cols.size() > 7 ? stoi(cols[7]) : 0;
            }
        }
        else if (section == "[DECO]" && cols.size() >= 5)
        {
            int y = stoi(cols[0]);
            int z = stoi(cols[1]);
            int x = stoi(cols[2]);

            if (IsSaveLoadPosValid(y, z, x))
            {
                DecoMap[y][z][x] = stoi(cols[3]);
                DecoRot[y][z][x] = stoi(cols[4]);
            }
        }
        else if (section == "[COLLISION]" && cols.size() >= 4)
        {
            int y = stoi(cols[0]);
            int z = stoi(cols[1]);
            int x = stoi(cols[2]);

            // 2026-05-13: 保存済みのセル当たり判定をエディターへ復元するため追加。
            if (IsSaveLoadPosValid(y, z, x))
            {
                CollisionMap[y][z][x] = stoi(cols[3]);
            }
        }
        else if (section == "[COLLISION_EDGE]" && cols.size() >= 4)
        {
            int y = stoi(cols[0]);
            int z = stoi(cols[1]);
            int x = stoi(cols[2]);

            if (IsSaveLoadPosValid(y, z, x))
            {
                CollisionEdgeMap[y][z][x] = stoi(cols[3]);

                for (int edge = 0; edge < 4; edge++)
                {
                    CollisionEdgeScaleMap[y][z][x][edge] =
                        cols.size() > (4 + edge) ? stoi(cols[4 + edge]) : 100;

                    CollisionEdgeThicknessMap[y][z][x][edge] =
                        cols.size() > (8 + edge) ? stoi(cols[8 + edge]) : 100;
                }
            }
        }
        else if (section == "[EVENT]" && cols.size() >= 4)
        {
            int y = stoi(cols[0]);
            int z = stoi(cols[1]);
            int x = stoi(cols[2]);

            if (IsSaveLoadPosValid(y, z, x))
            {
                EventMap[y][z][x] = stoi(cols[3]);
            }
        }
    }

    ifs.close();

    // 2026-05-13: 古い[EVENT]行よりSTART/GOAL行を優先して復元するため、読込後に再同期。
    SyncEventMarkersFromStartGoal();
}

#pragma endregion
