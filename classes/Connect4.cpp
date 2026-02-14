#include "Connect4.h"


Connect4::Connect4()
{
    _grid = new Grid(7, 6);
}

Connect4::~Connect4()
{
    delete _grid;
}

Bit* Connect4::PieceForPlayer(const int playerNumber)
{
    // depending on playerNumber load the "red.png" or the "yellow.png" graphic
    Bit *bit = new Bit();
    bit->LoadTextureFromFile(playerNumber == AI_PLAYER ? "red.png" : "yellow.png");
    bit->setOwner(getPlayerAt(playerNumber == AI_PLAYER ? 1 : 0));
    return bit;
}

void Connect4::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 7;
    _gameOptions.rowY = 6;
    _grid->initializeSquares(80, "square.png");

    if (gameHasAI()) {
        setAIPlayer(AI_PLAYER);
    }

    startGame();
}

bool Connect4::actionForEmptyHolder(BitHolder &holder)
{
    if (holder.bit()) {
        return false;
    }
    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber() == 0 ? HUMAN_PLAYER : AI_PLAYER);
    if (bit) {
        bit->setPosition(holder.getPosition());

        ChessSquare *newPos = (ChessSquare*)&holder;
        int x = newPos->getColumn();
        int y = newPos->getRow();

        ChessSquare *below = newPos;

        // while below exists and is not occupied, set new position to below current
        while (below != nullptr && !below->bit()) {
            newPos = below;
            x = newPos->getColumn();
            y = newPos->getRow();
            below = _grid->getS(x,y);
        }

        bit->setPosition(newPos->getPosition());
        newPos->setBit(bit);
        endTurn();
        return true;
    }   
    return false;
}

bool Connect4::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    return false;
}

bool Connect4::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    return false;
}

void Connect4::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

Player* Connect4::ownerAt(int index ) const
{
    auto square = _grid->getSquare(index % 7, index / 7);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

Player* Connect4::checkForWinner()
{
    static const int WinningFours[69][4] = {

        // HORIZONTAL
        {0,1,2,3}, {1,2,3,4}, {2,3,4,5}, {3,4,5,6},
        {7,8,9,10}, {8,9,10,11}, {9,10,11,12}, {10,11,12,13},
        {14,15,16,17}, {15,16,17,18}, {16,17,18,19}, {17,18,19,20},
        {21,22,23,24}, {22,23,24,25}, {23,24,25,26}, {24,25,26,27},
        {28,29,30,31}, {29,30,31,32}, {30,31,32,33}, {31,32,33,34},
        {35,36,37,38}, {36,37,38,39}, {37,38,39,40}, {38,39,40,41},

        // VERTICAL
        {0,7,14,21}, {7,14,21,28}, {14,21,28,35},
        {1,8,15,22}, {8,15,22,29}, {15,22,29,36},
        {2,9,16,23}, {9,16,23,30}, {16,23,30,37},
        {3,10,17,24}, {10,17,24,31}, {17,24,31,38},
        {4,11,18,25}, {11,18,25,32}, {18,25,32,39},
        {5,12,19,26}, {12,19,26,33}, {19,26,33,40},
        {6,13,20,27}, {13,20,27,34}, {20,27,34,41},

        // DIAGONAL RIGHT [\]
        {0,8,16,24}, {1,9,17,25}, {2,10,18,26}, {3,11,19,27},
        {7,15,23,31}, {8,16,24,32}, {9,17,25,33}, {10,18,26,34},
        {14,22,30,38}, {15,23,31,39}, {16,24,32,40}, {17,25,33,41},

        // DIAGONAL LEFT [/]
        {3,9,15,21}, {4,10,16,22}, {5,11,17,23}, {6,12,18,24},
        {10,16,22,28}, {11,17,23,29}, {12,18,24,30}, {13,19,25,31},
        {17,23,29,35}, {18,24,30,36}, {19,25,31,37}, {20,26,32,38}
    };

    for(int i = 0; i < 69; i++) {
        const int *four = WinningFours[i];
        Player* p = ownerAt(four[0]);
        if (p && p == ownerAt(four[1]) && p == ownerAt(four[2]) && p == ownerAt(four[3])) {
            return p;
        }
    }
    return nullptr;
}

bool Connect4::checkForDraw()
{
    bool isDraw = true;
    // check to see if the board is full
    _grid->forEachSquare([&isDraw](ChessSquare* square, int x, int y) {
        if (!square->bit()) {
            isDraw = false;
        }
    });
    return isDraw;
}

std::string Connect4::initialStateString()
{
    return "000000000000000000000000000000000000000000";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string Connect4::stateString()
{
    std::string s = "000000000000000000000000000000000000000000";
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        Bit *bit = square->bit();
        if (bit) {
            s[y * 7 + x] = std::to_string(bit->getOwner()->playerNumber()+1)[0];
        }
    });
    return s;
}

void Connect4::setStateString(const std::string &s)
{
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y * 7 + x;
        int playerNumber = s[index] - '0';
        if (playerNumber) {
            square->setBit( PieceForPlayer(playerNumber - 1) );
        } else {
            square->setBit( nullptr );
        }
    });
}


