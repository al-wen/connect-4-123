#pragma once
#include "Game.h"

class Connect4 : public Game
{
public:
    Connect4();
    ~Connect4();

    // set up the board
    void        setUpBoard() override;

    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;
    void        setStateString(const std::string &s) override;
    bool        actionForEmptyHolder(BitHolder &holder) override;
    bool        canBitMoveFrom(Bit &bit, BitHolder &src) override;
    bool        canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    void        stopGame() override;

	void        updateAI() override;
    bool        gameHasAI() override { return true; }
    Grid* getGrid() override { return _grid; }
private:
    Bit *       PieceForPlayer(const int playerNumber);
    Player*     ownerAt(int index ) const;
    int         negamax(std::string& state, int depth, int playerColor, int alpha, int beta);

    struct      MoveResult { int score; int column; };

    std::string findMove(std::string state, int column, int playerNumber);
    int         evalBoard(std::string& state);
    int         evalFour(char arr[4]);
    bool        checkForTerminal(std::string& state);

    Grid*       _grid;
};

