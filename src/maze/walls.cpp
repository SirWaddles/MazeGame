#include "walls.h"

#define NULL 0

void Block::SetBlockType(BlockType toSet){
	bType = toSet;
}

BlockType Block::GetBlockType(){
	return bType;
}

void Block::SetAdjacents(Block* top, Block* left, Block* right, Block* bottom){
	nTop = top;
	nLeft = left;
	nRight = right;
	nBottom = bottom;
}

void Block::SetPos(int x, int y){
	bX = x;
	bY = y;
}

int Block::GetX(){
	return bX;
}

int Block::GetY(){
	return bY;
}

Block::Block(){
	bType = BLOCK_WALL;
	enterCell = NULL;
	nTop = NULL;
	nLeft = NULL;
	nRight = NULL;
	nBottom = NULL;
}

Block* Block::GetTop(){
	return nTop;
}

Block* Block::GetLeft(){
	return nLeft;
}

Block* Block::GetRight(){
	return nRight;
}

Block* Block::GetBottom(){
	return nBottom;
}

void Block::SetEnterCell(Block* enterCellSet){
	enterCell = enterCellSet;
}

Block* Block::GetEnterCell(){
	return enterCell;
}