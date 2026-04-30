#include "MapEditor.h"
#include <fstream>
#include <string>
#include <vector>

using namespace std;

#pragma region  ‹¤’Ê 

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

#pragma endregion


#pragma region  •Û‘¶ 

void SaveMap()
{
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

    ofs << "\n[CORNER]\n";
    ofs << "y,z,x,id,rot\n";
    for (int y = 0; y < BLOCK_NUM_Y; y++)
        for (int z = 0; z < BLOCK_NUM_Z; z++)
            for (int x = 0; x < BLOCK_NUM_X; x++)
                if (CornerMap[y][z][x] >= 0)
                    ofs << y << "," << z << "," << x << "," << CornerMap[y][z][x] << "," << CornerRot[y][z][x] << "\n";

    ofs << "\n[DECO]\n";
    ofs << "y,z,x,id,rot\n";
    for (int y = 0; y < BLOCK_NUM_Y; y++)
        for (int z = 0; z < BLOCK_NUM_Z; z++)
            for (int x = 0; x < BLOCK_NUM_X; x++)
                if (DecoMap[y][z][x] >= 0)
                    ofs << y << "," << z << "," << x << "," << DecoMap[y][z][x] << "," << DecoRot[y][z][x] << "\n";

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


#pragma region  “Çž 

void LoadMap(int mapIndex)
{
    currentMapIndex = mapIndex;

    ResetAllMap();

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
}

#pragma endregion
