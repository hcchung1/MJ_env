#include "EpsilonGreedyPlayer.h"

EpsilonGreedyPlayer::EpsilonGreedyPlayer()
{
	//[CAUTION]Do not use this constructor!!
}

EpsilonGreedyPlayer::EpsilonGreedyPlayer(std::unique_ptr<BasePlayer>& pPlayer, const EpsilonGreedyPlayerConfig & oConfig, const int& iSeed)
	: m_oConfig(oConfig), m_iSeed(iSeed)
{
	m_pPlayer = std::move(pPlayer);
	if (iSeed > 0) {
		gen.seed(iSeed);
	}
}

void EpsilonGreedyPlayer::reset()
{
	m_pPlayer->reset();
	BasePlayer::reset();
}

Tile EpsilonGreedyPlayer::askThrow()
{
	if (m_oConfig.m_fProbaMakeRandomMove > 0.0f) {
		float fRandomVal = _random_double();
		if (fRandomVal < m_oConfig.m_fProbaMakeRandomMove) {//triggered
			return _randomThrowTile(m_oConfig.m_bMaintainBestMinLack);
		}
	}
	return m_pPlayer->askThrow();
}

MoveType EpsilonGreedyPlayer::askEat(const Tile & oTile)
{
	if (m_oConfig.m_fProbaMakeRandomMove > 0.0f) {
		float fRandomVal = _random_double();
		if (fRandomVal < m_oConfig.m_fProbaMakeRandomMove) {//triggered
			return _randomEat(oTile, m_oConfig.m_bMaintainBestMinLack);
		}
	}
	return m_pPlayer->askEat(oTile);
}

bool EpsilonGreedyPlayer::askPong(const Tile & oTile, const bool & bCanChow, const bool & bCanKong)
{
	if (m_oConfig.m_fProbaMakeRandomMove > 0.0f) {
		float fRandomVal = _random_double();
		if (fRandomVal < m_oConfig.m_fProbaMakeRandomMove) {//triggered
			return _randomPong(oTile, m_oConfig.m_bMaintainBestMinLack);
		}
	}
	return m_pPlayer->askPong(oTile, bCanChow,bCanKong);
}

bool EpsilonGreedyPlayer::askKong(const Tile & oTile)
{
	if (m_oConfig.m_fProbaMakeRandomMove > 0.0f) {
		float fRandomVal = _random_double();
		if (fRandomVal < m_oConfig.m_fProbaMakeRandomMove) {//triggered
			return _randomKong(oTile, m_oConfig.m_bMaintainBestMinLack);
		}
	}
	return m_pPlayer->askKong(oTile);
}

std::pair<MoveType, Tile> EpsilonGreedyPlayer::askDarkKongOrUpgradeKong()
{
	if (m_oConfig.m_fProbaMakeRandomMove > 0.0f) {
		float fRandomVal = _random_double();
		if (fRandomVal < m_oConfig.m_fProbaMakeRandomMove) {//triggered
			return _randomDarkKongOrUpgradeKong(m_oConfig.m_bMaintainBestMinLack);
		}
	}
	return m_pPlayer->askDarkKongOrUpgradeKong();
}

void EpsilonGreedyPlayer::throwTile(const Tile & oTile)
{
	m_pPlayer->throwTile(oTile);
	BasePlayer::throwTile(oTile);

	if (m_oPlayerTile != m_pPlayer->getPlayerTile() || !(m_oRemainTile == m_pPlayer->getRemainTile())) {
		//Error
		cerr << m_oPlayerTile.toString() << " " << m_oRemainTile.toString() << endl;
		cerr << m_pPlayer->getPlayerTile().toString() << " " << m_pPlayer->getRemainTile().toString() << endl;
	}
	assert(m_oPlayerTile == m_pPlayer->getPlayerTile());
	assert(m_oRemainTile == m_pPlayer->getRemainTile());
}

