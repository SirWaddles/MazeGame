#ifndef _WALL_HEADER_
#define _WALL_HEADER_

#ifndef _M_SERVER_
#include "graphics/graphics.h"
#endif

enum BlockType {
	BLOCK_WALL = 1,
	BLOCK_CELL,
	BLOCK_MAZE,
	BLOCK_PASSAGE,
	BLOCK_FINISH
};

class Block {
public:
	void SetBlockType(BlockType toSet);
	BlockType GetBlockType();

	void SetPos(int x, int y);
	int GetX();
	int GetY();
	
	void SetAdjacents(Block* top, Block* left, Block* right, Block* bottom);

	Block* GetTop();
	Block* GetLeft();
	Block* GetRight();
	Block* GetBottom();

	void SetEnterCell(Block* enterCellSet);
	Block* GetEnterCell();

	Block();
protected:
	BlockType bType;

	Block* nTop;
	Block* nLeft;
	Block* nRight;
	Block* nBottom;

	Block* enterCell;

	int bX;
	int bY;
private:

};

// Yea, only the one maze at any one time.

namespace Maze {

#ifndef _M_SERVER_
	struct MazeGeometry{
		GraphicsM::Model* MazeModel;
		GraphicsM::Model* FloorModel;
		GraphicsM::Model* CeilingModel;
	};

	MazeGeometry CreateGeometry();
#endif

	void GenerateMaze(int w, int h, unsigned int seed = 0);
	Block* GetBlock(int x, int y);
	Block* GetOppositeBlock(Block* cBlock, Block* wBlock);
	
	int GetMazeWidth();
	int GetMazeHeight();
}


#endif // _WALL_HEADER_