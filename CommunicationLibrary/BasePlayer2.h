#pragma once
#include <memory>
#include "../MJLibrary/MJ_Base/PlayerTile.h"
#include "../MJLibrary/MJ_Base/MJBaseTypedef.h"

class BaseThrowHandler
{
public:
	BaseThrowHandler() {};
	~BaseThrowHandler() {};

	virtual Tile throwTile() = 0;
};

class BaseMeldHandler
{
public:
	BaseMeldHandler() {};
	~BaseMeldHandler() {};

	virtual MoveType meld(const Tile& oTargetTile, const bool& bCanEat, const bool& bCanKong) = 0;
};

class BaseDarkKongOrUpgradeKongHandler
{
public:
	BaseDarkKongOrUpgradeKongHandler() {};
	~BaseDarkKongOrUpgradeKongHandler() {};

	virtual std::pair<MoveType, Tile> darkKongOrUpgradeKong() = 0;
};

class BasePlayer2
{
public:
	BasePlayer2() : m_oRemainTile(RemainTile_t(RemainTileType::Type_Playing)) {};
	~BasePlayer2() {};

	Tile askThrow() { return m_pThrowHandler->throwTile(); };
	MoveType askEat(const Tile& oTile);
	//inline bool askEatRight(const Tile& oTile) { return m_pMeldHandler->meld(oTile, true, false) == MoveType::Move_EatRight; };
	//inline bool askEatMiddle(const Tile& oTile) { return m_pMeldHandler->meld(oTile, true, false) == MoveType::Move_EatMiddle; };
	//inline bool askEatLeft(const Tile& oTile) { return m_pMeldHandler->meld(oTile, true, false) == MoveType::Move_EatLeft; };
	inline bool askPong(const Tile& oTile, const bool& bCanChow, const bool& bCanKong) { return m_pMeldHandler->meld(oTile, bCanChow, bCanKong) == MoveType::Move_Pong; };
	inline bool askKong(const Tile& oTile) { return m_pMeldHandler->meld(oTile, false, true) == MoveType::Move_Kong; };
	std::pair<MoveType, Tile> askDarkKongOrUpgradeKong();

	inline void throwTile(const Tile& oTile) { m_oPlayerTile.popTileFromHandTile(oTile); };
	void drawTile(const Tile& oTile);

public:
	PlayerTile m_oPlayerTile;
	RemainTile_t m_oRemainTile;
	uint32_t m_iSeat;								//[1, 4]

protected:
	std::shared_ptr<BaseThrowHandler> m_pThrowHandler;
	std::shared_ptr<BaseMeldHandler> m_pMeldHandler;
	std::shared_ptr<BaseDarkKongOrUpgradeKongHandler> m_pDarkKongOrUpgradeKongHandler;
};

