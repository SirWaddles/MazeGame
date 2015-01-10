#include "walls.h"
#ifndef _M_SERVER_
#include "graphics/graphics.h"
#endif
#include <time.h>
#include <vector>

using namespace std;
#ifndef _M_SERVER_
using namespace GraphicsM;
#endif

vector<Block*> blockList;
int mazeWidth = 0;
int mazeHeight = 0;

#ifndef _M_SERVER_
Maze::MazeGeometry Maze::CreateGeometry(){
	Model* mModel = new Model();
	Model* fmModel = new Model();
	Model* cmModel = new Model();

	//Preset normals
	Vec3 Up(0.0f, -1.0f, 0.0f);
	Vec3 Down(0.0f, 1.0f, 0.0f);
	Vec3 Left(-1.0f, 0.0f, 0.0f);
	Vec3 Right(1.0f, 0.0f, 0.0f);

	Vec3 Floor(0.0f, 0.0f, 1.0f);
	Vec3 Ceiling(0.0f, 0.0f, -1.0f);
	
	vector<Block*>::iterator it;
	for(it=blockList.begin(); it<blockList.end(); it++){
		Block* nBlock = (*it);
		float x = float(nBlock->GetX());
		float y = float(nBlock->GetY());

		if(nBlock->GetBlockType()==BLOCK_WALL){

			Block* leftBlock = Maze::GetBlock(x-1, y);
			if(leftBlock && leftBlock->GetBlockType() != BLOCK_WALL){
				mModel->AddVertex(Vec3(x, y, 0.0f), Vec2(0.0f, 0.0f), Left);
				mModel->AddVertex(Vec3(x, y, 1.0f), Vec2(0.0f, 1.0f), Left);
				mModel->AddVertex(Vec3(x, y+1.0f, 1.0f), Vec2(1.0f, 1.0f), Left);

				mModel->AddVertex(Vec3(x, y, 0.0f), Vec2(0.0f, 0.0f), Left);
				mModel->AddVertex(Vec3(x, y+1.0f, 1.0f), Vec2(1.0f, 1.0f), Left);
				mModel->AddVertex(Vec3(x, y+1.0f, 0.0f), Vec2(1.0f, 0.0f), Left);
			}

			Block* upBlock = Maze::GetBlock(x, y-1);
			if(upBlock && upBlock->GetBlockType() != BLOCK_WALL){
				mModel->AddVertex(Vec3(x, y, 0.0f), Vec2(0.0f, 0.0f), Up);
				mModel->AddVertex(Vec3(x+1.0f, y, 0.0f), Vec2(1.0f, 0.0f), Up);
				mModel->AddVertex(Vec3(x+1.0f, y, 1.0f), Vec2(1.0f, 1.0f), Up);
		
				mModel->AddVertex(Vec3(x, y, 0.0f), Vec2(0.0f, 0.0f), Up);
				mModel->AddVertex(Vec3(x+1.0f, y, 1.0f), Vec2(1.0f, 1.0f), Up);
				mModel->AddVertex(Vec3(x, y, 1.0f), Vec2(0.0f, 1.0f), Up);
			}

			Block* rightBlock = Maze::GetBlock(x+1, y);
			if(rightBlock && rightBlock->GetBlockType() != BLOCK_WALL){
				mModel->AddVertex(Vec3(x+1.0f, y, 0.0f), Vec2(0.0f, 0.0f), Right);
				mModel->AddVertex(Vec3(x+1.0f, y+1.0f, 0.0f), Vec2(1.0f, 0.0f), Right);
				mModel->AddVertex(Vec3(x+1.0f, y+1.0f, 1.0f), Vec2(1.0f, 1.0f), Right);

				mModel->AddVertex(Vec3(x+1.0f, y, 0.0f), Vec2(0.0f, 0.0f), Right);
				mModel->AddVertex(Vec3(x+1.0f, y+1.0f, 1.0f), Vec2(1.0f, 1.0f), Right);
				mModel->AddVertex(Vec3(x+1.0f, y, 1.0f), Vec2(0.0f, 1.0f), Right);
			}

			Block* downBlock = Maze::GetBlock(x, y+1);
			if(downBlock && downBlock->GetBlockType() != BLOCK_WALL){
				mModel->AddVertex(Vec3(x, y+1.0f, 0.0f), Vec2(0.0f, 0.0f), Down);
				mModel->AddVertex(Vec3(x, y+1.0f, 1.0f), Vec2(0.0f, 1.0f), Down);
				mModel->AddVertex(Vec3(x+1.0f, y+1.0f, 1.0f), Vec2(1.0f, 1.0f), Down);

				mModel->AddVertex(Vec3(x, y+1.0f, 0.0f), Vec2(0.0f, 0.0f), Down);
				mModel->AddVertex(Vec3(x+1.0f, y+1.0f, 1.0f), Vec2(1.0f, 1.0f), Down);
				mModel->AddVertex(Vec3(x+1.0f, y+1.0f, 0.0f), Vec2(1.0f, 0.0f), Down);
			}
		} else {
			fmModel->AddVertex(Vec3(x, y, 0.0f), Vec2(0.0f, 0.0f), Floor);
			fmModel->AddVertex(Vec3(x+1.0f, y, 0.0f), Vec2(1.0f, 0.0f), Floor);
			fmModel->AddVertex(Vec3(x+1.0f, y+1.0f, 0.0f), Vec2(1.0f, 1.0f), Floor);

			fmModel->AddVertex(Vec3(x, y, 0.0f), Vec2(0.0f, 0.0f), Floor);
			fmModel->AddVertex(Vec3(x+1.0f, y+1.0f, 0.0f), Vec2(1.0f, 1.0f), Floor);
			fmModel->AddVertex(Vec3(x, y+1.0f, 0.0f), Vec2(0.0f, 1.0f), Floor);

			cmModel->AddVertex(Vec3(x, y, 1.0f), Vec2(0.0f, 0.0f), Ceiling);
			cmModel->AddVertex(Vec3(x+1.0f, y+1.0f, 1.0f), Vec2(1.0f, 1.0f), Ceiling);
			cmModel->AddVertex(Vec3(x+1.0f, y, 1.0f), Vec2(1.0f, 0.0f), Ceiling);
			
			cmModel->AddVertex(Vec3(x, y, 1.0f), Vec2(0.0f, 0.0f), Ceiling);
			cmModel->AddVertex(Vec3(x, y+1.0f, 1.0f), Vec2(0.0f, 1.0f), Ceiling);
			cmModel->AddVertex(Vec3(x+1.0f, y+1.0f, 1.0f), Vec2(1.0f, 1.0f), Ceiling);
		}
	}

	mModel->Compile();
	fmModel->Compile();
	cmModel->Compile();
	Maze::MazeGeometry retval;
	retval.FloorModel = fmModel;
	retval.MazeModel = mModel;
	retval.CeilingModel = cmModel;
	return retval;
}
#endif

