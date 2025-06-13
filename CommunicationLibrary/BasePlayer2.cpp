#include "BasePlayer2.h"

std::pair<MoveType, Tile> BasePlayer2::askDarkKongOrUpgradeKong()
{
	std::pair<MoveType, Tile> oAns = m_pDarkKongOrUpgradeKongHandler->darkKongOrUpgradeKong();
	//if(oAns.first != MoveType::Move_DarkKong && oAns.first != MoveType::Move_UpgradeKong)
		//return pair<MoveType, Tile>(MoveType::Move_Pass, Tile());
	return oAns;
}

void BasePlayer2::drawTile(const Tile & oTile)
{
	m_oPlayerTile.putTileToHandTile(oTile);
	m_oRemainTile.popTile(oTile);
}

MoveType BasePlayer2::askEat(const Tile & oTile)
{
	MoveType oMeldType = m_pMeldHandler->meld(oTile, true, false);
	if (oMeldType == MoveType::Move_Pong || oMeldType == MoveType::Move_Kong)
		return MoveType::Move_Pass;
	return oMeldType;
}
