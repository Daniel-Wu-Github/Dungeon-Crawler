#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"

using std::cout, std::endl, std::ifstream, std::string;

/**
 * TODO: Student implement this function
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {
    std::ifstream currentLevel(fileName);
    if(!currentLevel.is_open() || !(currentLevel >> maxRow >> maxCol)){
        return nullptr;
    }
    if (maxRow < 1 || maxCol < 1 || maxRow * static_cast<long long>(maxCol) > INT32_MAX) {
        return nullptr;
    }
    char** finalMap = createMap(maxRow, maxCol);
    if(!finalMap){
        return nullptr;
    }

    int pSpawnRow, pSpawnCol;
    if(!(currentLevel >> pSpawnRow >> pSpawnCol) || pSpawnRow >= maxRow || pSpawnCol >= maxCol || pSpawnRow < 0 || pSpawnCol < 0){
        deleteMap(finalMap, maxRow);
        return nullptr;
    }
    player.row = pSpawnRow;
    player.col = pSpawnCol;
    char tile;
    int tileCount = 0;
    bool hasExit = false;
    for(int r = 0; r < maxRow; r++){
        for(int c = 0; c < maxCol; c++){
            if(!(currentLevel>>tile)){
                deleteMap(finalMap, maxRow);
                return nullptr;
            }
            if(tile != TILE_OPEN && tile != TILE_PLAYER && tile != TILE_TREASURE && tile != TILE_AMULET && tile != TILE_MONSTER && tile != TILE_PILLAR && tile != TILE_DOOR && tile != TILE_EXIT){
                deleteMap(finalMap, maxRow);
                return nullptr;
            }
            if(tile == TILE_DOOR || tile == TILE_EXIT){
                hasExit = true;
            }
            finalMap[r][c] = tile;
            tileCount++;
        }
    }
    if(tileCount != maxRow * maxCol || !hasExit){
        deleteMap(finalMap, maxRow);
        return nullptr;
    }
    char extra;
    if(currentLevel>>extra || finalMap[pSpawnRow][pSpawnCol]!=TILE_OPEN){
        deleteMap(finalMap, maxRow);
        return nullptr;
    }
    finalMap[pSpawnRow][pSpawnCol] = TILE_PLAYER;
    return finalMap;
}

/**
 * TODO: Student implement this function
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {
    if(input == MOVE_DOWN || input == MOVE_LEFT || input == MOVE_RIGHT || input == MOVE_UP){
        switch (input){
            case MOVE_DOWN:
                nextRow++;
                break;
            case MOVE_LEFT:
                nextCol--;
                break;
            case MOVE_RIGHT:
                nextCol++;
                break;
            case MOVE_UP:
                nextRow--;
                break;
        }
    }
}

/**
 * TODO: [suggested] Student implement this function
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {
    char** currentMap = new char*[maxRow];
    for(int r = 0; r < maxRow; r++){
        currentMap[r] = new char[maxCol];
    }
    for(int r = 0; r < maxRow; r++){
        for(int c = 0; c < maxCol; c++){
            currentMap[r][c] = TILE_OPEN;
        }
    }
    return currentMap;
}

/**
 * TODO: Student implement this function
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {
    if(map) {
        for(int i = 0; i < maxRow; i++) {
            delete[] map[i];
        }
        delete[] map;
    }  
    map = nullptr;
    maxRow = 0;
}

/**
 * TODO: Student implement this function
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {

    if(map == nullptr || maxRow < 1 || maxCol < 1){
        return nullptr;
    }

    char** doubleMap = new char*[maxRow*2];
    for(int i = 0; i < maxRow*2; ++i){
        doubleMap[i] = new char[maxCol*2];
    }
    
    for(int r = 0; r<maxRow; r++){
        for(int c =0; c<maxCol; c++){
            doubleMap[r][c] = map[r][c];
        }
    }
    maxRow*=2;
    maxCol*=2;
    
    

    for(int r = maxRow/2; r<maxRow; r++){ //bottom right quadrant
        for(int c =maxCol/2; c<maxCol; c++){
            if((map[r-maxRow/2][c-maxCol/2])==TILE_PLAYER){
                doubleMap[r][c] = TILE_OPEN;
            }
            else{
                doubleMap[r][c] = map[r-maxRow/2][c-maxCol/2];
            }
        }
    }
    
    for(int r = maxRow/2; r<maxRow; r++){ //bottom left quadrant
        for(int c = 0; c<maxCol/2; c++){
            if((map[r-maxRow/2][c])==TILE_PLAYER){
                doubleMap[r][c] = TILE_OPEN;
            }   
            else{
                doubleMap[r][c] = map[r-maxRow/2][c];
            }
        }
    }

    for(int r = 0; r<maxRow/2; r++){ //top right quadrant
        for(int c = maxCol/2; c<maxCol; c++){
            if((map[r][c-maxCol/2])==TILE_PLAYER){
                doubleMap[r][c] = TILE_OPEN;
            }
            else{
                doubleMap[r][c] = map[r][c-maxCol/2];
            }
        }
    }
    for(int i = 0; i < maxRow/2; ++i){
        delete[] map[i];
    }
    delete[] map;
    return doubleMap;
}

/**
 * TODO: Student implement this function
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {
    if(nextRow>=maxRow || nextRow<0 ){
        return STATUS_STAY;
    }
    if(nextCol >= maxCol || nextCol < 0){
        return STATUS_STAY;
    }
    if(map[nextRow][nextCol] == TILE_PILLAR){
        return STATUS_STAY;
    }
    if(map[nextRow][nextCol] == TILE_MONSTER){
        return STATUS_STAY;
    }
    if(map[nextRow][nextCol] == TILE_EXIT && player.treasure == 0){
        return STATUS_STAY;
    }
    if(map[nextRow][nextCol] == TILE_OPEN){
    map[player.row][player.col] = TILE_OPEN;
    map[nextRow][nextCol] = TILE_PLAYER;
    player.row = nextRow;
    player.col = nextCol;
    return STATUS_MOVE;
    }
    if(map[nextRow][nextCol] == TILE_TREASURE){
        map[player.row][player.col] = TILE_OPEN;
        map[nextRow][nextCol] = TILE_PLAYER;
        player.treasure++;
        player.row = nextRow;
        player.col = nextCol;
        return STATUS_TREASURE;
    }
    if(map[nextRow][nextCol] == TILE_AMULET){
        map[player.row][player.col] = TILE_OPEN;
        map[nextRow][nextCol] = TILE_PLAYER;
        player.row = nextRow;
        player.col = nextCol;
        return STATUS_AMULET;
    }
    if(map[nextRow][nextCol] == TILE_DOOR){
        map[player.row][player.col] = TILE_OPEN;
        map[nextRow][nextCol] = TILE_PLAYER;
        player.row = nextRow;
        player.col = nextCol;
        return STATUS_LEAVE;
    }
    if(map[nextRow][nextCol] == TILE_EXIT && player.treasure > 0){
        map[player.row][player.col] = TILE_OPEN;
        map[nextRow][nextCol] = TILE_PLAYER;
        player.row = nextRow;
        player.col = nextCol;
        return STATUS_ESCAPE;
    }
    return STATUS_STAY;
}

/**
 * TODO: Student implement this function
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {
    //check up
    for(int i = player.row-1; i>=0; i--){
        if(map[i][player.col] == TILE_PILLAR){
            break;
        }
        if(map[i][player.col] == TILE_MONSTER){
            map[i][player.col] = TILE_OPEN;
            map[i+1][player.col] = TILE_MONSTER;
            continue;
        }
    }
    // Check down
    for(int i = player.row+1; i < maxRow; i++){
        if(map[i][player.col] == TILE_PILLAR){
            break;
        }
        if(map[i][player.col] == TILE_MONSTER){
            map[i][player.col] = TILE_OPEN;
            map[i-1][player.col] = TILE_MONSTER;
            continue;
        }
    }
    // Check left
    for(int j = player.col-1; j >= 0; j--){
        if(map[player.row][j] == TILE_PILLAR){
            break;
        }
        if(map[player.row][j] == TILE_MONSTER){
            map[player.row][j] = TILE_OPEN;
            map[player.row][j+1] = TILE_MONSTER;
            continue;
        }
    }
    // Check right
    for(int j = player.col+1; j < maxCol; j++){
        if(map[player.row][j] == TILE_PILLAR){
            break;
        }
        if(map[player.row][j] == TILE_MONSTER){
            map[player.row][j] = TILE_OPEN;
            map[player.row][j-1] = TILE_MONSTER;
            continue;
        }
    }
    if(map[player.row][player.col]==TILE_MONSTER){
        return true;
    }
    return false;
}