void Maze::GenerateMaze(int w, int h, unsigned int seed){
	blockList.clear();
	mazeWidth = w;
	mazeHeight = h;
	for(int i=0; i < (w*h); i++){
		Block* newBlock = new Block();
		blockList.push_back(newBlock);
		
		int x = i % w;
		int y = floor(float(i-x)/float(w));
		//printf("Cell x: %i y: %i\n", x, y);
		newBlock->SetPos(x, y);
		
		if(x==0 || x==w-1){
			newBlock->SetBlockType(BLOCK_WALL);
			//DrawPixel(x, y, 0);
			continue;
		}
		if(y==0 || y==h-1){
			newBlock->SetBlockType(BLOCK_WALL);
			//DrawPixel(x, y, 0);
			continue;
		}

		float divx = float(x)/2.0;
		float divy = float(y)/2.0;
		if(floor(divx)!=divx && floor(divy) != divy){
			newBlock->SetBlockType(BLOCK_CELL);
			//DrawPixel(x, y, 255);
			//printf("Cell @ %i %i\n", x, y);
			continue;
		}
	}
	
	vector<Block*>::iterator it;
	for(it=blockList.begin(); it<blockList.end(); it++){
		Block* curBlock = (*it);
		if(curBlock->GetBlockType() != BLOCK_CELL){
			continue;
		}
		int x = curBlock->GetX();
		int y = curBlock->GetY();
		Block* nTop = Maze::GetBlock(x, y-1);
		Block* nLeft = Maze::GetBlock(x-1, y);
		Block* nRight = Maze::GetBlock(x+1, y);
		Block* nBottom = Maze::GetBlock(x, y+1);
		curBlock->SetAdjacents(nTop, nLeft, nRight, nBottom);
	}
	
	Block* startBlock = Maze::GetBlock(1, 1);
	//startBlock->SetBlockType(BLOCK_MAZE);
	vector<Block*> WallList;
	if(startBlock->GetBlockType()==BLOCK_CELL){
		//printf("This really should be a cell.\n");
		startBlock->SetBlockType(BLOCK_MAZE);
	} 

	startBlock->GetTop()->SetEnterCell(startBlock);
	startBlock->GetLeft()->SetEnterCell(startBlock);
	startBlock->GetRight()->SetEnterCell(startBlock);
	startBlock->GetBottom()->SetEnterCell(startBlock);

	WallList.push_back(startBlock->GetTop());
	WallList.push_back(startBlock->GetLeft());
	WallList.push_back(startBlock->GetRight());
	WallList.push_back(startBlock->GetBottom());

	/*if(seed){
		srand(seed);
	} else {
		srand(time(NULL));
	}*/

	int times = 0;
	while(WallList.size()>0){
		times++;
		//if(times > 100){
		//	break;
		//}
		//printf("Size: %i\n", WallList.size());
		float nRand = float(rand())/float(RAND_MAX);
		//printf("-1\n");
		int nDex = floor(float(WallList.size()-1)*nRand);
		//printf("nDex: %i\n", nDex);
		Block* tBlock = WallList[nDex];
		Block* nStart = tBlock->GetEnterCell();
		//printf("0\n");
		if(!nStart){
			nStart = startBlock;
		}
		if(nStart->GetBlockType()==BLOCK_MAZE){
			//printf("goooood\n");
		}
		//printf("0.2\n");
		tBlock->SetEnterCell(nStart);
		//printf("0.4\n");
		Block* oBlock = Maze::GetOppositeBlock(nStart, tBlock);
		//printf("1\n");
		//printf("UMMM...\n");
		if(!oBlock){
			//printf("NOPE\n");
			WallList.erase(WallList.begin()+nDex);
			continue;
		}
		if(oBlock->GetBlockType()==BLOCK_WALL){
			//printf("WATWALLNOOOO\n");
			WallList.erase(WallList.begin()+nDex);
			continue;
		}
		//printf("2\n");
		if(oBlock->GetBlockType()==BLOCK_MAZE){
			//printf("MAZE\n");
			WallList.erase(WallList.begin()+nDex);
			continue;
		}
		//printf("3\n");
		if(oBlock->GetBlockType()==BLOCK_CELL){
			//printf("CELL\n");
			oBlock->SetBlockType(BLOCK_MAZE);
			tBlock->SetBlockType(BLOCK_PASSAGE);
			oBlock->GetTop()->SetEnterCell(oBlock);
			WallList.push_back(oBlock->GetTop());
			oBlock->GetLeft()->SetEnterCell(oBlock);
			WallList.push_back(oBlock->GetLeft());
			oBlock->GetRight()->SetEnterCell(oBlock);
			WallList.push_back(oBlock->GetRight());
			oBlock->GetBottom()->SetEnterCell(oBlock);
			WallList.push_back(oBlock->GetBottom());
		}
	}
	//Maze::GetBlock(mazeWidth-1, mazeHeight-2)->SetBlockType(BLOCK_FINISH);
	printf("Maze generated in %i loops.\n", times);
	//Hopefully no infinite loop, bound to be some debugging though.
}



