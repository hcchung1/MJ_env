#pragma once
#include "BasePlayer.h"
#include "../MJLibrary/Base/Ini.h"

class EpsilonGreedyPlayerConfig {
public:
	EpsilonGreedyPlayerConfig() : m_fProbaMakeRandomMove(0.0f), m_bMaintainBestMinLack(false) {};
	EpsilonGreedyPlayerConfig(Ini& oIni) { loadFromConfigFile(oIni); };
	~EpsilonGreedyPlayerConfig() {};

	int loadFromConfigFile(Ini& oIni, const bool& bForceLoadConfig = false)
	{
		m_fProbaMakeRandomMove = oIni.getDoubleIni("EpsilonGreedy.Epsilon");
		m_bMaintainBestMinLack = oIni.getIntIni("EpsilonGreedy.MaintainBestMinLackInEpsilonGreedy") > 0;
		return 0;
	}

	float m_fProbaMakeRandomMove;
	bool m_bMaintainBestMinLack;
};

class EpsilonGreedyPlayer : public BasePlayer
{
public:
	EpsilonGreedyPlayer();
	EpsilonGreedyPlayer(std::unique_ptr<BasePlayer>& pPlayer, const EpsilonGreedyPlayerConfig& oConfig, const int& iSeed);
	~EpsilonGreedyPlayer() {};

	void reset() override;
	Tile askThrow() override;
	MoveType askEat(const Tile& oTile) override;
	bool askPong(const Tile& oTile, const bool& bCanChow, const bool& bCanKong) override;
	bool askKong(const Tile& oTile) override;
	std::pair<MoveType, Tile> askDarkKongOrUpgradeKong() override;

	void throwTile(const Tile& oTile) override;
	void drawTile(const Tile& oTile) override;
	void doAction(const Move& oMove) override;
	void putTileToHandTile(const Tile& oTile) override;
	void popTileFromRemainTile(const Tile& oTile, const int& iTileNumber, const bool& bPreviousMoveIsDraw) override;
	void darkKong_otherPlayer() override;

private:
	double _random_double() { return dis_double(gen); };
	int _random_int() { return dis_int(gen); };
	Move _randomMove(const vector<Move>& vMoves, const bool& bMaintainBestMinLack = false);
	Tile _randomThrowTile(const bool& bMaintainBestMinLack = false);
	MoveType _randomEat(const Tile& oTargetTile, const bool& bMaintainBestMinLack = false);
	bool _randomPong(const Tile& oTargetTile, const bool& bMaintainBestMinLack = false);
	bool _randomKong(const Tile& oTargetTile, const bool& bMaintainBestMinLack = false) ;
	std::pair<MoveType, Tile> _randomDarkKongOrUpgradeKong(const bool& bMaintainBestMinLack = false);

	std::unique_ptr<BasePlayer> m_pPlayer;
	EpsilonGreedyPlayerConfig m_oConfig;

	//random related
	int m_iSeed;
	std::random_device rd;
	std::mt19937 gen;
	std::uniform_real_distribution<double> dis_double;
	std::uniform_int_distribution<int> dis_int;
};

