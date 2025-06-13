#pragma once
#include "../CommunicationLibrary/BasePlayer.h"
#include "../MJLibrary/Base/ReserviorOneSampler.h"
#include <cassert>

class DummyPlayer : public BasePlayer
{
public:
	DummyPlayer() : BasePlayer() {};
	DummyPlayer(const PlayerTile& oPlayerTile, const RemainTile_t& oRemainTile) : BasePlayer(oPlayerTile, oRemainTile) {
		for (int i = MAX_DIFFERENT_TILE_COUNT - 1; i >= 0; i--) {
			if (oPlayerTile.getHandTileNumber(i) > 0) {
				oPreviousDrawnTile = i;
				break;
			}
		}
	};
	~DummyPlayer() {};

	void reset() override { 
		BasePlayer::reset();
		oPreviousDrawnTile = Tile(); 
	};
	Tile askThrow() override { 
		if (!oPreviousDrawnTile.isNull())
			return oPreviousDrawnTile;

		//random throw a tile from hand
		ReserviorOneSampler<int> oSampler;
		for (int i = MAX_DIFFERENT_TILE_COUNT - 1; i >= 0; i--) {
			if (m_oPlayerTile.getHandTileNumber(i) > 0) {
				oSampler.input(i);
			}
		}
		return Tile(oSampler.getData());
	};
	MoveType askEat(const Tile& oTile) override { return MoveType::Move_Pass; };
	bool askPong(const Tile& oTile, const bool& bCanChow, const bool& bCanKong) override { return false; };
	bool askKong(const Tile& oTile) override { return false; };
	std::pair<MoveType, Tile> askDarkKongOrUpgradeKong() override { return std::pair<MoveType, Tile>(MoveType::Move_Pass, Tile()); };

	inline void drawTile(const Tile& oTile) {
		BasePlayer::drawTile(oTile);
		//std::cerr << "[DummyPlayer::drawTile] Draw " << oTile.toString() << std::endl;
		oPreviousDrawnTile = oTile;
	}

private:
	Tile oPreviousDrawnTile;
};