Block* Maze::GetBlock(int x, int y){
	if (mazeHeight <= 0){
		return NULL;
	}
	if(x >= mazeWidth || x < 0){
		return NULL;
	}
	if(y >= mazeHeight || y < 0){
		return NULL;
	}
	int index = (y*mazeWidth)+x;
	int size = blockList.size();
	if(index < 0 || index >= size){
		return NULL;
	}
	//Block* targBlock = blockList[index];
	//printf("A: %i %i T: %i %i\n", targBlock->GetX(), targBlock->GetY(), x, y);
	return blockList[index];
}

Block* Maze::GetOppositeBlock(Block* cBlock, Block* wBlock){
	if(!cBlock) return NULL;
	int x = cBlock->GetX();
	int y = cBlock->GetY();

	int wX = wBlock->GetX();
	int wY = wBlock->GetY();

	int xDif = wX-x;
	int yDif = wY-y;
	//printf("Differs: %i %i\n", xDif, yDif);
	/*int xFin = x+xDif*2;
	int yFin = y+yDif*2;

	printf("Hmm: %i %i - %i %i\n", x, y, xFin, yFin);*/

	Block* fBlock = Maze::GetBlock(x+xDif*2, y+yDif*2);
	return fBlock;
}

int Maze::GetMazeHeight(){
	return mazeHeight;
}

int Maze::GetMazeWidth(){
	return mazeWidth;
}