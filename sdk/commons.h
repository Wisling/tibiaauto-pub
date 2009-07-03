/////////////////////////////////////////////////////////////////////////////
#include "PackSenderProxy.h"
#include "MemReaderProxy.h"
#include "ModuleUtil.h"

int itemOnTopIndex(int x,int y,int z)//Now uses Tibia's own indexing system found in memory to determine this
{
	CMemReaderProxy reader;
	int pos;

	int stackCount=reader.mapGetPointItemsCount(point(x,y,z));
	if (stackCount==0) AfxMessageBox("Fatal Error: Tile does not exist.");
	for (pos=0;pos<stackCount;pos++)
	{
		int stackInd=reader.mapGetPointStackIndex(point(x,y,z),pos);
		if(stackInd==stackCount-1){
			return pos;
			break;
		}
	}
	return -1;
}

int itemOnTopIndex(int x,int y)
{
	return itemOnTopIndex(x,y,0);
}

int isItemOnTop(int x,int y,int *itemArr,int itemArrSize)
{
	CMemReaderProxy reader;
	int topPos=itemOnTopIndex(x,y);
	int tileId = reader.mapGetPointItemId(point(x,y,0),topPos);
	for (int i=0;i<itemArrSize;i++)
	{
		if (tileId==itemArr[i]) return tileId;
	}		
	return 0;
}

int isItemCovered(int x,int y,int *itemArr,int itemArrSize)
{
	CMemReaderProxy reader;

	int topPos=itemOnTopIndex(x,y);
	int stackCount=reader.mapGetPointItemsCount(point(x,y,0));
	for (int pos=topPos+1;pos!=topPos;pos=(pos+1)%stackCount)
	{
		int tileId = reader.mapGetPointItemId(point(x,y,0),pos);
		for (int i=0;i<itemArrSize;i++)
		{
			if (tileId==itemArr[i]) return tileId;
		}		
	}
	return 0;
}

int isItemOnTop(int x,int y,int itemId)
{
	int itemArr[1]={itemId};
	return isItemOnTop(x,y,itemArr,1);
}

int isItemCovered(int x,int y,int itemId)
{
	int itemArr[1]={itemId};
	return isItemCovered(x,y,itemArr,1);
}

int itemOnTopCode(int x,int y)
{

	CMemReaderProxy reader;
	int pos=itemOnTopIndex(x,y);
	if (pos!=-1)
	{
		return reader.mapGetPointItemId(point(x,y,0),pos);
	}
	return 0;
}

int itemSeenOnTopIndex(int x,int y)
{
	CMemReaderProxy reader;
	int pos;	
	int stackCount=reader.mapGetPointItemsCount(point(x,y,0));
	for (pos=0;pos<stackCount;pos++)
	{
		int stackInd=reader.mapGetPointStackIndex(point(x,y,0),pos);
		int tileId = reader.mapGetPointItemId(point(x,y,0),pos);
		if (stackInd==stackCount-1 || tileId==99)
			return pos;
	}
	return stackCount-1;
}

int itemSeenOnTopCode(int x,int y)
{
	CMemReaderProxy reader;
	int pos=itemSeenOnTopIndex(x,y);
	if (pos!=-1)
	{
		return reader.mapGetPointItemId(point(x,y,0),pos);
	}
	return 0;
}

int itemOnTopQty(int x,int y)
{
	CMemReaderProxy reader;
	int qty=reader.mapGetPointItemExtraInfo(point(x,y,0),itemOnTopIndex(x,y),1);
	return qty?qty:1;
}

int findNextClosedContainer(int afterCont=-1)
{

	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaContainer *container;
	CMemConstData memConstData = reader.getMemConstData();
	int targetBag;
	for (targetBag=afterCont+1;targetBag<memConstData.m_memMaxContainers;targetBag++){
		container = reader.readContainer(targetBag);
		if (!container->flagOnOff) break;
	}
	
	if (targetBag == memConstData.m_memMaxContainers){
		targetBag=memConstData.m_memMaxContainers-1;
		sender.closeContainer(targetBag);
		CModuleUtil::waitForOpenContainer(targetBag,true);

	}
	return targetBag;
}