void EpsilonGreedyPlayer::drawTile(const Tile & oTile)
{
	m_pPlayer->drawTile(oTile);
	assert(m_pPlayer->getPlayerTile().getHandTileNumber() <= MAX_HANDTILE_COUNT);
	BasePlayer::drawTile(oTile);

	if (m_oPlayerTile != m_pPlayer->getPlayerTile() || !(m_oRemainTile == m_pPlayer->getRemainTile())) {
		//Error
		cerr << m_oPlayerTile.toString() << " " << m_oRemainTile.toString() << endl;
		cerr << m_pPlayer->getPlayerTile().toString() << " " << m_pPlayer->getRemainTile().toString() << endl;
	}
	assert(m_oPlayerTile == m_pPlayer->getPlayerTile());
	assert(m_oRemainTile == m_pPlayer->getRemainTile());
}

void EpsilonGreedyPlayer::doAction(const Move & oMove)
{
	m_pPlayer->doAction(oMove);
	BasePlayer::doAction(oMove);

	if (m_oPlayerTile != m_pPlayer->getPlayerTile() || !(m_oRemainTile == m_pPlayer->getRemainTile())) {
		//Error
		cerr << m_oPlayerTile.toString() << " " << m_oRemainTile.toString() << endl;
		cerr << m_pPlayer->getPlayerTile().toString() << " " << m_pPlayer->getRemainTile().toString() << endl;
	}
	assert(m_oPlayerTile == m_pPlayer->getPlayerTile());
	assert(m_oRemainTile == m_pPlayer->getRemainTile());
}

void EpsilonGreedyPlayer::putTileToHandTile(const Tile & oTile)
{
	m_pPlayer->putTileToHandTile(oTile);
	BasePlayer::putTileToHandTile(oTile);
}

void EpsilonGreedyPlayer::popTileFromRemainTile(const Tile & oTile, const int& iTileNumber, const bool & bPreviousMoveIsDraw)
{
	m_pPlayer->popTileFromRemainTile(oTile, iTileNumber, bPreviousMoveIsDraw);
	BasePlayer::popTileFromRemainTile(oTile, iTileNumber, bPreviousMoveIsDraw);
}

void EpsilonGreedyPlayer::darkKong_otherPlayer()
{
	m_pPlayer->darkKong_otherPlayer();
	BasePlayer::darkKong_otherPlayer();
}

Move EpsilonGreedyPlayer::_randomMove(const vector<Move>& vMoves, const bool & bMaintainBestMinLack)
{
	assert(!vMoves.empty());

	if (bMaintainBestMinLack) {
		//setup candidate moves
		int iOriginalMinLack = m_oPlayerTile.getMinLack();
		vector<Move> vCandidateMoves;
		vCandidateMoves.reserve(vMoves.size());

		for (auto oMove : vMoves) {
			m_oPlayerTile.doAction(oMove);
			switch (oMove.getMoveType()) {
			case MoveType::Move_Throw:
			case MoveType::Move_Kong:
			case MoveType::Move_DarkKong:
			case MoveType::Move_UpgradeKong:
				if (m_oPlayerTile.getMinLack() == iOriginalMinLack) {
					vCandidateMoves.emplace_back(oMove);
				}
				break;
			case MoveType::Move_EatLeft:
			case MoveType::Move_EatMiddle:
			case MoveType::Move_EatRight:
			case MoveType::Move_Pong:
				if (m_oPlayerTile.getMinLack() < iOriginalMinLack) {
					vCandidateMoves.emplace_back(oMove);
				}
				break;
			case MoveType::Move_Pass:
			case MoveType::Move_Init:
				vCandidateMoves.emplace_back(oMove);
				break;
			default:
				break;
			}
			m_oPlayerTile.undoAction(oMove);
		}

		//Random select a candidate move.
		assert(!vCandidateMoves.empty());
		int iRandomIndex = _random_int() % vCandidateMoves.size();
		return vCandidateMoves[iRandomIndex];
	}
	else {
		int iRandomIndex = _random_int() % vMoves.size();
		return vMoves[iRandomIndex];
	}
}