void Connect4::updateAI() 
{
    if (checkForWinner()) {
        return;
    }

    MoveResult best { INT_MIN, -1 };
    std::string state = stateString();

    for (int i = 0; i < 7; i++) {
        if (state[i] != '0') continue;

        std::string prevState = state;
        state = findMove(state, i, AI_PLAYER);

        int score = -negamax(state, 7, HUMAN_PLAYER, INT_MIN, INT_MAX);

        if (score > best.score) {
            best.score = score;
            best.column = i;
        }

        state = prevState;
    }
    
    if (best.column != -1) {
        actionForEmptyHolder(*_grid->getSquare(best.column, 0));
    }
}

std::string Connect4::findMove(std::string state, int column, int playerNumber) {
    for (int i = 35 + column; i >= 0; i -= 7) {
        if (state[i] == '0') {
            state[i] = playerNumber == -1 ? '1' : '2';
            return state;
        }
    }
    return state;
}

int Connect4::negamax(std::string& state, int depth, int playerColor, int alpha, int beta) {
    int score = evalBoard(state);

    if (depth == 0 || checkForTerminal(state) || std::abs(score) >= 10000) return playerColor * score;
    
    int best = -10000;

    for (int i = 0; i < 7; i++) {
        if (state[i] != '0') continue;

        std::string prevState = state;
        state = findMove(state, i, playerColor);

        int newScore = -negamax(state, depth - 1, -playerColor, -beta, -alpha);

        if (newScore > best) {
            best = newScore;
        }

        state = prevState;

        alpha = std::max(alpha, newScore);
        if (alpha >= beta) {
            break;
        }
    }

    return best;
}

int Connect4::evalBoard(std::string& state) {
    static const int WinningFours[69][4] = {

        // HORIZONTAL
        {0,1,2,3}, {1,2,3,4}, {2,3,4,5}, {3,4,5,6},
        {7,8,9,10}, {8,9,10,11}, {9,10,11,12}, {10,11,12,13},
        {14,15,16,17}, {15,16,17,18}, {16,17,18,19}, {17,18,19,20},
        {21,22,23,24}, {22,23,24,25}, {23,24,25,26}, {24,25,26,27},
        {28,29,30,31}, {29,30,31,32}, {30,31,32,33}, {31,32,33,34},
        {35,36,37,38}, {36,37,38,39}, {37,38,39,40}, {38,39,40,41},

        // VERTICAL
        {0,7,14,21}, {7,14,21,28}, {14,21,28,35},
        {1,8,15,22}, {8,15,22,29}, {15,22,29,36},
        {2,9,16,23}, {9,16,23,30}, {16,23,30,37},
        {3,10,17,24}, {10,17,24,31}, {17,24,31,38},
        {4,11,18,25}, {11,18,25,32}, {18,25,32,39},
        {5,12,19,26}, {12,19,26,33}, {19,26,33,40},
        {6,13,20,27}, {13,20,27,34}, {20,27,34,41},

        // DIAGONAL RIGHT [\]
        {0,8,16,24}, {1,9,17,25}, {2,10,18,26}, {3,11,19,27},
        {7,15,23,31}, {8,16,24,32}, {9,17,25,33}, {10,18,26,34},
        {14,22,30,38}, {15,23,31,39}, {16,24,32,40}, {17,25,33,41},

        // DIAGONAL LEFT [/]
        {3,9,15,21}, {4,10,16,22}, {5,11,17,23}, {6,12,18,24},
        {10,16,22,28}, {11,17,23,29}, {12,18,24,30}, {13,19,25,31},
        {17,23,29,35}, {18,24,30,36}, {19,25,31,37}, {20,26,32,38}
    };

    int score = 0;

    for(int i = 0; i < 69; i++) {
        const int *four = WinningFours[i];
        char arr[4] = {state[four[0]], state[four[1]], state[four[2]], state[four[3]]};
        score += evalFour(arr);
    }

    return score;
}

int Connect4::evalFour(char arr[4]) {
    char ai, player;
    if (AI_PLAYER == -1) {
        ai = '1';
        player = '2';
    } else {
        ai = '2';
        player = '1';
    }
    
    int aiCount = 0;
    int playerCount = 0;
    
    for (int i = 0; i < sizeof(arr); i++) {
        if (arr[i] == ai) aiCount += 1;
        if (arr[i] == player) playerCount += 1; 
    }

    if (aiCount == 4) {
        return 10000;
    }

    if (playerCount == 4) {
        return -9000;
    }

    if (aiCount == 3 && playerCount == 0) {
        return 200;
    }

    if (playerCount == 0 && playerCount == 3) {
        return -200;
    }

    if (aiCount == 2 && playerCount == 0) {
        return 10;
    }

    if (playerCount == 0 && playerCount == 2) {
        return -10;
    }

    return 0;
}

bool Connect4::checkForTerminal(std::string& state) {
    for (int i = 0; i < 42; i++) {
        if (state[i] == '0') {
            return false;
        }
    }
    return true;
}