Tile EpsilonGreedyPlayer::_randomThrowTile(const bool & bMaintainBestMinLack)
{
	vector<Move> vCanThrowMoves;
	vCanThrowMoves.reserve(MAX_HANDTILE_COUNT);
	for (int i = 0; i < MAX_DIFFERENT_TILE_COUNT; i++) {
		if (m_oPlayerTile.getHandTileNumber(i) > 0)
			vCanThrowMoves.emplace_back(Move(MoveType::Move_Throw, Tile(i)));
	}
	return _randomMove(vCanThrowMoves, bMaintainBestMinLack).getTargetTile();

	/*vector<Tile> vCanThrowTiles;
	vCanThrowTiles.reserve(MAX_HANDTILE_COUNT);

	if (bMaintainBestMinLack) {
		int iOriginalMinLack = m_oPlayerTile.getMinLack();
		for (int i = 0; i < MAX_DIFFERENT_TILE_COUNT; i++) {
			if (m_oPlayerTile.getHandTileNumber(i) <= 0)
				continue;
			m_oPlayerTile.popTileFromHandTile(i);
			if (m_oPlayerTile.getMinLack() == iOriginalMinLack)
				vCanThrowTiles.emplace_back(i);
			m_oPlayerTile.putTileToHandTile(i);
		}
	}
	else {
		for (int i = 0; i < MAX_DIFFERENT_TILE_COUNT; i++) {
			if (m_oPlayerTile.getHandTileNumber(i) > 0)
				vCanThrowTiles.emplace_back(i);
		}
	}

	int iRandomIndex = _random_int() % vCanThrowTiles.size();
	return vCanThrowTiles[iRandomIndex];*/
}

MoveType EpsilonGreedyPlayer::_randomEat(const Tile & oTargetTile, const bool & bMaintainBestMinLack)
{
	vector<Move> vAvailableMoves;
	vAvailableMoves.reserve(4);
	vAvailableMoves.emplace_back(Move(MoveType::Move_Pass));
	if (m_oPlayerTile.canEatLeft(oTargetTile))
		vAvailableMoves.emplace_back(Move(MoveType::Move_EatLeft, oTargetTile));
	if (m_oPlayerTile.canEatMiddle(oTargetTile))
		vAvailableMoves.emplace_back(Move(MoveType::Move_EatMiddle, oTargetTile));
	if (m_oPlayerTile.canEatRight(oTargetTile))
		vAvailableMoves.emplace_back(Move(MoveType::Move_EatRight, oTargetTile));

	return _randomMove(vAvailableMoves, bMaintainBestMinLack).getMoveType();
}

bool EpsilonGreedyPlayer::_randomPong(const Tile & oTargetTile, const bool & bMaintainBestMinLack)
{
	vector<Move> vAvailableMoves;
	vAvailableMoves.emplace_back(Move(MoveType::Move_Pass));
	if (m_oPlayerTile.canPong(oTargetTile))
		vAvailableMoves.emplace_back(Move(MoveType::Move_Pong, oTargetTile));

	return _randomMove(vAvailableMoves, bMaintainBestMinLack).getMoveType() == MoveType::Move_Pong;
}

bool EpsilonGreedyPlayer::_randomKong(const Tile & oTargetTile, const bool & bMaintainBestMinLack)
{
	vector<Move> vAvailableMoves;
	vAvailableMoves.emplace_back(Move(MoveType::Move_Pass));
	if (m_oPlayerTile.canKong(oTargetTile))
		vAvailableMoves.emplace_back(Move(MoveType::Move_Kong, oTargetTile));

	return _randomMove(vAvailableMoves, bMaintainBestMinLack).getMoveType() == MoveType::Move_Kong;
}

std::pair<MoveType, Tile> EpsilonGreedyPlayer::_randomDarkKongOrUpgradeKong(const bool & bMaintainBestMinLack)
{
	vector<Move> vAvailableMoves;
	//push a initial move
	vAvailableMoves.emplace_back(Move(MoveType::Move_Init, Tile()));

	//push all legal darkKong & upgradeKong moves
	for (int i = 0; i < MAX_DIFFERENT_TILE_COUNT; i++) {
		if (m_oPlayerTile.canDarkKong(i))
			vAvailableMoves.emplace_back(Move(MoveType::Move_DarkKong, Tile(i)));
		if (m_oPlayerTile.canUpgradeKong(i))
			vAvailableMoves.emplace_back(Move(MoveType::Move_UpgradeKong, Tile(i)));
	}

	Move oMove = _randomMove(vAvailableMoves, bMaintainBestMinLack);
	if (oMove.getMoveType() == MoveType::Move_DarkKong || oMove.getMoveType() == MoveType::Move_UpgradeKong)
		return std::pair<MoveType, Tile>(oMove.getMoveType(), oMove.getTargetTile());
	return std::pair<MoveType, Tile>(MoveType::Move_Pass, Tile());
}
