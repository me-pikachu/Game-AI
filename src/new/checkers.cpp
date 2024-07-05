#include <bits/stdc++.h>

// for better speed and memory management
// change board size according to cache needs for faster results
// If cache line size is 32 bytes then 26 bytes is preferred and if it is 64 bytes then 51 or 52 bytes is preferred
#define board_size 51 // 51 bytes or 52 bytes or 26 bytes
#define PAWN_VALUE 1000
#define KINGPAWN_VALUE 2500
#define ENDGAME_KINGPAWN_VALUE_INCREMENT 500 // final value of KINGPAWN_VALUE is 3000
#define CENTER_ADVANTAGE 50
#define OPPOSITE_CENTER_ADVANTAGE 75
#define CORNER_INVASION_ADVANTAGE 120
#define INVASION_ADVANTAGE 35
#define BACK_ROW_IN_PLACE 15
#define MIDGAME_KING_IN_CORNER 30
#define ENDGAME_POSSIBLE_GAMEOVER 5000
#define ENDGAME_CONNECTED_PAWN 30
#define ENDGAME_PAWN_PUSH 25
#define ENDGAME_KING_IN_CENTER 20

#define BASIC_PAWN_VALUE 1
#define BASIC_KINGPAWN_VALUE 3
#define byte char // byte uses 1 bytes similar to char

const int GAMEOVEREVAL = 100000; // evaluation if white wins (-1*GAMEOVEREVAL if black wins)

struct position{
    // the current position of the game
    // the board is 10x10 = 100 square but only 50 squares can have pieces on them
    // #ifndef is used to check whether a particular compiler directive is defined or not
    // #if is used to compare it's value
    #if board_size == 51
        byte board[50]; // the current board
        // represent only movable squares
        // only the lower four bits represents the pieces
        // upper four bits are 0
        // 0000 -> empty square (0)
        // 0010 -> white pawn (2)
        // 0011 -> white king pawn (3)
        // 0100 -> black pawn (4)
        // 0101 -> black king pawn (5)
        // rest all other variations are absurd
        byte tomove;
        // tomove is of one byte
        // 0 1 2 3 4 5 6 7 8
        // 0th bit represent whether black would move or white(0 for black, 1 for white)
        // 1st to 7th bit => 6bits -> represent forces_capture square is any(0 to 49)
        // 7th bit represent if forces capture valid or not (1->valid, 0->not valid)
        // tomove > 0 -> no force capture

        bool operator==(const position& other) const {
            if (this->tomove != other.tomove) return 0;
            for (int i=0; i<50; ++i){
                if (this->board[i] != other.board[i]) return 0;
            }
            return 1;
        }

        position& operator=(const position& other){
            this->tomove = other.tomove;
            for (int i=0; i<50; ++i){
                this->board[i] = other.board[i];
            }
            return *this;
        }

        position(){
            memset(this, 0, sizeof(position));
        }

        position(const position& other){
            this->tomove = other.tomove;
            for (int i=0; i<50; ++i){
                this->board[i] = other.board[i];
            }
        }

    #elif board_size == 52
        byte board[50]; // the current board
        // only the lower four bits represents the pieces
        // upper four bits are 0
        // 0000 -> empty square
        // 0010 -> white pawn
        // 0011 -> white king pawn
        // 0100 -> black pawn
        // 0101 -> black king pawn
        // rest all other variations are absurd
        byte tomove;
        // tomove is of one byte(0 for black, 1 for white)
        byte forced_capture;
        // represent the square of forces capture if any

        bool operator==(const position& other) const {
            if (this->tomove != other.tomove) return 0;
            if (this->forced_capture != other.forced_capture) return 0;
            for (int i=0; i<50; ++i){
                if (this->board[i] != other.board[i]) return 0;
            }
            return 1;
        }

        bool operator=(const position& other){
            this->tomove = other.tomove;
            this->forced_capture = other.forced_capture;
            for (int i=0; i<50; ++i){
                this->board[i] = other.board[i];
            }
        }

        position(){
            memset(this, 0, sizeof(position));
        }

        position(const position& other){
            this->tomove = other.tomove;
            for (int i=0; i<50; ++i){
                this->board[i] = other.board[i];
            }
        }

    #elif board_size == 26
        byte board[25]; // the current board
        // every char value holds two movable squares
        // only the lower four bits represents the pieces
        // upper four bits are 0
        // 0000 -> empty square
        // 0010 -> white pawn
        // 0011 -> white king pawn
        // 0100 -> black pawn
        // 0101 -> black king pawn
        // rest all other variations are absurd
        byte tomove;
        // tomove is of one byte
        // 0 1 2 3 4 5 6 7 8
        // 0th bit represent whether black would move or white(0 for black, 1 for white)
        // 1st to 7th bit => 6bits -> represent forces_capture square is any(0 to 49)  

        bool operator==(const position& other) const {
            if (this->tomove != other.tomove) return 0;
            for (int i=0; i<25; ++i){
                if (this->board[i] != other.board[i]) return 0;
            }
            return 1;
        }

    #else
        // should never reach here
        assert(0);
    #endif   
};

position get_init_pos(){
    #if board_size == 51
    position pos;
    for (int i=0; i<20; ++i){
        pos.board[i] = 2;
    }

    for (int i=20; i<30; ++i){
        pos.board[i] = 0;
    }

    for (int i=30; i<50; ++i){
        pos.board[i] = 4;
    }
    pos.tomove = 1; // white to move
    return pos;
    #elif board_size == 52
    position pos;
    for (int i=0; i<10; ++i){
        pos.board[i] = 3;
    }

    for (int i=10; i<30; ++i){
        pos.board[i] = 0;
    }

    for (int i=30; i<50; ++i){
        pos.board[i] = 4;
    }
    pos.tomove = 1; // white to move
    pos.forced_capture = 0; // no forced capture
    return pos;
    #elif board_size == 26
        // todo    
    #else
        // should never reach here
        assert(0);
    #endif
}

position get_input_pos(){
    #if board_size == 51
    position pos;
    for (int i=9; i>=0; --i){
        std::string str;
        getline(std::cin, str);
        if (i & 1){
            for (int j=18; j>=2; j=j-4){
                if (str[j] == '.') pos.board[i*5 + j/4] = 0;
                else if (str[j] == 'w') pos.board[i*5 + j/4] = 2;
                else if (str[j] == 'W') pos.board[i*5 + j/4] = 3;
                else if (str[j] == 'b') pos.board[i*5 + j/4] = 4;
                else if (str[j] == 'B') pos.board[i*5 + j/4] = 5;
                else assert(0);
            }
        } else {
            for (int j=16; j>=0; j=j-4){
                if (str[j] == '.') pos.board[i*5 + j/4] = 0;
                else if (str[j] == 'w') pos.board[i*5 + j/4] = 2;
                else if (str[j] == 'W') pos.board[i*5 + j/4] = 3;
                else if (str[j] == 'b') pos.board[i*5 + j/4] = 4;
                else if (str[j] == 'B') pos.board[i*5 + j/4] = 5;
                else assert(0);
            }
        }
    }
    int tomove;
    std::cin >> tomove;
    pos.tomove = tomove;
    return pos;
    #elif board_size == 52
    position pos;
    for (int i=9; i>=0; --i){
        std::string str;
        getline(std::cin, str);
        if (i & 1){
            for (int j=18; j>=2; j=j-4){
                if (str[j] == '.') pos.board[i*5 + j/4] = 0;
                else if (str[j] == 'w') pos.board[i*5 + j/4] = 2;
                else if (str[j] == 'W') pos.board[i*5 + j/4] = 3;
                else if (str[j] == 'b') pos.board[i*5 + j/4] = 4;
                else if (str[j] == 'B') pos.board[i*5 + j/4] = 5;
                else assert(0);
            }
        } else {
            for (int j=16; j>=0; j=j-4){
                if (str[j] == '.') pos.board[i*5 + j/4] = 0;
                else if (str[j] == 'w') pos.board[i*5 + j/4] = 2;
                else if (str[j] == 'W') pos.board[i*5 + j/4] = 3;
                else if (str[j] == 'b') pos.board[i*5 + j/4] = 4;
                else if (str[j] == 'B') pos.board[i*5 + j/4] = 5;
                else assert(0);
            }
        }
    }
    bool tomove;
    std::cin >> tomove;
    int forced_capture;
    std::cin >> forced_capture;
    pos.tomove = tomove;
    pos.forced_capture = forced_capture;
    return pos;
    #elif board_size == 26
        // todo    
    #else
        // should never reach here
        assert(0);
    #endif
}

// we need a hash function for the position
// that hash function would be used in the transposition table
struct position_hash{
    #if board_size == 51
    uint64_t operator()(const position& curpos) const {
        // some hash function
        uint64_t hash = 0;

        int i = 0, j = 0;
        for (; i<22; ++i){
            hash += ((int)curpos.board[i] << j);
            j += 3;
        }

        j = 1;
        for (; i<44; ++i){
            hash ^= ((int)curpos.board[i] << j);
            j += 3;
        }

        j = 2;
        for (; i<50; ++i){
            hash ^= ((int)curpos.board[i] << j);
            j += 11;
        }

        // would not consider .tomove (it's okay if it is black or white to move having the 
        // same board to hash to the same hash value
        return hash;
    }
    #elif board_size == 52
    uint64_t operator()(const position& curpos) const {
        // some hash function
        uint64_t hash = 0;

        int i = 0, j = 0;
        for (; i<22; ++i){
            hash += ((int)curpos.board[i] << j);
            j += 3;
        }

        j = 1;
        for (; i<44; ++i){
            hash ^= ((int)curpos.board[i] << j);
            j += 3;
        }

        j = 2;
        for (; i<50; ++i){
            hash ^= ((int)curpos.board[i] << j);
            j += 11;
        }

        // would not consider .tomove (it's okay if it is black or white to move having the 
        // same board to hash to the same hash value
        return hash;
    }
    #elif board_size == 26
        // todo
    #else
        // should never reach here
        assert(0);
    #endif
};

struct move{
    byte initialsq;
    byte finalsq;
    move(){
        initialsq = -1;
        finalsq = -1;
    }
    
    move(int sq1, int sq2){
        initialsq = sq1;
        finalsq = sq2;
    }
};

void print_board(position pos){
    int i, j;
    #if board_size == 51
        for (i=9; i>=0; --i){
            if (i % 2 == 0){
                // i is even
                for (j=0; j<5; ++j){
                    char piece = pos.board[i*5+j];
                    if (piece == 0){
                        piece = '.';
                    } else if (piece == 2){
                        piece = 'w';
                    } else if (piece == 3){
                        piece = 'W';
                    } else if (piece == 4){
                        piece = 'b';
                    } else if (piece == 5){
                        piece = 'B';
                    } else {
                        // should not reach here
                        assert(0);
                    }
                    std::cout << piece << " " << "." << " ";
                }
            } else {
                // i is odd
                for (j=0; j<5; ++j){
                    char piece = pos.board[i*5+j];
                    if (piece == 0){
                        piece = '.';
                    } else if (piece == 2){
                        piece = 'w';
                    } else if (piece == 3){
                        piece = 'W';
                    } else if (piece == 4){
                        piece = 'b';
                    } else if (piece == 5){
                        piece = 'B';
                    } else {
                        // should not reach here
                        std::cout << (int)piece << std::endl;
                        assert(0);
                    }
                    std::cout << "." << " " << piece << " ";
                }
            }
            std::cout << std::endl;
        }
        std::cout << (int)pos.tomove << std::endl;
    #elif board_size == 52
        for (i=9; i>=0; --i){
            if (i % 2 == 0){
                // i is even
                for (j=0; j<5; ++j){
                    char piece = pos.board[i*5+j];
                    if (piece == 0){
                        piece = '.';
                    } else if (piece == 2){
                        piece = 'w';
                    } else if (piece == 3){
                        piece = 'W';
                    } else if (piece == 4){
                        piece = 'b';
                    } else if (piece == 5){
                        piece = 'B';
                    } else {
                        // should not reach here
                        assert(0);
                    }
                    std::cout << piece << " " << "." << " ";
                }
            } else {
                // i is odd
                for (j=0; j<5; ++j){
                    char piece = pos.board[i*5+j];
                    if (piece == 0){
                        piece = '.';
                    } else if (piece == 2){
                        piece = 'w';
                    } else if (piece == 3){
                        piece = 'W';
                    } else if (piece == 4){
                        piece = 'b';
                    } else if (piece == 5){
                        piece = 'B';
                    } else {
                        // should not reach here
                        assert(0);
                    }
                    std::cout << "." << " " << piece << " ";
                }
            }
            std::cout << std::endl;
        }
        std::cout << (int)pos.tomove << std::endl;
        std::cout << (int)pos.forced_capture << std::endl;
    #elif board_size == 26
        // todo
    #else
        // should never reach here
        assert(0);
    #endif   
}

bool has_capture(byte board[], byte square){
    #if board_size == 51
        byte row = square / 5;
        byte col = square % 5;
        if (board[square] == 2){
            // right capture
            if (row % 2 == 0){
                // row is even
                // right capture
                if (col + 1 < 5){
                    // forward capture
                    if (row + 2 < 10){
                        // now check whether the pawn are placed correctly or not
                        if ((board[square+5] == 4 || board[square+5] == 5) && board[square+11] == 0){
                            // this is a possible move
                            return 1;
                        }
                    }

                    // backward capture
                    if (row - 2 >= 0){
                        // now check whether the pawn are placed correctly or not
                        if ((board[square-5] == 4 || board[square-5] == 5) && board[square-9] == 0){
                            // this is a possible move
                            return 1;
                        }
                    }
                }

                    // left capture
                if (col - 1 >= 0){
                    // forward capture
                    if (row + 2 < 10){
                        // now check whether the pawn are placed correctly or not
                        if ((board[square+4] == 4 || board[square+4] == 5) && board[square+9] == 0){
                            // this is a possible move
                            return 1;
                        }
                    }

                    // backward capture
                    if (row - 2 >= 0){
                        // now check whether the pawn are placed correctly or not
                        if ((board[square-6] == 4 || board[square-6] == 5) && board[square-11] == 0){
                            // this is a possible move
                            return 1;
                        }
                    }
                }

            } else {
                // row is odd

                // row is even
                // right capture
                if (col + 1 < 5){
                    // forward capture
                    if (row + 2 < 10){
                        // now check whether the pawn are placed correctly or not
                        if ((board[square+6] == 4 || board[square+6] == 5) && board[square+11] == 0){
                            // this is a possible move
                            return 1;
                        }
                    }

                    // backward capture
                    if (row - 2 >= 0){
                        // now check whether the pawn are placed correctly or not
                        if ((board[square-4] == 4 || board[square-4] == 5) && board[square-9] == 0){
                            // this is a possible move
                            return 1;
                        }
                    }
                }

                // left capture
                if (col - 1 >= 0){
                    // forward capture
                    if (row + 2 < 10){
                        // now check whether the pawn are placed correctly or not
                        if ((board[square+5] == 4 || board[square+5] == 5) && board[square+9] == 0){
                            // this is a possible move
                            return 1;
                        }
                    }

                    // backward capture
                    if (row - 2 >= 0){
                        // now check whether the pawn are placed correctly or not
                        if ((board[square-5] == 4 || board[square-5] == 5) && board[square-11] == 0){
                            // this is a possible move
                            return 1;
                        }
                    }
                }
            }
        } else if (board[square] == 3){
            // right forward capture
            byte currow = row+1;
            byte curcol = (row % 2) ? col+1 : col;
            bool found = false;
            while (curcol < 5 && currow < 10){
                byte piece = board[currow*5+curcol];
                if (piece == 0){
                    if (currow % 2){
                        // row is odd
                        curcol++;
                    }
                    currow++;
                } else if (piece == 4 || piece == 5){
                    found = true;
                    break;
                } else {
                    // piece == 2 or piece == 3
                    break;
                }
            }

            if (found){
                if (currow % 2){
                    // currow is odd
                    curcol++;
                }
                currow++;
                while (curcol < 5 && currow < 10){
                    if (board[currow*5+curcol] == 0){
                        // this is a capture move
                        return 1;
                    } else {
                        // some piece present so capture either stops or no capture at all
                        break;
                    }
                    if (currow % 2){
                        // currow is odd
                        curcol++;
                    }
                    currow++;
                }                        
            }

            // left forward capture
            currow = row+1;
            curcol = (row % 2 == 0) ? col-1 : col;
            found = false;
            while (curcol >= 0 && currow < 10){
                byte piece = board[currow*5+curcol];
                if (piece == 0){
                    if (currow % 2 == 0){
                        // row is even
                        curcol--;
                    }
                    currow++;
                } else if (piece == 4 || piece == 5){
                    found = true;
                    break;
                } else {
                    // piece == 2 or piece == 3
                    break;
                }
            }

            if (found){
                if (currow % 2 == 0){
                    // currow is even
                    curcol--;
                }
                currow++;
                while (curcol >= 0 && currow < 10){
                    if (board[currow*5+curcol] == 0){
                        // this is a capture move
                        return 1;
                    } else {
                        // some piece present so capture either stops or no capture at all
                        break;
                    }
                    if (currow % 2 == 0){
                        // currow is even
                        curcol--;
                    }
                    currow++;
                }
            }

            // right backward capture
            currow = row-1;
            curcol = (row % 2) ? col+1 : col;
            found = false;
            while (curcol < 5 && currow >= 0){
                byte piece = board[currow*5+curcol];
                if (piece == 0){
                    if (currow % 2){
                        // row is odd
                        curcol++;
                    }
                    currow--;
                } else if (piece == 4 || piece == 5){
                    found = true;
                    break;
                } else {
                    // piece == 2 or piece == 3
                    break;
                }
            }

            if (found){
                if (currow % 2){
                    // currow is odd
                    curcol++;
                }
                currow--;
                while (curcol < 5 && currow >= 0){
                    if (board[currow*5+curcol] == 0){
                        // this is a capture move
                        return 1;
                    } else {
                        // some piece present so capture either stops or no capture at all
                        break;
                    }
                    if (currow % 2){
                        // currow is odd
                        curcol++;
                    }
                    currow--;
                }                        
            }

            // left backward capture
            currow = row-1;
            curcol = (row % 2 == 0) ? col-1 : col;
            found = false;
            while (curcol >= 0 && currow >= 0){
                byte piece = board[currow*5+curcol];
                if (piece == 0){
                    if (currow % 2 == 0){
                        // row is even
                        curcol--;
                    }
                    currow--;
                } else if (piece == 4 || piece == 5){
                    found = true;
                    break;
                } else {
                    // piece == 2 or piece == 3
                    break;
                }
            }

            if (found){
                if (currow % 2 == 0){
                    // currow is even
                    curcol--;
                }
                currow--;
                while (curcol >= 0 && currow >= 0){
                    if (board[currow*5+curcol] == 0){
                        // this is a capture move
                        return 1;
                    } else {
                        // some piece present so capture either stops or no capture at all
                        break;
                    }
                    if (currow % 2 == 0){
                        // currow is even
                        curcol--;
                    }
                    currow--;
                }
            }
        } else if (board[square] == 4){
            if (row % 2 == 0){
                // row is even
                // right capture
                if (col + 1 < 5){
                    // forward capture
                    if (row + 2 < 10){
                        // now check whether the pawn are placed correctly or not
                        if ((board[square+5] == 2 || board[square+5] == 3) && board[square+11] == 0){
                            // this is a possible move
                            return 1;
                        }
                    }

                    // backward capture
                    if (row - 2 >= 0){
                        // now check whether the pawn are placed correctly or not
                        if ((board[square-5] == 2 || board[square-5] == 3) && board[square-9] == 0){
                            // this is a possible move
                            return 1;
                        }
                    }
                }

                // left capture
                if (col - 1 >= 0){
                    // forward capture
                    if (row + 2 < 10){
                        // now check whether the pawn are placed correctly or not
                        if ((board[square+4] == 2 || board[square+4] == 3) && board[square+9] == 0){
                            // this is a possible move
                            return 1;
                        }
                    }

                    // backward capture
                    if (row - 2 >= 0){
                        // now check whether the pawn are placed correctly or not
                        if ((board[square-6] == 2 || board[square-6] == 3) && board[square-11] == 0){
                            // this is a possible move
                            return 1;
                        }
                    }
                }

            } else {
                // row is odd

                // row is even
                // right capture
                if (col + 1 < 5){
                    // forward capture
                    if (row + 2 < 10){
                        // now check whether the pawn are placed correctly or not
                        if ((board[square+6] == 2 || board[square+6] == 3) && board[square+11] == 0){
                            // this is a possible move
                            return 1;
                        }
                    }

                    // backward capture
                    if (row - 2 >= 0){
                        // now check whether the pawn are placed correctly or not
                        if ((board[square-4] == 2 || board[square-4] == 3) && board[square-9] == 0){
                            // this is a possible move
                            return 1;
                        }
                    }
                }

                // left capture
                if (col - 1 >= 0){
                    // forward capture
                    if (row + 2 < 10){
                        // now check whether the pawn are placed correctly or not
                        if ((board[square+5] == 2 || board[square+5] == 3) && board[square+9] == 0){
                            // this is a possible move
                            return 1;
                        }
                    }

                    // backward capture
                    if (row - 2 >= 0){
                        // now check whether the pawn are placed correctly or not
                        if ((board[square-5] == 2 || board[square-5] == 3) && board[square-11] == 0){
                            // this is a possible move
                            return 1;
                        }
                    }
                }
            }

        } else if (board[square] == 5){
            byte currow = row+1;
            byte curcol = (row % 2) ? col+1 : col;
            bool found = false;
            while (curcol < 5 && currow < 10){
                byte piece = board[currow*5+curcol];
                if (piece == 0){
                    if (currow % 2){
                        // row is odd
                        curcol++;
                    }
                    currow++;
                } else if (piece == 2 || piece == 3){
                    found = true;
                    break;
                } else {
                    // piece == 4 or piece == 5
                    break;
                }
            }
            if (found){
                if (currow % 2){
                    // currow is odd
                    curcol++;
                }
                currow++;
                while (curcol < 5 && currow < 10){
                    if (board[currow*5+curcol] == 0){
                        // this is a capture move
                        return 1;
                    } else {
                        // some piece present so capture either stops or no capture at all
                        break;
                    }
                    if (currow % 2){
                        // currow is odd
                        curcol++;
                    }
                    currow++;
                }                        
            }

            // left forward capture
            currow = row+1;
            curcol = (row % 2 == 0) ? col-1 : col;
            found = false;
            while (curcol >= 0 && currow < 10){
                byte piece = board[currow*5+curcol];
                if (piece == 0){
                    if (currow % 2 == 0){
                        // row is even
                        curcol--;
                    }
                    currow++;
                } else if (piece == 2 || piece == 3){
                    found = true;
                    break;
                } else {
                    // piece == 4 or piece == 5
                    break;
                }
            }

            if (found){
                if (currow % 2 == 0){
                    // currow is even
                    curcol--;
                }
                currow++;
                while (curcol >= 0 && currow < 10){
                    if (board[currow*5+curcol] == 0){
                        // this is a capture move
                        return 1;
                    } else {
                        // some piece present so capture either stops or no capture at all
                        break;
                    }
                    if (currow % 2 == 0){
                        // currow is even
                        curcol--;
                    }
                    currow++;
                }
            }

            // right backward capture
            currow = row-1;
            curcol = (row % 2) ? col+1 : col;
            found = false;
            while (curcol < 5 && currow >= 0){
                byte piece = board[currow*5+curcol];
                if (piece == 0){
                    if (currow % 2){
                        // row is odd
                        curcol++;
                    }
                    currow--;
                } else if (piece == 2 || piece == 3){
                    found = true;
                    break;
                } else {
                    // piece == 4 or piece == 5
                    break;
                }
            }

            if (found){
                if (currow % 2){
                    // currow is odd
                    curcol++;
                }
                currow--;
                while (curcol < 5 && currow >= 0){
                    if (board[currow*5+curcol] == 0){
                        // this is a capture move
                        return 1;
                    } else {
                        // some piece present so capture either stops or no capture at all
                        break;
                    }
                    if (currow % 2){
                        // currow is odd
                        curcol++;
                    }
                    currow--;
                }                        
            }

            // left backward capture
            currow = row-1;
            curcol = (row % 2 == 0) ? col-1 : col;
            found = false;
            while (curcol >= 0 && currow >= 0){
                byte piece = board[currow*5+curcol];
                if (piece == 0){
                    if (currow % 2 == 0){
                        // row is even
                        curcol--;
                    }
                    currow--;
                } else if (piece == 2 || piece == 3){
                    found = true;
                    break;
                } else {
                    // piece == 4 or piece == 5
                    break;
                }
            }

            if (found){
                if (currow % 2 == 0){
                    // currow is even
                    curcol--;
                }
                currow--;
                while (curcol >= 0 && currow >= 0){
                    if (board[currow*5+curcol] == 0){
                        // this is a capture move
                        return 1;
                    } else {
                        // some piece present so capture either stops or no capture at all
                        break;
                    }
                    if (currow % 2 == 0){
                        // currow is even
                        curcol--;
                    }
                    currow--;
                }
            }
        } else {
            // should not reach here
            assert(0);
        }
        return 0;
    #elif board_size == 52
        
    #elif board_size == 26
        // todo
    #else
        // should never reach here
        assert(0);
    #endif
}

std::vector<std::pair<move, position>> get_all_moves(position curpos){
    std::vector<std::pair<move, position>> moves;
    // every move can be represent using an integer
    #if board_size == 51
        if (curpos.tomove & 1){
            // white to move
            if (curpos.tomove < 0){
                // forced capture
                byte square = (curpos.tomove & 0b01111110) >> 1; // 1st to 7th bit
                byte row = square / 5;
                byte col = square % 5;
                if (curpos.board[square] == 2){
                    // the forced capture square has a white pawn on it
                    if (row % 2 == 0){
                        // row is even
                        // right capture
                        if (col + 1 < 5){
                            // forward capture
                            if (row + 2 < 10){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square+5] == 4 || curpos.board[square+5] == 5) && curpos.board[square+11] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square+11] = 2;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square+5] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square+11)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square+11) << 1) + 1;
                                    } else {
                                        aftermove.tomove = 0; // no forced captures + black to move
                                    }
                                    moves.push_back({{square, square+11}, aftermove});
                                }
                            }

                            // backward capture
                            if (row - 2 >= 0){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square-5] == 4 || curpos.board[square-5] == 5) && curpos.board[square-9] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square-9] = 2;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square-5] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square-9)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square-9) << 1) + 1;
                                    } else {
                                        aftermove.tomove = 0; // no forced captures + black to move
                                    }
                                    moves.push_back({{square, square-9}, aftermove});
                                }
                            }
                        }

                            // left capture
                        if (col - 1 >= 0){
                            // forward capture
                            if (row + 2 < 10){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square+4] == 4 || curpos.board[square+4] == 5) && curpos.board[square+9] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square+9] = 2;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square+4] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square+9)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square+9) << 1) + 1;
                                    } else {
                                        aftermove.tomove = 0; // no forced captures + black to move
                                    }
                                    moves.push_back({{square, square+9}, aftermove});
                                }
                            }

                            // backward capture
                            if (row - 2 >= 0){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square-6] == 4 || curpos.board[square-6] == 5) && curpos.board[square-11] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square-11] = 2;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square-6] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square-11)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square-11) << 1) + 1;
                                    } else {
                                        aftermove.tomove = 0; // no forced captures + black to move
                                    }
                                    moves.push_back({{square, square-11}, aftermove});
                                }
                            }
                        }

                    } else {
                        // row is odd

                        // row is even
                        // right capture
                        if (col + 1 < 5){
                            // forward capture
                            if (row + 2 < 10){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square+6] == 4 || curpos.board[square+6] == 5) && curpos.board[square+11] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square+11] = 2;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square+6] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    // this piece even if it reaches the other end might not become a king ...
                                    // ... if this piece has more captures
                                    if (has_capture(aftermove.board, square+11)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square+11) << 1) + 1;
                                    } else {
                                        if (row+2 == 9) aftermove.board[square+11] = 3; // it becomes a king
                                        aftermove.tomove = 0; // no forced captures + black to move
                                    }
                                    moves.push_back({{square, square+11}, aftermove});
                                }
                            }

                            // backward capture
                            if (row - 2 >= 0){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square-4] == 4 || curpos.board[square-4] == 5) && curpos.board[square-9] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square-9] = 2;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square-4] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square-9)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square-9) << 1) + 1;
                                    } else {
                                        aftermove.tomove = 0; // no forced captures + black to move
                                    }
                                    moves.push_back({{square, square-9}, aftermove});
                                }
                            }
                        }

                        // left capture
                        if (col - 1 >= 0){
                            // forward capture
                            if (row + 2 < 10){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square+5] == 4 || curpos.board[square+5] == 5) && curpos.board[square+9] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square+9] = 2;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square+5] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    // this piece even if it reaches the other end might not become a king ...
                                    // ... if this piece has more captures
                                    if (has_capture(aftermove.board, square+9)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square+9) << 1) + 1;
                                    } else {
                                        if (row+2 == 9) aftermove.board[square+9] = 3; // it becomes a king
                                        aftermove.tomove = 0; // no forced captures + black to move
                                    }
                                    moves.push_back({{square, square+9}, aftermove});
                                }
                            }

                            // backward capture
                            if (row - 2 >= 0){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square-5] == 4 || curpos.board[square-5] == 5) && curpos.board[square-11] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square-11] = 2;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square-5] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square-11)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square-11) << 1) + 1;
                                    } else {
                                        aftermove.tomove = 0; // no forced captures + black to move
                                    }
                                    moves.push_back({{square, square-11}, aftermove});
                                }
                            }
                        }
                    }

                } else if (curpos.board[square] == 3){
                    // the forces capture square has a king white pawn on it
                    
                    // right forward capture
                    byte currow = row+1;
                    byte curcol = (row % 2) ? col+1 : col;
                    bool found = false;
                    while (curcol < 5 && currow < 10){
                        byte piece = curpos.board[currow*5+curcol];
                        if (piece == 0){
                            // move along the diagonal
                            if (currow % 2){
                                // row is odd
                                curcol++;
                            }
                            currow++;
                        } else if (piece == 4 || piece == 5){
                            found = true;
                            break;
                        } else {
                            // piece == 2 or piece == 3
                            break;
                        }
                    }

                    byte capture_square = currow*5+curcol;

                    if (found){
                        if (currow % 2){
                            // currow is odd
                            curcol++;
                        }
                        currow++;
                        while (curcol < 5 && currow < 10){
                            if (curpos.board[currow*5+curcol] == 0){
                                // this is a capture move
                                position aftermove = curpos;
                                aftermove.board[currow*5+curcol] = 3;
                                aftermove.board[square] = 0;
                                aftermove.board[capture_square] = 0;
                                if (has_capture(aftermove.board, currow*5+curcol)){
                                    aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1) + 1;
                                    // forced capture and it's white's chance
                                } else {
                                    aftermove.tomove = 0; // black's chance
                                }
                                moves.push_back({{square, currow*5+curcol}, aftermove});
                            } else {
                                // some piece present so capture either stops or no capture at all
                                break;
                            }
                            // move along the diagonal
                            if (currow % 2){
                                // currow is odd
                                curcol++;
                            }
                            currow++;
                        }                        
                    }

                    // left forward capture
                    currow = row+1;
                    curcol = (row % 2 == 0) ? col-1 : col;
                    found = false;
                    while (curcol >= 0 && currow < 10){
                        byte piece = curpos.board[currow*5+curcol];
                        if (piece == 0){
                            if (currow % 2 == 0){
                                // row is even
                                curcol--;
                            }
                            currow++;
                        } else if (piece == 4 || piece == 5){
                            found = true;
                            break;
                        } else {
                            // piece == 2 or piece == 3
                            break;
                        }
                    }

                    capture_square = currow*5+curcol;

                    if (found){
                        if (currow % 2 == 0){
                            // currow is even
                            curcol--;
                        }
                        currow++;
                        while (curcol >= 0 && currow < 10){
                            if (curpos.board[currow*5+curcol] == 0){
                                // this is a capture move
                                position aftermove = curpos;
                                aftermove.board[currow*5+curcol] = 3;
                                aftermove.board[square] = 0;
                                aftermove.board[capture_square] = 0;
                                if (has_capture(aftermove.board, currow*5+curcol)){
                                    aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1) + 1;
                                    // forced capture and it's white's chance
                                } else {
                                    aftermove.tomove = 0; // black's chance
                                }
                                moves.push_back({{square, currow*5+curcol}, aftermove});
                            } else {
                                // some piece present so capture either stops or no capture at all
                                break;
                            }
                            if (currow % 2 == 0){
                                // currow is even
                                curcol--;
                            }
                            currow++;
                        }
                    }

                    // right backward capture
                    currow = row-1;
                    curcol = (row % 2) ? col+1 : col;
                    found = false;
                    while (curcol < 5 && currow >= 0){
                        byte piece = curpos.board[currow*5+curcol];
                        if (piece == 0){
                            if (currow % 2){
                                // row is odd
                                curcol++;
                            }
                            currow--;
                        } else if (piece == 4 || piece == 5){
                            found = true;
                            break;
                        } else {
                            // piece == 2 or piece == 3
                            break;
                        }
                    }

                    capture_square = currow*5+curcol;

                    if (found){
                        if (currow % 2){
                            // currow is odd
                            curcol++;
                        }
                        currow--;
                        while (curcol < 5 && currow >= 0){
                            if (curpos.board[currow*5+curcol] == 0){
                                // this is a capture move
                                position aftermove = curpos;
                                aftermove.board[currow*5+curcol] = 3;
                                aftermove.board[square] = 0;
                                aftermove.board[capture_square] = 0;
                                if (has_capture(aftermove.board, currow*5+curcol)){
                                    aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1) + 1;
                                    // forced capture and it's white's chance
                                } else {
                                    aftermove.tomove = 0; // black's chance
                                }
                                moves.push_back({{square, currow*5+curcol}, aftermove});
                            } else {
                                // some piece present so capture either stops or no capture at all
                                break;
                            }
                            if (currow % 2){
                                // currow is odd
                                curcol++;
                            }
                            currow--;
                        }                        
                    }

                    // left backward capture
                    currow = row-1;
                    curcol = (row % 2 == 0) ? col-1 : col;
                    found = false;
                    while (curcol >= 0 && currow >= 0){
                        byte piece = curpos.board[currow*5+curcol];
                        if (piece == 0){
                            if (currow % 2 == 0){
                                // row is even
                                curcol--;
                            }
                            currow--;
                        } else if (piece == 4 || piece == 5){
                            found = true;
                            break;
                        } else {
                            // piece == 2 or piece == 3
                            break;
                        }
                    }

                    capture_square = currow*5+curcol;

                    if (found){
                        if (currow % 2 == 0){
                            // currow is even
                            curcol--;
                        }
                        currow--;
                        while (curcol >= 0 && currow >= 0){
                            if (curpos.board[currow*5+curcol] == 0){
                                // this is a capture move
                                position aftermove = curpos;
                                aftermove.board[currow*5+curcol] = 3;
                                aftermove.board[square] = 0;
                                aftermove.board[capture_square] = 0;
                                if (has_capture(aftermove.board, currow*5+curcol)){
                                    aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1) + 1;
                                    // forced capture and it's white's chance
                                } else {
                                    aftermove.tomove = 0; // black's chance
                                }
                                moves.push_back({{square, currow*5+curcol}, aftermove});
                            } else {
                                // some piece present so capture either stops or no capture at all
                                break;
                            }
                            if (currow % 2 == 0){
                                // currow is even
                                curcol--;
                            }
                            currow--;
                        }
                    }

                } else {
                    // should not reach here
                    // the forced capture square is not occupied by white itself (to move is white)
                    assert(0);
                }
            } else {
                // firstly check for captures, if any, limit to them
                // iterate over every white piece
                bool does_have_captures = false;
                // since if any capture found we have to capture the piece no other option for movement
                for (byte square=0; square<50; ++square){
                    byte row = square / 5;
                    byte col = square % 5;
                    if (curpos.board[square] == 2){
                        // the forced capture square has a white pawn on it
                        if (row % 2 == 0){
                            // row is even
                            // right capture
                            if (col + 1 < 5){
                                // forward capture
                                if (row + 2 < 10){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square+5] == 4 || curpos.board[square+5] == 5) && curpos.board[square+11] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square+11] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square+5] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square+11)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square+11) << 1) + 1;
                                        } else {
                                            aftermove.tomove = 0; // no forced captures + black to move
                                        }
                                        moves.push_back({{square, square+11}, aftermove});
                                    }
                                }

                                // backward capture
                                if (row - 2 >= 0){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square-5] == 4 || curpos.board[square-5] == 5) && curpos.board[square-9] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square-9] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square-5] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square-9)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square-9) << 1) + 1;
                                        } else {
                                            aftermove.tomove = 0; // no forced captures + black to move
                                        }
                                        moves.push_back({{square, square-9}, aftermove});
                                    }
                                }
                            }

                                // left capture
                            if (col - 1 >= 0){
                                // forward capture
                                if (row + 2 < 10){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square+4] == 4 || curpos.board[square+4] == 5) && curpos.board[square+9] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square+9] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square+4] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square+9)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square+9) << 1) + 1;
                                        } else {
                                            aftermove.tomove = 0; // no forced captures + black to move
                                        }
                                        moves.push_back({{square, square+9}, aftermove});
                                    }
                                }

                                // backward capture
                                if (row - 2 >= 0){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square-6] == 4 || curpos.board[square-6] == 5) && curpos.board[square-11] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square-11] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square-6] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square-11)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square-11) << 1) + 1;
                                        } else {
                                            aftermove.tomove = 0; // no forced captures + black to move
                                        }
                                        moves.push_back({{square, square-11}, aftermove});
                                    }
                                }
                            }

                        } else {
                            // row is odd

                            // row is even
                            // right capture
                            if (col + 1 < 5){
                                // forward capture
                                if (row + 2 < 10){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square+6] == 4 || curpos.board[square+6] == 5) && curpos.board[square+11] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square+11] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square+6] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        // this piece even if it reaches the other end might not become a king ...
                                        // ... if this piece has more captures
                                        if (has_capture(aftermove.board, square+11)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square+11) << 1) + 1;
                                        } else {
                                            if (row+2 == 9) aftermove.board[square+11] = 3; // it becomes a king
                                            aftermove.tomove = 0; // no forced captures + black to move
                                        }
                                        moves.push_back({{square, square+11}, aftermove});
                                    }
                                }

                                // backward capture
                                if (row - 2 >= 0){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square-4] == 4 || curpos.board[square-4] == 5) && curpos.board[square-9] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square-9] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square-4] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square-9)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square-9) << 1) + 1;
                                        } else {
                                            aftermove.tomove = 0; // no forced captures + black to move
                                        }
                                        moves.push_back({{square, square-9}, aftermove});
                                    }
                                }
                            }

                            // left capture
                            if (col - 1 >= 0){
                                // forward capture
                                if (row + 2 < 10){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square+5] == 4 || curpos.board[square+5] == 5) && curpos.board[square+9] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square+9] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square+5] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        // this piece even if it reaches the other end might not become a king ...
                                        // ... if this piece has more captures
                                        if (has_capture(aftermove.board, square+9)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square+9) << 1) + 1;
                                        } else {
                                            if (row+2 == 9) aftermove.board[square+9] = 3; // it becomes a king
                                            aftermove.tomove = 0; // no forced captures + black to move
                                        }
                                        moves.push_back({{square, square+9}, aftermove});
                                    }
                                }

                                // backward capture
                                if (row - 2 >= 0){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square-5] == 4 || curpos.board[square-5] == 5) && curpos.board[square-11] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square-11] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square-5] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square-11)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square-11) << 1) + 1;
                                        } else {
                                            aftermove.tomove = 0; // no forced captures + black to move
                                        }
                                        moves.push_back({{square, square-11}, aftermove});
                                    }
                                }
                            }
                        }

                    } else if (curpos.board[square] == 3){
                        // the forces capture square has a king white pawn on it
                        
                        // right forward capture
                        byte currow = row+1;
                        byte curcol = (row % 2) ? col+1 : col;
                        bool found = false;
                        while (curcol < 5 && currow < 10){
                            byte piece = curpos.board[currow*5+curcol];
                            if (piece == 0){
                                if (currow % 2){
                                    // row is odd
                                    curcol++;
                                }
                                currow++;
                            } else if (piece == 4 || piece == 5){
                                found = true;
                                break;
                            } else {
                                // piece == 2 or piece == 3
                                break;
                            }
                        }

                        byte capture_square = currow*5+curcol;

                        if (found){
                            if (currow % 2){
                                // currow is odd
                                curcol++;
                            }
                            currow++;
                            while (curcol < 5 && currow < 10){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a capture move
                                    does_have_captures = true;
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 3;
                                    aftermove.board[square] = 0;
                                    aftermove.board[capture_square] = 0;
                                    if (has_capture(aftermove.board, currow*5+curcol)){
                                        aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1) + 1;
                                        // forced capture and it's white's chance
                                    } else {
                                        aftermove.tomove = 0; // black's chance
                                    }
                                    moves.push_back({{square, currow*5+curcol}, aftermove});
                                } else {
                                    // some piece present so capture either stops or no capture at all
                                    break;
                                }
                                if (currow % 2){
                                    // currow is odd
                                    curcol++;
                                }
                                currow++;
                            }                        
                        }

                        // left forward capture
                        currow = row+1;
                        curcol = (row % 2 == 0) ? col-1 : col;
                        found = false;
                        while (curcol >= 0 && currow < 10){
                            byte piece = curpos.board[currow*5+curcol];
                            if (piece == 0){
                                if (currow % 2 == 0){
                                    // row is even
                                    curcol--;
                                }
                                currow++;
                            } else if (piece == 4 || piece == 5){
                                found = true;
                                break;
                            } else {
                                // piece == 2 or piece == 3
                                break;
                            }
                        }

                        capture_square = currow*5+curcol;

                        if (found){
                            if (currow % 2 == 0){
                                // currow is even
                                curcol--;
                            }
                            currow++;
                            while (curcol >= 0 && currow < 10){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a capture move
                                    does_have_captures = true;
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 3;
                                    aftermove.board[square] = 0;
                                    aftermove.board[capture_square] = 0;
                                    if (has_capture(aftermove.board, currow*5+curcol)){
                                        aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1) + 1;
                                        // forced capture and it's white's chance
                                    } else {
                                        aftermove.tomove = 0; // black's chance
                                    }
                                    moves.push_back({{square, currow*5+curcol}, aftermove});
                                } else {
                                    // some piece present so capture either stops or no capture at all
                                    break;
                                }
                                if (currow % 2 == 0){
                                    // currow is even
                                    curcol--;
                                }
                                currow++;
                            }
                        }

                        // right backward capture
                        currow = row-1;
                        curcol = (row % 2) ? col+1 : col;
                        found = false;
                        while (curcol < 5 && currow >= 0){
                            byte piece = curpos.board[currow*5+curcol];
                            if (piece == 0){
                                if (currow % 2){
                                    // row is odd
                                    curcol++;
                                }
                                currow--;
                            } else if (piece == 4 || piece == 5){
                                found = true;
                                break;
                            } else {
                                // piece == 2 or piece == 3
                                break;
                            }
                        }

                        capture_square = currow*5+curcol;

                        if (found){
                            if (currow % 2){
                                // currow is odd
                                curcol++;
                            }
                            currow--;
                            while (curcol < 5 && currow >= 0){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a capture move
                                    does_have_captures = true;
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 3;
                                    aftermove.board[square] = 0;
                                    aftermove.board[capture_square] = 0;
                                    if (has_capture(aftermove.board, currow*5+curcol)){
                                        aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1) + 1;
                                        // forced capture and it's white's chance
                                    } else {
                                        aftermove.tomove = 0; // black's chance
                                    }
                                    moves.push_back({{square, currow*5+curcol}, aftermove});
                                } else {
                                    // some piece present so capture either stops or no capture at all
                                    break;
                                }
                                if (currow % 2){
                                    // currow is odd
                                    curcol++;
                                }
                                currow--;
                            }                        
                        }

                        // left backward capture
                        currow = row-1;
                        curcol = (row % 2 == 0) ? col-1 : col;
                        found = false;
                        while (curcol >= 0 && currow >= 0){
                            byte piece = curpos.board[currow*5+curcol];
                            if (piece == 0){
                                if (currow % 2 == 0){
                                    // row is even
                                    curcol--;
                                }
                                currow--;
                            } else if (piece == 4 || piece == 5){
                                found = true;
                                break;
                            } else {
                                // piece == 2 or piece == 3
                                break;
                            }
                        }

                        capture_square = currow*5+curcol;

                        if (found){
                            if (currow % 2 == 0){
                                // currow is even
                                curcol--;
                            }
                            currow--;
                            while (curcol >= 0 && currow >= 0){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a capture move
                                    does_have_captures = true;
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 3;
                                    aftermove.board[square] = 0;
                                    aftermove.board[capture_square] = 0;
                                    if (has_capture(aftermove.board, currow*5+curcol)){
                                        aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1) + 1;
                                        // forced capture and it's white's chance
                                    } else {
                                        aftermove.tomove = 0; // black's chance
                                    }
                                    moves.push_back({{square, currow*5+curcol}, aftermove});
                                } else {
                                    // some piece present so capture either stops or no capture at all
                                    break;
                                }
                                if (currow % 2 == 0){
                                    // currow is even
                                    curcol--;
                                }
                                currow--;
                            }
                        }

                    }
                }
                
                if (!does_have_captures){
                    // it does not have any captures
                    // normal moves
                    for (byte square=0; square<50; ++square){
                        if (curpos.board[square] == 2){
                            // white pawn on this square
                            byte row = square / 5;
                            byte col = square % 5;
                            if (row % 2 == 0){
                                // row is even

                                // right moves
                                // right forward move
                                if (row+1 < 10){
                                    if (curpos.board[square+5] == 0){
                                        // empty place can move to this square
                                        position aftermove = curpos;
                                        if (row+1 == 9) aftermove.board[square+5] = 3;
                                        else aftermove.board[square+5] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.tomove = 0; // black to move
                                        moves.push_back({{square, square+5}, aftermove});
                                    }
                                }

                                // left moves
                                // left forward move
                                if (row+1 < 10 && col-1 >= 0){
                                    if (curpos.board[square+4] == 0){
                                        // empty place can move to this square
                                        position aftermove = curpos;
                                        if (row+1 == 9) aftermove.board[square+4] = 3;
                                        else aftermove.board[square+4] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.tomove = 0; // black to move
                                        moves.push_back({{square, square+4}, aftermove});
                                    }
                                }
                            } else {
                                // row is odd

                                // right moves
                                // right forward move
                                if (row+1 < 10 && col+1 < 5){
                                    if (curpos.board[square+6] == 0){
                                        // empty place can move to this square
                                        position aftermove = curpos;
                                        aftermove.board[square+6] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.tomove = 0; // black to move
                                        moves.push_back({{square, square+6}, aftermove});
                                    }
                                }

                                // left moves
                                // left forward move
                                if (row+1 < 10){
                                    if (curpos.board[square+5] == 0){
                                        // empty place can move to this square
                                        position aftermove = curpos;
                                        aftermove.board[square+5] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.tomove = 0; // black to move
                                        moves.push_back({{square, square+5}, aftermove});
                                    }
                                }
                            }
                        } else if (curpos.board[square] == 3){
                            // white's king pawn
                            
                            // it can go in all four directions and it is ...
                            // ... guaranteed that there is no capture move
                            byte row = square / 5;
                            byte col = square % 5;

                            // right forward
                            byte currow = row+1;
                            byte curcol = (row % 2) ? col+1 : col;
                            while (curcol < 5 && currow < 10){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 3;
                                    aftermove.board[square] = 0;
                                    aftermove.tomove = 0; // black to move
                                    moves.push_back({{square, currow*5+curcol}, aftermove});
                                } else {
                                    // piece == 2 or piece == 3 or piece == 4 or piece == 5
                                    break;
                                }
                                if (currow % 2){
                                    // row is odd
                                    curcol++;
                                }
                                currow++;
                            }

                            // right backward
                            currow = row-1;
                            curcol = (row % 2) ? col+1 : col;
                            while (curcol < 5 && currow >= 0){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 3;
                                    aftermove.board[square] = 0;
                                    aftermove.tomove = 0; // black to move
                                    moves.push_back({{square, currow*5+curcol}, aftermove});
                                } else {
                                    // piece == 2 or piece == 3
                                    break;
                                }
                                if (currow % 2){
                                    // row is odd
                                    curcol++;
                                }
                                currow--;
                            }

                            // left forward
                            currow = row+1;
                            curcol = (row % 2 == 0) ? col-1 : col;
                            while (curcol >= 0 && currow < 10){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 3;
                                    aftermove.board[square] = 0;
                                    aftermove.tomove = 0; // black to move
                                    moves.push_back({{square, currow*5+curcol}, aftermove});
                                } else {
                                    // piece == 2 or piece == 3
                                    break;
                                }
                                if (currow % 2 == 0){
                                    // row is even
                                    curcol--;
                                }
                                currow++;
                            }

                            // left backward
                            currow = row-1;
                            curcol = (row % 2 == 0) ? col-1 : col;
                            while (curcol >= 0 && currow >= 0){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 3;
                                    aftermove.board[square] = 0;
                                    aftermove.tomove = 0; // black to move
                                    moves.push_back({{square, currow*5+curcol}, aftermove});
                                } else {
                                    // piece == 2 or piece == 3
                                    break;
                                }
                                if (currow % 2 == 0){
                                    // row is even
                                    curcol--;
                                }
                                currow--;
                            }
                        }
                    }
                }
            }

        } else {
            // black to move
            if (curpos.tomove < 0){
                // forced capture
                byte square = (curpos.tomove & 0b01111110) >> 1; // 1st to 7th bit
                byte row = square / 5;
                byte col = square % 5;
                if (curpos.board[square] == 4){
                    // the forced capture square has a black pawn on it
                    if (row % 2 == 0){
                        // row is even
                        // right capture
                        if (col + 1 < 5){
                            // forward capture
                            if (row + 2 < 10){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square+5] == 2 || curpos.board[square+5] == 3) && curpos.board[square+11] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square+11] = 4;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square+5] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square+11)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square+11) << 1);
                                    } else {
                                        aftermove.tomove = 1; // no forced captures + white to move
                                    }
                                    moves.push_back({{square, square+11}, aftermove});
                                }
                            }

                            // backward capture
                            if (row - 2 >= 0){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square-5] == 2 || curpos.board[square-5] == 3) && curpos.board[square-9] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square-9] = 4;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square-5] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    // this piece even if it reaches the other end might not become a king ...
                                    // ... if this piece has more captures
                                    if (has_capture(aftermove.board, square-9)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square-9) << 1);
                                    } else {
                                        if (row-2 == 0) aftermove.board[square-9] = 5;
                                        aftermove.tomove = 1; // no forced captures + white to move
                                    }
                                    moves.push_back({{square, square-9}, aftermove});
                                }
                            }
                        }

                        // left capture
                        if (col - 1 >= 0){
                            // forward capture
                            if (row + 2 < 10){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square+4] == 2 || curpos.board[square+4] == 3) && curpos.board[square+9] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square+9] = 4;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square+4] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square+9)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square+9) << 1);
                                    } else {
                                        aftermove.tomove = 1; // no forced captures + white to move
                                    }
                                    moves.push_back({{square, square+9}, aftermove});
                                }
                            }

                            // backward capture
                            if (row - 2 >= 0){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square-6] == 2 || curpos.board[square-6] == 3) && curpos.board[square-11] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square-11] = 4;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square-6] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    // this piece even if it reaches the other end might not become a king ...
                                    // ... if this piece has more captures
                                    if (has_capture(aftermove.board, square-11)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square-11) << 1);
                                    } else {
                                        if (row-2 == 0) aftermove.board[square-11] = 5;
                                        aftermove.tomove = 1; // no forced captures + white to move
                                    }
                                    moves.push_back({{square, square-11}, aftermove});
                                }
                            }
                        }

                    } else {
                        // row is odd

                        // row is even
                        // right capture
                        if (col + 1 < 5){
                            // forward capture
                            if (row + 2 < 10){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square+6] == 2 || curpos.board[square+6] == 3) && curpos.board[square+11] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square+11] = 4;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square+6] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square+11)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square+11) << 1);
                                    } else {
                                        aftermove.tomove = 1; // no forced captures + white to move
                                    }
                                    moves.push_back({{square, square+11}, aftermove});
                                }
                            }

                            // backward capture
                            if (row - 2 >= 0){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square-4] == 2 || curpos.board[square-4] == 3) && curpos.board[square-9] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square-9] = 4;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square-4] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square-9)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square-9) << 1);
                                    } else {
                                        aftermove.tomove = 1; // no forced captures + white to move
                                    }
                                    moves.push_back({{square, square-9}, aftermove});
                                }
                            }
                        }

                        // left capture
                        if (col - 1 >= 0){
                            // forward capture
                            if (row + 2 < 10){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square+5] == 2 || curpos.board[square+5] == 3) && curpos.board[square+9] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square+9] = 4;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square+5] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square+9)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square+9) << 1);
                                    } else {
                                        aftermove.tomove = 1; // no forced captures + white to move
                                    }
                                    moves.push_back({{square, square+9}, aftermove});
                                }
                            }

                            // backward capture
                            if (row - 2 >= 0){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square-5] == 2 || curpos.board[square-5] == 3) && curpos.board[square-11] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square-11] = 4;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square-5] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square-11)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square-11) << 1);
                                    } else {
                                        aftermove.tomove = 1; // no forced captures + white to move
                                    }
                                    moves.push_back({{square, square-11}, aftermove});
                                }
                            }
                        }
                    }

                } else if (curpos.board[square] == 5){
                    // the forces capture square has a king black pawn on it
                    
                    // right forward capture
                    byte currow = row+1;
                    byte curcol = (row % 2) ? col+1 : col;
                    bool found = false;
                    while (curcol < 5 && currow < 10){
                        byte piece = curpos.board[currow*5+curcol];
                        if (piece == 0){
                            if (currow % 2){
                                // row is odd
                                curcol++;
                            }
                            currow++;
                        } else if (piece == 2 || piece == 3){
                            found = true;
                            break;
                        } else {
                            // piece == 4 or piece == 5
                            break;
                        }
                    }

                    byte capture_square = currow*5+curcol;

                    if (found){
                        if (currow % 2){
                            // currow is odd
                            curcol++;
                        }
                        currow++;
                        while (curcol < 5 && currow < 10){
                            if (curpos.board[currow*5+curcol] == 0){
                                // this is a capture move
                                position aftermove = curpos;
                                aftermove.board[currow*5+curcol] = 5;
                                aftermove.board[square] = 0;
                                aftermove.board[capture_square] = 0;
                                if (has_capture(aftermove.board, currow*5+curcol)){
                                    aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1);
                                    // forced capture and it's black's chance
                                } else {
                                    aftermove.tomove = 1; // white's chance
                                }
                                moves.push_back({{square, currow*5+curcol}, aftermove});
                            } else {
                                // some piece present so capture either stops or no capture at all
                                break;
                            }
                            if (currow % 2){
                                // currow is odd
                                curcol++;
                            }
                            currow++;
                        }                        
                    }

                    // left forward capture
                    currow = row+1;
                    curcol = (row % 2 == 0) ? col-1 : col;
                    found = false;
                    while (curcol >= 0 && currow < 10){
                        byte piece = curpos.board[currow*5+curcol];
                        if (piece == 0){
                            if (currow % 2 == 0){
                                // row is even
                                curcol--;
                            }
                            currow++;
                        } else if (piece == 2 || piece == 3){
                            found = true;
                            break;
                        } else {
                            // piece == 4 or piece == 5
                            break;
                        }
                    }

                    capture_square = currow*5+curcol;

                    if (found){
                        if (currow % 2 == 0){
                            // currow is even
                            curcol--;
                        }
                        currow++;
                        while (curcol >= 0 && currow < 10){
                            if (curpos.board[currow*5+curcol] == 0){
                                // this is a capture move
                                position aftermove = curpos;
                                aftermove.board[currow*5+curcol] = 5;
                                aftermove.board[square] = 0;
                                aftermove.board[capture_square] = 0;
                                if (has_capture(aftermove.board, currow*5+curcol)){
                                    aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1);
                                    // forced capture and it's black's chance
                                } else {
                                    aftermove.tomove = 1; // white's chance
                                }
                                moves.push_back({{square, currow*5+curcol}, aftermove});
                            } else {
                                // some piece present so capture either stops or no capture at all
                                break;
                            }
                            if (currow % 2 == 0){
                                // currow is even
                                curcol--;
                            }
                            currow++;
                        }
                    }

                    // right backward capture
                    currow = row-1;
                    curcol = (row % 2) ? col+1 : col;
                    found = false;
                    while (curcol < 5 && currow >= 0){
                        byte piece = curpos.board[currow*5+curcol];
                        if (piece == 0){
                            if (currow % 2){
                            // row is odd
                                curcol++;
                            }
                            currow--;
                        } else if (piece == 2 || piece == 3){
                            found = true;
                            break;
                        } else {
                            // piece == 4 or piece == 5
                            break;
                        }
                    }

                    capture_square = currow*5+curcol;

                    if (found){
                        if (currow % 2){
                            // currow is odd
                            curcol++;
                        }
                        currow--;
                        while (curcol < 5 && currow >= 0){
                            if (curpos.board[currow*5+curcol] == 0){
                                // this is a capture move
                                position aftermove = curpos;
                                aftermove.board[currow*5+curcol] = 5;
                                aftermove.board[square] = 0;
                                aftermove.board[capture_square] = 0;
                                if (has_capture(aftermove.board, currow*5+curcol)){
                                    aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1);
                                    // forced capture and it's black's chance
                                } else {
                                    aftermove.tomove = 1; // white's chance
                                }
                                moves.push_back({{square, currow*5+curcol}, aftermove});
                            } else {
                                // some piece present so capture either stops or no capture at all
                                break;
                            }
                            if (currow % 2){
                                // currow is odd
                                curcol++;
                            }
                            currow--;
                        }                        
                    }

                    // left backward capture
                    currow = row-1;
                    curcol = (row % 2 == 0) ? col-1 : col;
                    found = false;
                    while (curcol >= 0 && currow >= 0){
                        byte piece = curpos.board[currow*5+curcol];
                        if (piece == 0){
                            if (currow % 2 == 0){
                                // row is even
                                curcol--;
                            }
                            currow--;
                        } else if (piece == 2 || piece == 3){
                            found = true;
                            break;
                        } else {
                            // piece == 4 or piece == 5
                            break;
                        }
                    }

                    capture_square = currow*5+curcol;

                    if (found){
                        if (currow % 2 == 0){
                            // currow is even
                            curcol--;
                        }
                        currow--;
                        while (curcol >= 0 && currow >= 0){
                            if (curpos.board[currow*5+curcol] == 0){
                                // this is a capture move
                                position aftermove = curpos;
                                aftermove.board[currow*5+curcol] = 5;
                                aftermove.board[square] = 0;
                                aftermove.board[capture_square] = 0;
                                if (has_capture(aftermove.board, currow*5+curcol)){
                                    aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1);
                                    // forced capture and it's black's chance
                                } else {
                                    aftermove.tomove = 1; // white's chance
                                }
                                moves.push_back({{square, currow*5+curcol}, aftermove});
                            } else {
                                // some piece present so capture either stops or no capture at all
                                break;
                            }
                            if (currow % 2 == 0){
                                // currow is even
                                curcol--;
                            }
                            currow--;
                        }
                    }

                } else {
                    // should not reach here
                    // the forced capture square is not occupied by white itself (to move is white)
                    assert(0);
                }
            } else {
                // firstly check for captures, if any, limit to them
                // iterate over every black piece
                bool does_have_captures = false;
                // since if any capture found we have to capture the piece no other option for movement
                for (byte square=0; square<50; ++square){
                    byte row = square / 5;
                    byte col = square % 5;
                    if (curpos.board[square] == 4){
                        // the forced capture square has a black pawn on it
                        if (row % 2 == 0){
                            // row is even
                            // right capture
                            if (col + 1 < 5){
                                // forward capture
                                if (row + 2 < 10){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square+5] == 2 || curpos.board[square+5] == 3) && curpos.board[square+11] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square+11] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square+5] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square+11)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square+11) << 1);
                                        } else {
                                            aftermove.tomove = 1; // no forced captures + white to move
                                        }
                                        moves.push_back({{square, square+11}, aftermove});
                                    }
                                }

                                // backward capture
                                if (row - 2 >= 0){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square-5] == 2 || curpos.board[square-5] == 3) && curpos.board[square-9] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square-9] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square-5] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        // this piece even if it reaches the other end might not become a king ...
                                        // ... if this piece has more captures
                                        if (has_capture(aftermove.board, square-9)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square-9) << 1);
                                        } else {
                                            if (row-2 == 0) aftermove.board[square-9] = 5;
                                            aftermove.tomove = 1; // no forced captures + white to move
                                        }
                                        moves.push_back({{square, square-9}, aftermove});
                                    }
                                }
                            }

                            // left capture
                            if (col - 1 >= 0){
                                // forward capture
                                if (row + 2 < 10){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square+4] == 2 || curpos.board[square+4] == 3) && curpos.board[square+9] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square+9] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square+4] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square+9)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square+9) << 1);
                                        } else {
                                            aftermove.tomove = 1; // no forced captures + white to move
                                        }
                                        moves.push_back({{square, square+9}, aftermove});
                                    }
                                }

                                // backward capture
                                if (row - 2 >= 0){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square-6] == 2 || curpos.board[square-6] == 3) && curpos.board[square-11] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square-11] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square-6] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        // this piece even if it reaches the other end might not become a king ...
                                        // ... if this piece has more captures
                                        if (has_capture(aftermove.board, square-11)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square-11) << 1);
                                        } else {
                                            if (row-2 == 0) aftermove.board[square-11] = 5;
                                            aftermove.tomove = 1; // no forced captures + white to move
                                        }
                                        moves.push_back({{square, square-11}, aftermove});
                                    }
                                }
                            }

                        } else {
                            // row is odd

                            // row is even
                            // right capture
                            if (col + 1 < 5){
                                // forward capture
                                if (row + 2 < 10){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square+6] == 2 || curpos.board[square+6] == 3) && curpos.board[square+11] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square+11] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square+6] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square+11)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square+11) << 1);
                                        } else {
                                            aftermove.tomove = 1; // no forced captures + white to move
                                        }
                                        moves.push_back({{square, square+11}, aftermove});
                                    }
                                }

                                // backward capture
                                if (row - 2 >= 0){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square-4] == 2 || curpos.board[square-4] == 3) && curpos.board[square-9] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square-9] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square-4] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square-9)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square-9) << 1);
                                        } else {
                                            aftermove.tomove = 1; // no forced captures + white to move
                                        }
                                        moves.push_back({{square, square-9}, aftermove});
                                    }
                                }
                            }

                            // left capture
                            if (col - 1 >= 0){
                                // forward capture
                                if (row + 2 < 10){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square+5] == 2 || curpos.board[square+5] == 3) && curpos.board[square+9] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square+9] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square+5] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square+9)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square+9) << 1);
                                        } else {
                                            aftermove.tomove = 1; // no forced captures + white to move
                                        }
                                        moves.push_back({{square, square+9}, aftermove});
                                    }
                                }

                                // backward capture
                                if (row - 2 >= 0){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square-5] == 2 || curpos.board[square-5] == 3) && curpos.board[square-11] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square-11] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square-5] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square-11)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square-11) << 1);
                                        } else {
                                            aftermove.tomove = 1; // no forced captures + white to move
                                        }
                                        moves.push_back({{square, square-11}, aftermove});
                                    }
                                }
                            }
                        }

                    } else if (curpos.board[square] == 5){
                        // the forces capture square has a king black pawn on it
                        
                        // right forward capture
                        byte currow = row+1;
                        byte curcol = (row % 2) ? col+1 : col;
                        bool found = false;
                        while (curcol < 5 && currow < 10){
                            byte piece = curpos.board[currow*5+curcol];
                            if (piece == 0){
                                if (currow % 2){
                                    // row is odd
                                    curcol++;
                                }
                                currow++;
                            } else if (piece == 2 || piece == 3){
                                found = true;
                                break;
                            } else {
                                // piece == 4 or piece == 5
                                break;
                            }
                        }

                        byte capture_square = currow*5+curcol;

                        if (found){
                            if (currow % 2){
                                // currow is odd
                                curcol++;
                            }
                            currow++;
                            while (curcol < 5 && currow < 10){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a capture move
                                    does_have_captures = true;
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 5;
                                    aftermove.board[square] = 0;
                                    aftermove.board[capture_square] = 0;
                                    if (has_capture(aftermove.board, currow*5+curcol)){
                                        aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1);
                                        // forced capture and it's black's chance
                                    } else {
                                        aftermove.tomove = 1; // white's chance
                                    }
                                    moves.push_back({{square, currow*5+curcol}, aftermove});
                                } else {
                                    // some piece present so capture either stops or no capture at all
                                    break;
                                }
                                if (currow % 2){
                                    // currow is odd
                                    curcol++;
                                }
                                currow++;
                            }                        
                        }

                        // left forward capture
                        currow = row+1;
                        curcol = (row % 2 == 0) ? col-1 : col;
                        found = false;
                        while (curcol >= 0 && currow < 10){
                            byte piece = curpos.board[currow*5+curcol];
                            if (piece == 0){
                                if (currow % 2 == 0){
                                    // row is even
                                    curcol--;
                                }
                                currow++;
                            } else if (piece == 2 || piece == 3){
                                found = true;
                                break;
                            } else {
                                // piece == 4 or piece == 5
                                break;
                            }
                        }

                        capture_square = currow*5+curcol;

                        if (found){
                            if (currow % 2 == 0){
                                // currow is even
                                curcol--;
                            }
                            currow++;
                            while (curcol >= 0 && currow < 10){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a capture move
                                    does_have_captures = true;
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 5;
                                    aftermove.board[square] = 0;
                                    aftermove.board[capture_square] = 0;
                                    if (has_capture(aftermove.board, currow*5+curcol)){
                                        aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1);
                                        // forced capture and it's black's chance
                                    } else {
                                        aftermove.tomove = 1; // white's chance
                                    }
                                    moves.push_back({{square, currow*5+curcol}, aftermove});
                                } else {
                                    // some piece present so capture either stops or no capture at all
                                    break;
                                }
                                if (currow % 2 == 0){
                                    // currow is even
                                    curcol--;
                                }
                                currow++;
                            }
                        }

                        // right backward capture
                        currow = row-1;
                        curcol = (row % 2) ? col+1 : col;
                        found = false;
                        while (curcol < 5 && currow >= 0){
                            byte piece = curpos.board[currow*5+curcol];
                            if (piece == 0){
                                if (currow % 2){
                                    // row is odd
                                    curcol++;
                                }
                                currow--;
                            } else if (piece == 2 || piece == 3){
                                found = true;
                                break;
                            } else {
                                // piece == 4 or piece == 5
                                break;
                            }
                        }

                        capture_square = currow*5+curcol;

                        if (found){
                            if (currow % 2){
                                // currow is odd
                                curcol++;
                            }
                            currow--;
                            while (curcol < 5 && currow >= 0){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a capture move
                                    does_have_captures = true;
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 5;
                                    aftermove.board[square] = 0;
                                    aftermove.board[capture_square] = 0;
                                    if (has_capture(aftermove.board, currow*5+curcol)){
                                        aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1);
                                        // forced capture and it's black's chance
                                    } else {
                                        aftermove.tomove = 1; // white's chance
                                    }
                                    moves.push_back({{square, currow*5+curcol}, aftermove});
                                } else {
                                    // some piece present so capture either stops or no capture at all
                                    break;
                                }
                                if (currow % 2){
                                    // currow is odd
                                    curcol++;
                                }
                                currow--;
                            }                        
                        }

                        // left backward capture
                        currow = row-1;
                        curcol = (row % 2 == 0) ? col-1 : col;
                        found = false;
                        while (curcol >= 0 && currow >= 0){
                            byte piece = curpos.board[currow*5+curcol];
                            if (piece == 0){
                                if (currow % 2 == 0){
                                    // row is even
                                    curcol--;
                                }
                                currow--;
                            } else if (piece == 2 || piece == 3){
                                found = true;
                                break;
                            } else {
                                // piece == 4 or piece == 5
                                break;
                            }
                        }

                        capture_square = currow*5+curcol;

                        if (found){
                            if (currow % 2 == 0){
                                // currow is even
                                curcol--;
                            }
                            currow--;
                            while (curcol >= 0 && currow >= 0){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a capture move
                                    does_have_captures = true;
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 5;
                                    aftermove.board[square] = 0;
                                    aftermove.board[capture_square] = 0;
                                    if (has_capture(aftermove.board, currow*5+curcol)){
                                        aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1);
                                        // forced capture and it's black's chance
                                    } else {
                                        aftermove.tomove = 1; // white's chance
                                    }
                                    moves.push_back({{square, currow*5+curcol}, aftermove});
                                } else {
                                    // some piece present so capture either stops or no capture at all
                                    break;
                                }
                                if (currow % 2 == 0){
                                    // currow is even
                                    curcol--;
                                }
                                currow--;
                            }
                        }
                    }
                }

                if (!does_have_captures){
                    // it does not have any captures
                    // normal moves
                    for (byte square=0; square<50; ++square){
                        if (curpos.board[square] == 4){
                            // black pawn on this square
                            byte row = square / 5;
                            byte col = square % 5;
                            if (row % 2 == 0){
                                // row is even

                                // right moves
                                // right backward move
                                if (row-1 >= 0){
                                    if (curpos.board[square-5] == 0){
                                        // empty place can move to this square
                                        position aftermove = curpos;
                                        aftermove.board[square-5] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.tomove = 1; // white to move
                                        moves.push_back({{square, square-5}, aftermove});
                                    }
                                }

                                // left moves
                                // left backward move
                                if (row-1 >= 0 && col-1 >= 0){
                                    if (curpos.board[square-6] == 0){
                                        // empty place can move to this square
                                        position aftermove = curpos;
                                        aftermove.board[square-6] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.tomove = 1; // white to move
                                        moves.push_back({{square, square-6}, aftermove});
                                    }
                                }
                            } else {
                                // row is odd

                                // right moves
                                // right backward move
                                if (row-1 >= 0 && col+1 < 5){
                                    if (curpos.board[square-4] == 0){
                                        // empty place can move to this square
                                        position aftermove = curpos;
                                        if (row-1 == 0) aftermove.board[square-4] = 5;
                                        else aftermove.board[square-4] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.tomove = 1; // white to move
                                        moves.push_back({{square, square-4}, aftermove});
                                    }
                                }

                                // left moves
                                // left backward move
                                if (row-1 >= 0){
                                    if (curpos.board[square-5] == 0){
                                        // empty place can move to this square
                                        position aftermove = curpos;
                                        if (row-1 == 0) aftermove.board[square-5] = 5;
                                        else aftermove.board[square-5] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.tomove = 1; // white to move
                                        moves.push_back({{square, square-5}, aftermove});
                                    }
                                }
                            }
                        } else if (curpos.board[square] == 5){
                            // black's king pawn

                            // it can go in all four directions and it is ...
                            // ... guaranteed that there is no capture move
                            byte row = square / 5;
                            byte col = square % 5;

                            // right forward
                            byte currow = row+1;
                            byte curcol = (row % 2) ? col+1 : col;
                            while (curcol < 5 && currow < 10){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 5;
                                    aftermove.board[square] = 0;
                                    aftermove.tomove = 1; // white to move
                                    moves.push_back({{square, currow*5+curcol}, aftermove});
                                } else {
                                    // piece == 2 or piece == 3 or piece == 4 or piece == 5
                                    break;
                                }
                                if (currow % 2){
                                    // row is odd
                                    curcol++;
                                }
                                currow++;
                            }

                            // right backward
                            currow = row-1;
                            curcol = (row % 2) ? col+1 : col;
                            while (curcol < 5 && currow >= 0){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 5;
                                    aftermove.board[square] = 0;
                                    aftermove.tomove = 1; // white to move
                                    moves.push_back({{square, currow*5+curcol}, aftermove});
                                } else {
                                    // piece == 2 or piece == 3
                                    break;
                                }
                                if (currow % 2){
                                    // row is odd
                                    curcol++;
                                }
                                currow--;
                            }

                            // left forward
                            currow = row+1;
                            curcol = (row % 2 == 0) ? col-1 : col;
                            while (curcol >= 0 && currow < 10){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 5;
                                    aftermove.board[square] = 0;
                                    aftermove.tomove = 1; // white to move
                                    moves.push_back({{square, currow*5+curcol}, aftermove});
                                } else {
                                    // piece == 2 or piece == 3
                                    break;
                                }
                                if (currow % 2 == 0){
                                    // row is even
                                    curcol--;
                                }
                                currow++;
                            }

                            // left backward
                            currow = row-1;
                            curcol = (row % 2 == 0) ? col-1 : col;
                            while (curcol >= 0 && currow >= 0){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 5;
                                    aftermove.board[square] = 0;
                                    aftermove.tomove = 1; // white to move
                                    moves.push_back({{square, currow*5+curcol}, aftermove});
                                } else {
                                    // piece == 2 or piece == 3
                                    break;
                                }
                                if (currow % 2 == 0){
                                    // row is even
                                    curcol--;
                                }
                                currow--;
                            }
                        }
                    }
                }
            }
        }
    #elif board_size == 52
        
    #elif board_size == 26
        // todo
    #else
        // should never reach here
        assert(0);
    #endif
    return moves;
}

std::vector<position> get_all_position(position curpos){
    std::vector<position> moves;
    // every move can be represent using an integer
    #if board_size == 51
        if (curpos.tomove & 1){
            // white to move
            if (curpos.tomove < 0){
                // forced capture
                byte square = (curpos.tomove & 0b01111110) >> 1; // 1st to 7th bit
                byte row = square / 5;
                byte col = square % 5;
                if (curpos.board[square] == 2){
                    // the forced capture square has a white pawn on it
                    if (row % 2 == 0){
                        // row is even
                        // right capture
                        if (col + 1 < 5){
                            // forward capture
                            if (row + 2 < 10){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square+5] == 4 || curpos.board[square+5] == 5) && curpos.board[square+11] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square+11] = 2;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square+5] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square+11)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square+11) << 1) + 1;
                                    } else {
                                        aftermove.tomove = 0; // no forced captures + black to move
                                    }
                                    moves.push_back(aftermove);
                                }
                            }

                            // backward capture
                            if (row - 2 >= 0){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square-5] == 4 || curpos.board[square-5] == 5) && curpos.board[square-9] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square-9] = 2;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square-5] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square-9)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square-9) << 1) + 1;
                                    } else {
                                        aftermove.tomove = 0; // no forced captures + black to move
                                    }
                                    moves.push_back(aftermove);
                                }
                            }
                        }

                            // left capture
                        if (col - 1 >= 0){
                            // forward capture
                            if (row + 2 < 10){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square+4] == 4 || curpos.board[square+4] == 5) && curpos.board[square+9] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square+9] = 2;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square+4] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square+9)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square+9) << 1) + 1;
                                    } else {
                                        aftermove.tomove = 0; // no forced captures + black to move
                                    }
                                    moves.push_back(aftermove);
                                }
                            }

                            // backward capture
                            if (row - 2 >= 0){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square-6] == 4 || curpos.board[square-6] == 5) && curpos.board[square-11] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square-11] = 2;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square-6] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square-11)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square-11) << 1) + 1;
                                    } else {
                                        aftermove.tomove = 0; // no forced captures + black to move
                                    }
                                    moves.push_back(aftermove);
                                }
                            }
                        }

                    } else {
                        // row is odd

                        // row is even
                        // right capture
                        if (col + 1 < 5){
                            // forward capture
                            if (row + 2 < 10){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square+6] == 4 || curpos.board[square+6] == 5) && curpos.board[square+11] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square+11] = 2;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square+6] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    // this piece even if it reaches the other end might not become a king ...
                                    // ... if this piece has more captures
                                    if (has_capture(aftermove.board, square+11)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square+11) << 1) + 1;
                                    } else {
                                        if (row+2 == 9) aftermove.board[square+11] = 3; // it becomes a king
                                        aftermove.tomove = 0; // no forced captures + black to move
                                    }
                                    moves.push_back(aftermove);
                                }
                            }

                            // backward capture
                            if (row - 2 >= 0){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square-4] == 4 || curpos.board[square-4] == 5) && curpos.board[square-9] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square-9] = 2;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square-4] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square-9)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square-9) << 1) + 1;
                                    } else {
                                        aftermove.tomove = 0; // no forced captures + black to move
                                    }
                                    moves.push_back(aftermove);
                                }
                            }
                        }

                        // left capture
                        if (col - 1 >= 0){
                            // forward capture
                            if (row + 2 < 10){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square+5] == 4 || curpos.board[square+5] == 5) && curpos.board[square+9] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square+9] = 2;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square+5] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    // this piece even if it reaches the other end might not become a king ...
                                    // ... if this piece has more captures
                                    if (has_capture(aftermove.board, square+9)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square+9) << 1) + 1;
                                    } else {
                                        if (row+2 == 9) aftermove.board[square+9] = 3; // it becomes a king
                                        aftermove.tomove = 0; // no forced captures + black to move
                                    }
                                    moves.push_back(aftermove);
                                }
                            }

                            // backward capture
                            if (row - 2 >= 0){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square-5] == 4 || curpos.board[square-5] == 5) && curpos.board[square-11] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square-11] = 2;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square-5] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square-11)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square-11) << 1) + 1;
                                    } else {
                                        aftermove.tomove = 0; // no forced captures + black to move
                                    }
                                    moves.push_back(aftermove);
                                }
                            }
                        }
                    }

                } else if (curpos.board[square] == 3){
                    // the forces capture square has a king white pawn on it
                    
                    // right forward capture
                    byte currow = row+1;
                    byte curcol = (row % 2) ? col+1 : col;
                    bool found = false;
                    while (curcol < 5 && currow < 10){
                        byte piece = curpos.board[currow*5+curcol];
                        if (piece == 0){
                            // move along the diagonal
                            if (currow % 2){
                                // row is odd
                                curcol++;
                            }
                            currow++;
                        } else if (piece == 4 || piece == 5){
                            found = true;
                            break;
                        } else {
                            // piece == 2 or piece == 3
                            break;
                        }
                    }

                    byte capture_square = currow*5+curcol;

                    if (found){
                        if (currow % 2){
                            // currow is odd
                            curcol++;
                        }
                        currow++;
                        while (curcol < 5 && currow < 10){
                            if (curpos.board[currow*5+curcol] == 0){
                                // this is a capture move
                                position aftermove = curpos;
                                aftermove.board[currow*5+curcol] = 3;
                                aftermove.board[square] = 0;
                                aftermove.board[capture_square] = 0;
                                if (has_capture(aftermove.board, currow*5+curcol)){
                                    aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1) + 1;
                                    // forced capture and it's white's chance
                                } else {
                                    aftermove.tomove = 0; // black's chance
                                }
                                moves.push_back(aftermove);
                            } else {
                                // some piece present so capture either stops or no capture at all
                                break;
                            }
                            // move along the diagonal
                            if (currow % 2){
                                // currow is odd
                                curcol++;
                            }
                            currow++;
                        }                        
                    }

                    // left forward capture
                    currow = row+1;
                    curcol = (row % 2 == 0) ? col-1 : col;
                    found = false;
                    while (curcol >= 0 && currow < 10){
                        byte piece = curpos.board[currow*5+curcol];
                        if (piece == 0){
                            if (currow % 2 == 0){
                                // row is even
                                curcol--;
                            }
                            currow++;
                        } else if (piece == 4 || piece == 5){
                            found = true;
                            break;
                        } else {
                            // piece == 2 or piece == 3
                            break;
                        }
                    }

                    capture_square = currow*5+curcol;

                    if (found){
                        if (currow % 2 == 0){
                            // currow is even
                            curcol--;
                        }
                        currow++;
                        while (curcol >= 0 && currow < 10){
                            if (curpos.board[currow*5+curcol] == 0){
                                // this is a capture move
                                position aftermove = curpos;
                                aftermove.board[currow*5+curcol] = 3;
                                aftermove.board[square] = 0;
                                aftermove.board[capture_square] = 0;
                                if (has_capture(aftermove.board, currow*5+curcol)){
                                    aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1) + 1;
                                    // forced capture and it's white's chance
                                } else {
                                    aftermove.tomove = 0; // black's chance
                                }
                                moves.push_back(aftermove);
                            } else {
                                // some piece present so capture either stops or no capture at all
                                break;
                            }
                            if (currow % 2 == 0){
                                // currow is even
                                curcol--;
                            }
                            currow++;
                        }
                    }

                    // right backward capture
                    currow = row-1;
                    curcol = (row % 2) ? col+1 : col;
                    found = false;
                    while (curcol < 5 && currow >= 0){
                        byte piece = curpos.board[currow*5+curcol];
                        if (piece == 0){
                            if (currow % 2){
                                // row is odd
                                curcol++;
                            }
                            currow--;
                        } else if (piece == 4 || piece == 5){
                            found = true;
                            break;
                        } else {
                            // piece == 2 or piece == 3
                            break;
                        }
                    }

                    capture_square = currow*5+curcol;

                    if (found){
                        if (currow % 2){
                            // currow is odd
                            curcol++;
                        }
                        currow--;
                        while (curcol < 5 && currow >= 0){
                            if (curpos.board[currow*5+curcol] == 0){
                                // this is a capture move
                                position aftermove = curpos;
                                aftermove.board[currow*5+curcol] = 3;
                                aftermove.board[square] = 0;
                                aftermove.board[capture_square] = 0;
                                if (has_capture(aftermove.board, currow*5+curcol)){
                                    aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1) + 1;
                                    // forced capture and it's white's chance
                                } else {
                                    aftermove.tomove = 0; // black's chance
                                }
                                moves.push_back(aftermove);
                            } else {
                                // some piece present so capture either stops or no capture at all
                                break;
                            }
                            if (currow % 2){
                                // currow is odd
                                curcol++;
                            }
                            currow--;
                        }                        
                    }

                    // left backward capture
                    currow = row-1;
                    curcol = (row % 2 == 0) ? col-1 : col;
                    found = false;
                    while (curcol >= 0 && currow >= 0){
                        byte piece = curpos.board[currow*5+curcol];
                        if (piece == 0){
                            if (currow % 2 == 0){
                                // row is even
                                curcol--;
                            }
                            currow--;
                        } else if (piece == 4 || piece == 5){
                            found = true;
                            break;
                        } else {
                            // piece == 2 or piece == 3
                            break;
                        }
                    }

                    capture_square = currow*5+curcol;

                    if (found){
                        if (currow % 2 == 0){
                            // currow is even
                            curcol--;
                        }
                        currow--;
                        while (curcol >= 0 && currow >= 0){
                            if (curpos.board[currow*5+curcol] == 0){
                                // this is a capture move
                                position aftermove = curpos;
                                aftermove.board[currow*5+curcol] = 3;
                                aftermove.board[square] = 0;
                                aftermove.board[capture_square] = 0;
                                if (has_capture(aftermove.board, currow*5+curcol)){
                                    aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1) + 1;
                                    // forced capture and it's white's chance
                                } else {
                                    aftermove.tomove = 0; // black's chance
                                }
                                moves.push_back(aftermove);
                            } else {
                                // some piece present so capture either stops or no capture at all
                                break;
                            }
                            if (currow % 2 == 0){
                                // currow is even
                                curcol--;
                            }
                            currow--;
                        }
                    }

                } else {
                    // should not reach here
                    // the forced capture square is not occupied by white itself (to move is white)
                    assert(0);
                }
            } else {
                // firstly check for captures, if any, limit to them
                // iterate over every white piece
                bool does_have_captures = false;
                // since if any capture found we have to capture the piece no other option for movement
                for (byte square=0; square<50; ++square){
                    byte row = square / 5;
                    byte col = square % 5;
                    if (curpos.board[square] == 2){
                        // the forced capture square has a white pawn on it
                        if (row % 2 == 0){
                            // row is even
                            // right capture
                            if (col + 1 < 5){
                                // forward capture
                                if (row + 2 < 10){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square+5] == 4 || curpos.board[square+5] == 5) && curpos.board[square+11] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square+11] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square+5] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square+11)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square+11) << 1) + 1;
                                        } else {
                                            aftermove.tomove = 0; // no forced captures + black to move
                                        }
                                        moves.push_back(aftermove);
                                    }
                                }

                                // backward capture
                                if (row - 2 >= 0){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square-5] == 4 || curpos.board[square-5] == 5) && curpos.board[square-9] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square-9] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square-5] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square-9)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square-9) << 1) + 1;
                                        } else {
                                            aftermove.tomove = 0; // no forced captures + black to move
                                        }
                                        moves.push_back(aftermove);
                                    }
                                }
                            }

                                // left capture
                            if (col - 1 >= 0){
                                // forward capture
                                if (row + 2 < 10){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square+4] == 4 || curpos.board[square+4] == 5) && curpos.board[square+9] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square+9] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square+4] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square+9)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square+9) << 1) + 1;
                                        } else {
                                            aftermove.tomove = 0; // no forced captures + black to move
                                        }
                                        moves.push_back(aftermove);
                                    }
                                }

                                // backward capture
                                if (row - 2 >= 0){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square-6] == 4 || curpos.board[square-6] == 5) && curpos.board[square-11] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square-11] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square-6] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square-11)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square-11) << 1) + 1;
                                        } else {
                                            aftermove.tomove = 0; // no forced captures + black to move
                                        }
                                        moves.push_back(aftermove);
                                    }
                                }
                            }

                        } else {
                            // row is odd

                            // row is even
                            // right capture
                            if (col + 1 < 5){
                                // forward capture
                                if (row + 2 < 10){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square+6] == 4 || curpos.board[square+6] == 5) && curpos.board[square+11] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square+11] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square+6] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        // this piece even if it reaches the other end might not become a king ...
                                        // ... if this piece has more captures
                                        if (has_capture(aftermove.board, square+11)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square+11) << 1) + 1;
                                        } else {
                                            if (row+2 == 9) aftermove.board[square+11] = 3; // it becomes a king
                                            aftermove.tomove = 0; // no forced captures + black to move
                                        }
                                        moves.push_back(aftermove);
                                    }
                                }

                                // backward capture
                                if (row - 2 >= 0){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square-4] == 4 || curpos.board[square-4] == 5) && curpos.board[square-9] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square-9] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square-4] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square-9)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square-9) << 1) + 1;
                                        } else {
                                            aftermove.tomove = 0; // no forced captures + black to move
                                        }
                                        moves.push_back(aftermove);
                                    }
                                }
                            }

                            // left capture
                            if (col - 1 >= 0){
                                // forward capture
                                if (row + 2 < 10){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square+5] == 4 || curpos.board[square+5] == 5) && curpos.board[square+9] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square+9] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square+5] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        // this piece even if it reaches the other end might not become a king ...
                                        // ... if this piece has more captures
                                        if (has_capture(aftermove.board, square+9)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square+9) << 1) + 1;
                                        } else {
                                            if (row+2 == 9) aftermove.board[square+9] = 3; // it becomes a king
                                            aftermove.tomove = 0; // no forced captures + black to move
                                        }
                                        moves.push_back(aftermove);
                                    }
                                }

                                // backward capture
                                if (row - 2 >= 0){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square-5] == 4 || curpos.board[square-5] == 5) && curpos.board[square-11] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square-11] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square-5] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square-11)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square-11) << 1) + 1;
                                        } else {
                                            aftermove.tomove = 0; // no forced captures + black to move
                                        }
                                        moves.push_back(aftermove);
                                    }
                                }
                            }
                        }

                    } else if (curpos.board[square] == 3){
                        // the forces capture square has a king white pawn on it
                        
                        // right forward capture
                        byte currow = row+1;
                        byte curcol = (row % 2) ? col+1 : col;
                        bool found = false;
                        while (curcol < 5 && currow < 10){
                            byte piece = curpos.board[currow*5+curcol];
                            if (piece == 0){
                                if (currow % 2){
                                    // row is odd
                                    curcol++;
                                }
                                currow++;
                            } else if (piece == 4 || piece == 5){
                                found = true;
                                break;
                            } else {
                                // piece == 2 or piece == 3
                                break;
                            }
                        }

                        byte capture_square = currow*5+curcol;

                        if (found){
                            if (currow % 2){
                                // currow is odd
                                curcol++;
                            }
                            currow++;
                            while (curcol < 5 && currow < 10){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a capture move
                                    does_have_captures = true;
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 3;
                                    aftermove.board[square] = 0;
                                    aftermove.board[capture_square] = 0;
                                    if (has_capture(aftermove.board, currow*5+curcol)){
                                        aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1) + 1;
                                        // forced capture and it's white's chance
                                    } else {
                                        aftermove.tomove = 0; // black's chance
                                    }
                                    moves.push_back(aftermove);
                                } else {
                                    // some piece present so capture either stops or no capture at all
                                    break;
                                }
                                if (currow % 2){
                                    // currow is odd
                                    curcol++;
                                }
                                currow++;
                            }                        
                        }

                        // left forward capture
                        currow = row+1;
                        curcol = (row % 2 == 0) ? col-1 : col;
                        found = false;
                        while (curcol >= 0 && currow < 10){
                            byte piece = curpos.board[currow*5+curcol];
                            if (piece == 0){
                                if (currow % 2 == 0){
                                    // row is even
                                    curcol--;
                                }
                                currow++;
                            } else if (piece == 4 || piece == 5){
                                found = true;
                                break;
                            } else {
                                // piece == 2 or piece == 3
                                break;
                            }
                        }

                        capture_square = currow*5+curcol;

                        if (found){
                            if (currow % 2 == 0){
                                // currow is even
                                curcol--;
                            }
                            currow++;
                            while (curcol >= 0 && currow < 10){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a capture move
                                    does_have_captures = true;
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 3;
                                    aftermove.board[square] = 0;
                                    aftermove.board[capture_square] = 0;
                                    if (has_capture(aftermove.board, currow*5+curcol)){
                                        aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1) + 1;
                                        // forced capture and it's white's chance
                                    } else {
                                        aftermove.tomove = 0; // black's chance
                                    }
                                    moves.push_back(aftermove);
                                } else {
                                    // some piece present so capture either stops or no capture at all
                                    break;
                                }
                                if (currow % 2 == 0){
                                    // currow is even
                                    curcol--;
                                }
                                currow++;
                            }
                        }

                        // right backward capture
                        currow = row-1;
                        curcol = (row % 2) ? col+1 : col;
                        found = false;
                        while (curcol < 5 && currow >= 0){
                            byte piece = curpos.board[currow*5+curcol];
                            if (piece == 0){
                                if (currow % 2){
                                    // row is odd
                                    curcol++;
                                }
                                currow--;
                            } else if (piece == 4 || piece == 5){
                                found = true;
                                break;
                            } else {
                                // piece == 2 or piece == 3
                                break;
                            }
                        }

                        capture_square = currow*5+curcol;

                        if (found){
                            if (currow % 2){
                                // currow is odd
                                curcol++;
                            }
                            currow--;
                            while (curcol < 5 && currow >= 0){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a capture move
                                    does_have_captures = true;
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 3;
                                    aftermove.board[square] = 0;
                                    aftermove.board[capture_square] = 0;
                                    if (has_capture(aftermove.board, currow*5+curcol)){
                                        aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1) + 1;
                                        // forced capture and it's white's chance
                                    } else {
                                        aftermove.tomove = 0; // black's chance
                                    }
                                    moves.push_back(aftermove);
                                } else {
                                    // some piece present so capture either stops or no capture at all
                                    break;
                                }
                                if (currow % 2){
                                    // currow is odd
                                    curcol++;
                                }
                                currow--;
                            }                        
                        }

                        // left backward capture
                        currow = row-1;
                        curcol = (row % 2 == 0) ? col-1 : col;
                        found = false;
                        while (curcol >= 0 && currow >= 0){
                            byte piece = curpos.board[currow*5+curcol];
                            if (piece == 0){
                                if (currow % 2 == 0){
                                    // row is even
                                    curcol--;
                                }
                                currow--;
                            } else if (piece == 4 || piece == 5){
                                found = true;
                                break;
                            } else {
                                // piece == 2 or piece == 3
                                break;
                            }
                        }

                        capture_square = currow*5+curcol;

                        if (found){
                            if (currow % 2 == 0){
                                // currow is even
                                curcol--;
                            }
                            currow--;
                            while (curcol >= 0 && currow >= 0){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a capture move
                                    does_have_captures = true;
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 3;
                                    aftermove.board[square] = 0;
                                    aftermove.board[capture_square] = 0;
                                    if (has_capture(aftermove.board, currow*5+curcol)){
                                        aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1) + 1;
                                        // forced capture and it's white's chance
                                    } else {
                                        aftermove.tomove = 0; // black's chance
                                    }
                                    moves.push_back(aftermove);
                                } else {
                                    // some piece present so capture either stops or no capture at all
                                    break;
                                }
                                if (currow % 2 == 0){
                                    // currow is even
                                    curcol--;
                                }
                                currow--;
                            }
                        }

                    }
                }
                
                if (!does_have_captures){
                    // it does not have any captures
                    // normal moves
                    for (byte square=0; square<50; ++square){
                        if (curpos.board[square] == 2){
                            // white pawn on this square
                            byte row = square / 5;
                            byte col = square % 5;
                            if (row % 2 == 0){
                                // row is even

                                // right moves
                                // right forward move
                                if (row+1 < 10){
                                    if (curpos.board[square+5] == 0){
                                        // empty place can move to this square
                                        position aftermove = curpos;
                                        if (row+1 == 9) aftermove.board[square+5] = 3;
                                        else aftermove.board[square+5] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.tomove = 0; // black to move
                                        moves.push_back(aftermove);
                                    }
                                }

                                // left moves
                                // left forward move
                                if (row+1 < 10 && col-1 >= 0){
                                    if (curpos.board[square+4] == 0){
                                        // empty place can move to this square
                                        position aftermove = curpos;
                                        if (row+1 == 9) aftermove.board[square+4] = 3;
                                        else aftermove.board[square+4] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.tomove = 0; // black to move
                                        moves.push_back(aftermove);
                                    }
                                }
                            } else {
                                // row is odd

                                // right moves
                                // right forward move
                                if (row+1 < 10 && col+1 < 5){
                                    if (curpos.board[square+6] == 0){
                                        // empty place can move to this square
                                        position aftermove = curpos;
                                        aftermove.board[square+6] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.tomove = 0; // black to move
                                        moves.push_back(aftermove);
                                    }
                                }

                                // left moves
                                // left forward move
                                if (row+1 < 10){
                                    if (curpos.board[square+5] == 0){
                                        // empty place can move to this square
                                        position aftermove = curpos;
                                        aftermove.board[square+5] = 2;
                                        aftermove.board[square] = 0;
                                        aftermove.tomove = 0; // black to move
                                        moves.push_back(aftermove);
                                    }
                                }
                            }
                        } else if (curpos.board[square] == 3){
                            // white's king pawn
                            
                            // it can go in all four directions and it is ...
                            // ... guaranteed that there is no capture move
                            byte row = square / 5;
                            byte col = square % 5;

                            // right forward
                            byte currow = row+1;
                            byte curcol = (row % 2) ? col+1 : col;
                            while (curcol < 5 && currow < 10){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 3;
                                    aftermove.board[square] = 0;
                                    aftermove.tomove = 0; // black to move
                                    moves.push_back(aftermove);
                                } else {
                                    // piece == 2 or piece == 3 or piece == 4 or piece == 5
                                    break;
                                }
                                if (currow % 2){
                                    // row is odd
                                    curcol++;
                                }
                                currow++;
                            }

                            // right backward
                            currow = row-1;
                            curcol = (row % 2) ? col+1 : col;
                            while (curcol < 5 && currow >= 0){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 3;
                                    aftermove.board[square] = 0;
                                    aftermove.tomove = 0; // black to move
                                    moves.push_back(aftermove);
                                } else {
                                    // piece == 2 or piece == 3
                                    break;
                                }
                                if (currow % 2){
                                    // row is odd
                                    curcol++;
                                }
                                currow--;
                            }

                            // left forward
                            currow = row+1;
                            curcol = (row % 2 == 0) ? col-1 : col;
                            while (curcol >= 0 && currow < 10){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 3;
                                    aftermove.board[square] = 0;
                                    aftermove.tomove = 0; // black to move
                                    moves.push_back(aftermove);
                                } else {
                                    // piece == 2 or piece == 3
                                    break;
                                }
                                if (currow % 2 == 0){
                                    // row is even
                                    curcol--;
                                }
                                currow++;
                            }

                            // left backward
                            currow = row-1;
                            curcol = (row % 2 == 0) ? col-1 : col;
                            while (curcol >= 0 && currow >= 0){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 3;
                                    aftermove.board[square] = 0;
                                    aftermove.tomove = 0; // black to move
                                    moves.push_back(aftermove);
                                } else {
                                    // piece == 2 or piece == 3
                                    break;
                                }
                                if (currow % 2 == 0){
                                    // row is even
                                    curcol--;
                                }
                                currow--;
                            }
                        }
                    }
                }
            }

        } else {
            // black to move
            if (curpos.tomove < 0){
                // forced capture
                byte square = (curpos.tomove & 0b01111110) >> 1; // 1st to 7th bit
                byte row = square / 5;
                byte col = square % 5;
                if (curpos.board[square] == 4){
                    // the forced capture square has a black pawn on it
                    if (row % 2 == 0){
                        // row is even
                        // right capture
                        if (col + 1 < 5){
                            // forward capture
                            if (row + 2 < 10){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square+5] == 2 || curpos.board[square+5] == 3) && curpos.board[square+11] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square+11] = 4;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square+5] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square+11)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square+11) << 1);
                                    } else {
                                        aftermove.tomove = 1; // no forced captures + white to move
                                    }
                                    moves.push_back(aftermove);
                                }
                            }

                            // backward capture
                            if (row - 2 >= 0){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square-5] == 2 || curpos.board[square-5] == 3) && curpos.board[square-9] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square-9] = 4;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square-5] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    // this piece even if it reaches the other end might not become a king ...
                                    // ... if this piece has more captures
                                    if (has_capture(aftermove.board, square-9)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square-9) << 1);
                                    } else {
                                        if (row-2 == 0) aftermove.board[square-9] = 5;
                                        aftermove.tomove = 1; // no forced captures + white to move
                                    }
                                    moves.push_back(aftermove);
                                }
                            }
                        }

                        // left capture
                        if (col - 1 >= 0){
                            // forward capture
                            if (row + 2 < 10){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square+4] == 2 || curpos.board[square+4] == 3) && curpos.board[square+9] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square+9] = 4;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square+4] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square+9)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square+9) << 1);
                                    } else {
                                        aftermove.tomove = 1; // no forced captures + white to move
                                    }
                                    moves.push_back(aftermove);
                                }
                            }

                            // backward capture
                            if (row - 2 >= 0){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square-6] == 2 || curpos.board[square-6] == 3) && curpos.board[square-11] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square-11] = 4;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square-6] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    // this piece even if it reaches the other end might not become a king ...
                                    // ... if this piece has more captures
                                    if (has_capture(aftermove.board, square-11)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square-11) << 1);
                                    } else {
                                        if (row-2 == 0) aftermove.board[square-11] = 5;
                                        aftermove.tomove = 1; // no forced captures + white to move
                                    }
                                    moves.push_back(aftermove);
                                }
                            }
                        }

                    } else {
                        // row is odd

                        // row is even
                        // right capture
                        if (col + 1 < 5){
                            // forward capture
                            if (row + 2 < 10){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square+6] == 2 || curpos.board[square+6] == 3) && curpos.board[square+11] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square+11] = 4;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square+6] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square+11)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square+11) << 1);
                                    } else {
                                        aftermove.tomove = 1; // no forced captures + white to move
                                    }
                                    moves.push_back(aftermove);
                                }
                            }

                            // backward capture
                            if (row - 2 >= 0){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square-4] == 2 || curpos.board[square-4] == 3) && curpos.board[square-9] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square-9] = 4;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square-4] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square-9)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square-9) << 1);
                                    } else {
                                        aftermove.tomove = 1; // no forced captures + white to move
                                    }
                                    moves.push_back(aftermove);
                                }
                            }
                        }

                        // left capture
                        if (col - 1 >= 0){
                            // forward capture
                            if (row + 2 < 10){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square+5] == 2 || curpos.board[square+5] == 3) && curpos.board[square+9] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square+9] = 4;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square+5] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square+9)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square+9) << 1);
                                    } else {
                                        aftermove.tomove = 1; // no forced captures + white to move
                                    }
                                    moves.push_back(aftermove);
                                }
                            }

                            // backward capture
                            if (row - 2 >= 0){
                                // now check whether the pawn are placed correctly or not
                                if ((curpos.board[square-5] == 2 || curpos.board[square-5] == 3) && curpos.board[square-11] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[square-11] = 4;
                                    aftermove.board[square] = 0;
                                    aftermove.board[square-5] = 0;
                                    // check whether this pawn has further capture avaliable or not
                                    if (has_capture(aftermove.board, square-11)){
                                        // has further forced captures
                                        aftermove.tomove = (1 << 7) + ((square-11) << 1);
                                    } else {
                                        aftermove.tomove = 1; // no forced captures + white to move
                                    }
                                    moves.push_back(aftermove);
                                }
                            }
                        }
                    }

                } else if (curpos.board[square] == 5){
                    // the forces capture square has a king black pawn on it
                    
                    // right forward capture
                    byte currow = row+1;
                    byte curcol = (row % 2) ? col+1 : col;
                    bool found = false;
                    while (curcol < 5 && currow < 10){
                        byte piece = curpos.board[currow*5+curcol];
                        if (piece == 0){
                            if (currow % 2){
                                // row is odd
                                curcol++;
                            }
                            currow++;
                        } else if (piece == 2 || piece == 3){
                            found = true;
                            break;
                        } else {
                            // piece == 4 or piece == 5
                            break;
                        }
                    }

                    byte capture_square = currow*5+curcol;

                    if (found){
                        if (currow % 2){
                            // currow is odd
                            curcol++;
                        }
                        currow++;
                        while (curcol < 5 && currow < 10){
                            if (curpos.board[currow*5+curcol] == 0){
                                // this is a capture move
                                position aftermove = curpos;
                                aftermove.board[currow*5+curcol] = 5;
                                aftermove.board[square] = 0;
                                aftermove.board[capture_square] = 0;
                                if (has_capture(aftermove.board, currow*5+curcol)){
                                    aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1);
                                    // forced capture and it's black's chance
                                } else {
                                    aftermove.tomove = 1; // white's chance
                                }
                                moves.push_back(aftermove);
                            } else {
                                // some piece present so capture either stops or no capture at all
                                break;
                            }
                            if (currow % 2){
                                // currow is odd
                                curcol++;
                            }
                            currow++;
                        }                        
                    }

                    // left forward capture
                    currow = row+1;
                    curcol = (row % 2 == 0) ? col-1 : col;
                    found = false;
                    while (curcol >= 0 && currow < 10){
                        byte piece = curpos.board[currow*5+curcol];
                        if (piece == 0){
                            if (currow % 2 == 0){
                                // row is even
                                curcol--;
                            }
                            currow++;
                        } else if (piece == 2 || piece == 3){
                            found = true;
                            break;
                        } else {
                            // piece == 4 or piece == 5
                            break;
                        }
                    }

                    capture_square = currow*5+curcol;

                    if (found){
                        if (currow % 2 == 0){
                            // currow is even
                            curcol--;
                        }
                        currow++;
                        while (curcol >= 0 && currow < 10){
                            if (curpos.board[currow*5+curcol] == 0){
                                // this is a capture move
                                position aftermove = curpos;
                                aftermove.board[currow*5+curcol] = 5;
                                aftermove.board[square] = 0;
                                aftermove.board[capture_square] = 0;
                                if (has_capture(aftermove.board, currow*5+curcol)){
                                    aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1);
                                    // forced capture and it's black's chance
                                } else {
                                    aftermove.tomove = 1; // white's chance
                                }
                                moves.push_back(aftermove);
                            } else {
                                // some piece present so capture either stops or no capture at all
                                break;
                            }
                            if (currow % 2 == 0){
                                // currow is even
                                curcol--;
                            }
                            currow++;
                        }
                    }

                    // right backward capture
                    currow = row-1;
                    curcol = (row % 2) ? col+1 : col;
                    found = false;
                    while (curcol < 5 && currow >= 0){
                        byte piece = curpos.board[currow*5+curcol];
                        if (piece == 0){
                            if (currow % 2){
                            // row is odd
                                curcol++;
                            }
                            currow--;
                        } else if (piece == 2 || piece == 3){
                            found = true;
                            break;
                        } else {
                            // piece == 4 or piece == 5
                            break;
                        }
                    }

                    capture_square = currow*5+curcol;

                    if (found){
                        if (currow % 2){
                            // currow is odd
                            curcol++;
                        }
                        currow--;
                        while (curcol < 5 && currow >= 0){
                            if (curpos.board[currow*5+curcol] == 0){
                                // this is a capture move
                                position aftermove = curpos;
                                aftermove.board[currow*5+curcol] = 5;
                                aftermove.board[square] = 0;
                                aftermove.board[capture_square] = 0;
                                if (has_capture(aftermove.board, currow*5+curcol)){
                                    aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1);
                                    // forced capture and it's black's chance
                                } else {
                                    aftermove.tomove = 1; // white's chance
                                }
                                moves.push_back(aftermove);
                            } else {
                                // some piece present so capture either stops or no capture at all
                                break;
                            }
                            if (currow % 2){
                                // currow is odd
                                curcol++;
                            }
                            currow--;
                        }                        
                    }

                    // left backward capture
                    currow = row-1;
                    curcol = (row % 2 == 0) ? col-1 : col;
                    found = false;
                    while (curcol >= 0 && currow >= 0){
                        byte piece = curpos.board[currow*5+curcol];
                        if (piece == 0){
                            if (currow % 2 == 0){
                                // row is even
                                curcol--;
                            }
                            currow--;
                        } else if (piece == 2 || piece == 3){
                            found = true;
                            break;
                        } else {
                            // piece == 4 or piece == 5
                            break;
                        }
                    }

                    capture_square = currow*5+curcol;

                    if (found){
                        if (currow % 2 == 0){
                            // currow is even
                            curcol--;
                        }
                        currow--;
                        while (curcol >= 0 && currow >= 0){
                            if (curpos.board[currow*5+curcol] == 0){
                                // this is a capture move
                                position aftermove = curpos;
                                aftermove.board[currow*5+curcol] = 5;
                                aftermove.board[square] = 0;
                                aftermove.board[capture_square] = 0;
                                if (has_capture(aftermove.board, currow*5+curcol)){
                                    aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1);
                                    // forced capture and it's black's chance
                                } else {
                                    aftermove.tomove = 1; // white's chance
                                }
                                moves.push_back(aftermove);
                            } else {
                                // some piece present so capture either stops or no capture at all
                                break;
                            }
                            if (currow % 2 == 0){
                                // currow is even
                                curcol--;
                            }
                            currow--;
                        }
                    }

                } else {
                    // should not reach here
                    // the forced capture square is not occupied by white itself (to move is white)
                    assert(0);
                }
            } else {
                // firstly check for captures, if any, limit to them
                // iterate over every black piece
                bool does_have_captures = false;
                // since if any capture found we have to capture the piece no other option for movement
                for (byte square=0; square<50; ++square){
                    byte row = square / 5;
                    byte col = square % 5;
                    if (curpos.board[square] == 4){
                        // the forced capture square has a black pawn on it
                        if (row % 2 == 0){
                            // row is even
                            // right capture
                            if (col + 1 < 5){
                                // forward capture
                                if (row + 2 < 10){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square+5] == 2 || curpos.board[square+5] == 3) && curpos.board[square+11] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square+11] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square+5] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square+11)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square+11) << 1);
                                        } else {
                                            aftermove.tomove = 1; // no forced captures + white to move
                                        }
                                        moves.push_back(aftermove);
                                    }
                                }

                                // backward capture
                                if (row - 2 >= 0){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square-5] == 2 || curpos.board[square-5] == 3) && curpos.board[square-9] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square-9] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square-5] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        // this piece even if it reaches the other end might not become a king ...
                                        // ... if this piece has more captures
                                        if (has_capture(aftermove.board, square-9)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square-9) << 1);
                                        } else {
                                            if (row-2 == 0) aftermove.board[square-9] = 5;
                                            aftermove.tomove = 1; // no forced captures + white to move
                                        }
                                        moves.push_back(aftermove);
                                    }
                                }
                            }

                            // left capture
                            if (col - 1 >= 0){
                                // forward capture
                                if (row + 2 < 10){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square+4] == 2 || curpos.board[square+4] == 3) && curpos.board[square+9] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square+9] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square+4] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square+9)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square+9) << 1);
                                        } else {
                                            aftermove.tomove = 1; // no forced captures + white to move
                                        }
                                        moves.push_back(aftermove);
                                    }
                                }

                                // backward capture
                                if (row - 2 >= 0){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square-6] == 2 || curpos.board[square-6] == 3) && curpos.board[square-11] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square-11] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square-6] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        // this piece even if it reaches the other end might not become a king ...
                                        // ... if this piece has more captures
                                        if (has_capture(aftermove.board, square-11)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square-11) << 1);
                                        } else {
                                            if (row-2 == 0) aftermove.board[square-11] = 5;
                                            aftermove.tomove = 1; // no forced captures + white to move
                                        }
                                        moves.push_back(aftermove);
                                    }
                                }
                            }

                        } else {
                            // row is odd

                            // row is even
                            // right capture
                            if (col + 1 < 5){
                                // forward capture
                                if (row + 2 < 10){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square+6] == 2 || curpos.board[square+6] == 3) && curpos.board[square+11] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square+11] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square+6] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square+11)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square+11) << 1);
                                        } else {
                                            aftermove.tomove = 1; // no forced captures + white to move
                                        }
                                        moves.push_back(aftermove);
                                    }
                                }

                                // backward capture
                                if (row - 2 >= 0){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square-4] == 2 || curpos.board[square-4] == 3) && curpos.board[square-9] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square-9] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square-4] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square-9)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square-9) << 1);
                                        } else {
                                            aftermove.tomove = 1; // no forced captures + white to move
                                        }
                                        moves.push_back(aftermove);
                                    }
                                }
                            }

                            // left capture
                            if (col - 1 >= 0){
                                // forward capture
                                if (row + 2 < 10){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square+5] == 2 || curpos.board[square+5] == 3) && curpos.board[square+9] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square+9] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square+5] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square+9)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square+9) << 1);
                                        } else {
                                            aftermove.tomove = 1; // no forced captures + white to move
                                        }
                                        moves.push_back(aftermove);
                                    }
                                }

                                // backward capture
                                if (row - 2 >= 0){
                                    // now check whether the pawn are placed correctly or not
                                    if ((curpos.board[square-5] == 2 || curpos.board[square-5] == 3) && curpos.board[square-11] == 0){
                                        // this is a possible move
                                        does_have_captures = true;
                                        position aftermove = curpos;
                                        aftermove.board[square-11] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.board[square-5] = 0;
                                        // check whether this pawn has further capture avaliable or not
                                        if (has_capture(aftermove.board, square-11)){
                                            // has further forced captures
                                            aftermove.tomove = (1 << 7) + ((square-11) << 1);
                                        } else {
                                            aftermove.tomove = 1; // no forced captures + white to move
                                        }
                                        moves.push_back(aftermove);
                                    }
                                }
                            }
                        }

                    } else if (curpos.board[square] == 5){
                        // the forces capture square has a king black pawn on it
                        
                        // right forward capture
                        byte currow = row+1;
                        byte curcol = (row % 2) ? col+1 : col;
                        bool found = false;
                        while (curcol < 5 && currow < 10){
                            byte piece = curpos.board[currow*5+curcol];
                            if (piece == 0){
                                if (currow % 2){
                                    // row is odd
                                    curcol++;
                                }
                                currow++;
                            } else if (piece == 2 || piece == 3){
                                found = true;
                                break;
                            } else {
                                // piece == 4 or piece == 5
                                break;
                            }
                        }

                        byte capture_square = currow*5+curcol;

                        if (found){
                            if (currow % 2){
                                // currow is odd
                                curcol++;
                            }
                            currow++;
                            while (curcol < 5 && currow < 10){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a capture move
                                    does_have_captures = true;
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 5;
                                    aftermove.board[square] = 0;
                                    aftermove.board[capture_square] = 0;
                                    if (has_capture(aftermove.board, currow*5+curcol)){
                                        aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1);
                                        // forced capture and it's black's chance
                                    } else {
                                        aftermove.tomove = 1; // white's chance
                                    }
                                    moves.push_back(aftermove);
                                } else {
                                    // some piece present so capture either stops or no capture at all
                                    break;
                                }
                                if (currow % 2){
                                    // currow is odd
                                    curcol++;
                                }
                                currow++;
                            }                        
                        }

                        // left forward capture
                        currow = row+1;
                        curcol = (row % 2 == 0) ? col-1 : col;
                        found = false;
                        while (curcol >= 0 && currow < 10){
                            byte piece = curpos.board[currow*5+curcol];
                            if (piece == 0){
                                if (currow % 2 == 0){
                                    // row is even
                                    curcol--;
                                }
                                currow++;
                            } else if (piece == 2 || piece == 3){
                                found = true;
                                break;
                            } else {
                                // piece == 4 or piece == 5
                                break;
                            }
                        }

                        capture_square = currow*5+curcol;

                        if (found){
                            if (currow % 2 == 0){
                                // currow is even
                                curcol--;
                            }
                            currow++;
                            while (curcol >= 0 && currow < 10){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a capture move
                                    does_have_captures = true;
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 5;
                                    aftermove.board[square] = 0;
                                    aftermove.board[capture_square] = 0;
                                    if (has_capture(aftermove.board, currow*5+curcol)){
                                        aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1);
                                        // forced capture and it's black's chance
                                    } else {
                                        aftermove.tomove = 1; // white's chance
                                    }
                                    moves.push_back(aftermove);
                                } else {
                                    // some piece present so capture either stops or no capture at all
                                    break;
                                }
                                if (currow % 2 == 0){
                                    // currow is even
                                    curcol--;
                                }
                                currow++;
                            }
                        }

                        // right backward capture
                        currow = row-1;
                        curcol = (row % 2) ? col+1 : col;
                        found = false;
                        while (curcol < 5 && currow >= 0){
                            byte piece = curpos.board[currow*5+curcol];
                            if (piece == 0){
                                if (currow % 2){
                                    // row is odd
                                    curcol++;
                                }
                                currow--;
                            } else if (piece == 2 || piece == 3){
                                found = true;
                                break;
                            } else {
                                // piece == 4 or piece == 5
                                break;
                            }
                        }

                        capture_square = currow*5+curcol;

                        if (found){
                            if (currow % 2){
                                // currow is odd
                                curcol++;
                            }
                            currow--;
                            while (curcol < 5 && currow >= 0){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a capture move
                                    does_have_captures = true;
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 5;
                                    aftermove.board[square] = 0;
                                    aftermove.board[capture_square] = 0;
                                    if (has_capture(aftermove.board, currow*5+curcol)){
                                        aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1);
                                        // forced capture and it's black's chance
                                    } else {
                                        aftermove.tomove = 1; // white's chance
                                    }
                                    moves.push_back(aftermove);
                                } else {
                                    // some piece present so capture either stops or no capture at all
                                    break;
                                }
                                if (currow % 2){
                                    // currow is odd
                                    curcol++;
                                }
                                currow--;
                            }                        
                        }

                        // left backward capture
                        currow = row-1;
                        curcol = (row % 2 == 0) ? col-1 : col;
                        found = false;
                        while (curcol >= 0 && currow >= 0){
                            byte piece = curpos.board[currow*5+curcol];
                            if (piece == 0){
                                if (currow % 2 == 0){
                                    // row is even
                                    curcol--;
                                }
                                currow--;
                            } else if (piece == 2 || piece == 3){
                                found = true;
                                break;
                            } else {
                                // piece == 4 or piece == 5
                                break;
                            }
                        }

                        capture_square = currow*5+curcol;

                        if (found){
                            if (currow % 2 == 0){
                                // currow is even
                                curcol--;
                            }
                            currow--;
                            while (curcol >= 0 && currow >= 0){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a capture move
                                    does_have_captures = true;
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 5;
                                    aftermove.board[square] = 0;
                                    aftermove.board[capture_square] = 0;
                                    if (has_capture(aftermove.board, currow*5+curcol)){
                                        aftermove.tomove = (1 << 7) + ((currow*5+curcol) << 1);
                                        // forced capture and it's black's chance
                                    } else {
                                        aftermove.tomove = 1; // white's chance
                                    }
                                    moves.push_back(aftermove);
                                } else {
                                    // some piece present so capture either stops or no capture at all
                                    break;
                                }
                                if (currow % 2 == 0){
                                    // currow is even
                                    curcol--;
                                }
                                currow--;
                            }
                        }
                    }
                }

                if (!does_have_captures){
                    // it does not have any captures
                    // normal moves
                    for (byte square=0; square<50; ++square){
                        if (curpos.board[square] == 4){
                            // black pawn on this square
                            byte row = square / 5;
                            byte col = square % 5;
                            if (row % 2 == 0){
                                // row is even

                                // right moves
                                // right backward move
                                if (row-1 >= 0){
                                    if (curpos.board[square-5] == 0){
                                        // empty place can move to this square
                                        position aftermove = curpos;
                                        aftermove.board[square-5] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.tomove = 1; // white to move
                                        moves.push_back(aftermove);
                                    }
                                }

                                // left moves
                                // left backward move
                                if (row-1 >= 0 && col-1 >= 0){
                                    if (curpos.board[square-6] == 0){
                                        // empty place can move to this square
                                        position aftermove = curpos;
                                        aftermove.board[square-6] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.tomove = 1; // white to move
                                        moves.push_back(aftermove);
                                    }
                                }
                            } else {
                                // row is odd

                                // right moves
                                // right backward move
                                if (row-1 >= 0 && col+1 < 5){
                                    if (curpos.board[square-4] == 0){
                                        // empty place can move to this square
                                        position aftermove = curpos;
                                        if (row-1 == 0) aftermove.board[square-4] = 5;
                                        else aftermove.board[square-4] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.tomove = 1; // white to move
                                        moves.push_back(aftermove);
                                    }
                                }

                                // left moves
                                // left backward move
                                if (row-1 >= 0){
                                    if (curpos.board[square-5] == 0){
                                        // empty place can move to this square
                                        position aftermove = curpos;
                                        if (row-1 == 0) aftermove.board[square-5] = 5;
                                        else aftermove.board[square-5] = 4;
                                        aftermove.board[square] = 0;
                                        aftermove.tomove = 1; // white to move
                                        moves.push_back(aftermove);
                                    }
                                }
                            }
                        } else if (curpos.board[square] == 5){
                            // black's king pawn

                            // it can go in all four directions and it is ...
                            // ... guaranteed that there is no capture move
                            byte row = square / 5;
                            byte col = square % 5;

                            // right forward
                            byte currow = row+1;
                            byte curcol = (row % 2) ? col+1 : col;
                            while (curcol < 5 && currow < 10){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 5;
                                    aftermove.board[square] = 0;
                                    aftermove.tomove = 1; // white to move
                                    moves.push_back(aftermove);
                                } else {
                                    // piece == 2 or piece == 3 or piece == 4 or piece == 5
                                    break;
                                }
                                if (currow % 2){
                                    // row is odd
                                    curcol++;
                                }
                                currow++;
                            }

                            // right backward
                            currow = row-1;
                            curcol = (row % 2) ? col+1 : col;
                            while (curcol < 5 && currow >= 0){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 5;
                                    aftermove.board[square] = 0;
                                    aftermove.tomove = 1; // white to move
                                    moves.push_back(aftermove);
                                } else {
                                    // piece == 2 or piece == 3
                                    break;
                                }
                                if (currow % 2){
                                    // row is odd
                                    curcol++;
                                }
                                currow--;
                            }

                            // left forward
                            currow = row+1;
                            curcol = (row % 2 == 0) ? col-1 : col;
                            while (curcol >= 0 && currow < 10){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 5;
                                    aftermove.board[square] = 0;
                                    aftermove.tomove = 1; // white to move
                                    moves.push_back(aftermove);
                                } else {
                                    // piece == 2 or piece == 3
                                    break;
                                }
                                if (currow % 2 == 0){
                                    // row is even
                                    curcol--;
                                }
                                currow++;
                            }

                            // left backward
                            currow = row-1;
                            curcol = (row % 2 == 0) ? col-1 : col;
                            while (curcol >= 0 && currow >= 0){
                                if (curpos.board[currow*5+curcol] == 0){
                                    // this is a possible move
                                    position aftermove = curpos;
                                    aftermove.board[currow*5+curcol] = 5;
                                    aftermove.board[square] = 0;
                                    aftermove.tomove = 1; // white to move
                                    moves.push_back(aftermove);
                                } else {
                                    // piece == 2 or piece == 3
                                    break;
                                }
                                if (currow % 2 == 0){
                                    // row is even
                                    curcol--;
                                }
                                currow--;
                            }
                        }
                    }
                }
            }
        }
    #elif board_size == 52
        
    #elif board_size == 26
        // todo
    #else
        // should never reach here
        assert(0);
    #endif
    return moves;
}

int basic_static_eval(position curpos){
    // evaluate the position statically
    #if board_size == 51
        int eval = 0;
        for (int i=0; i<50; ++i){
            if (curpos.board[i] == 2){
                eval += BASIC_PAWN_VALUE;
            } else if (curpos.board[i] == 3){
                eval += BASIC_KINGPAWN_VALUE;
            } else if (curpos.board[i] == 4){
                eval -= BASIC_PAWN_VALUE;
            } else if (curpos.board[i] == 5){
                eval -= BASIC_KINGPAWN_VALUE;
            }
        }
        return eval;
    #elif board_size == 52
        
    #elif board_size == 26
        // todo
    #else
        // should never reach here
        assert(0);
    #endif
}

int static_eval(position curpos){
    // evaluate the position statically
    #if board_size == 51
        int abs_white_val = 0;
        int abs_black_val = 0;
        byte whitepawn_count = 0;
        byte whitekingpawn_count = 0;
        byte blackpawn_count = 0;
        byte blackkingpawn_count = 0;
        for (int i=0; i<50; ++i){
            if (curpos.board[i] == 2){
                abs_white_val += PAWN_VALUE;
                whitepawn_count++;
            } else if (curpos.board[i] == 3){
                abs_white_val += KINGPAWN_VALUE;
                whitekingpawn_count++;
            } else if (curpos.board[i] == 4){
                abs_black_val += PAWN_VALUE;
                blackpawn_count++;
            } else if (curpos.board[i] == 5){
                abs_black_val += KINGPAWN_VALUE;
                blackkingpawn_count++;
            }
        }

        if ((whitepawn_count + whitekingpawn_count) == 0){
            return -GAMEOVEREVAL;
        } else if ((blackpawn_count + blackkingpawn_count) == 0){
            return GAMEOVEREVAL;
        }

        if ((whitepawn_count + blackpawn_count) >= 36 && (whitekingpawn_count + blackkingpawn_count) == 0){
            // start game

            // the side having the pawn on the center are more advantageous
            // if they have pawn on the center of the opponent's terrority they are even more advantageous
            if (curpos.board[21] == 2){
                abs_white_val += CENTER_ADVANTAGE;
            } else if (curpos.board[21] == 4){
                abs_black_val += OPPOSITE_CENTER_ADVANTAGE;
            }

            if (curpos.board[22] == 2){
                abs_white_val += CENTER_ADVANTAGE;
            } else if (curpos.board[22] == 4){
                abs_black_val += OPPOSITE_CENTER_ADVANTAGE;
            }

            if (curpos.board[23] == 2){
                abs_white_val += CENTER_ADVANTAGE;
            } else if (curpos.board[23] == 4){
                abs_black_val += OPPOSITE_CENTER_ADVANTAGE;
            }

            if (curpos.board[26] == 2){
                abs_white_val += OPPOSITE_CENTER_ADVANTAGE;
            } else if (curpos.board[26] == 4){
                abs_black_val += CENTER_ADVANTAGE;
            }

            if (curpos.board[27] == 2){
                abs_white_val += OPPOSITE_CENTER_ADVANTAGE;
            } else if (curpos.board[27] == 4){
                abs_black_val += CENTER_ADVANTAGE;
            }

            if (curpos.board[28] == 2){
                abs_white_val += OPPOSITE_CENTER_ADVANTAGE;
            } else if (curpos.board[28] == 4){
                abs_black_val += CENTER_ADVANTAGE;
            }

            // if one of the side is able to invade the opponent's side + corner (that piece is safe) it's really difficult
            // for the opponent to play

            if (curpos.board[30] == 2){
                abs_white_val += CORNER_INVASION_ADVANTAGE;
            }

            if (curpos.board[39] == 2){
                abs_white_val += CORNER_INVASION_ADVANTAGE;
            }

            if (curpos.board[40] == 2){
                abs_white_val += CORNER_INVASION_ADVANTAGE;
            }

            if (curpos.board[49] == 2){
                abs_white_val += CORNER_INVASION_ADVANTAGE;
            }

            if (curpos.board[19] == 4){
                abs_black_val += CORNER_INVASION_ADVANTAGE;
            }

            if (curpos.board[10] == 4){
                abs_black_val += CORNER_INVASION_ADVANTAGE;
            }

            if (curpos.board[9] == 4){
                abs_black_val += CORNER_INVASION_ADVANTAGE;
            }

            if (curpos.board[0] == 4){
                abs_black_val += CORNER_INVASION_ADVANTAGE;
            }

            byte forwardpawn_count = 0;
            for (int i=31; i<50; ++i){
                if (curpos.board[i] == 2) forwardpawn_count++;
            }
            abs_white_val += (forwardpawn_count / 2) * INVASION_ADVANTAGE; // for every 2 piece there is one invasion advantage

            forwardpawn_count = 0;
            for (int i=0; i<19; ++i){
                if (curpos.board[i] == 4) forwardpawn_count++;
            }
            abs_black_val += (forwardpawn_count / 2) * INVASION_ADVANTAGE; // for every 2 piece there is one invasion advantage
            
            // if back row is not moved then it is more advantageous
            if (curpos.board[0] == 2) abs_white_val += BACK_ROW_IN_PLACE;
            if (curpos.board[1] == 2) abs_white_val += BACK_ROW_IN_PLACE;
            if (curpos.board[2] == 2) abs_white_val += BACK_ROW_IN_PLACE;
            if (curpos.board[3] == 2) abs_white_val += BACK_ROW_IN_PLACE;
            if (curpos.board[4] == 2) abs_white_val += BACK_ROW_IN_PLACE;

            // if back row is not moved then it is more advantageous
            if (curpos.board[0] == 4) abs_black_val += BACK_ROW_IN_PLACE;
            if (curpos.board[1] == 4) abs_black_val += BACK_ROW_IN_PLACE;
            if (curpos.board[2] == 4) abs_black_val += BACK_ROW_IN_PLACE;
            if (curpos.board[3] == 4) abs_black_val += BACK_ROW_IN_PLACE;
            if (curpos.board[4] == 4) abs_black_val += BACK_ROW_IN_PLACE;

        } else if ((whitepawn_count + blackpawn_count) >= 20 && (whitekingpawn_count + blackkingpawn_count) <= 3){
            // mid game

            // all the start game advantage are here
            // but some more are required because the game now has king pawn

            // the side having the pawn on the center are more advantageous
            // if they have pawn on the center of the opponent's terrority they are even more advantageous
            if (curpos.board[21] == 2){
                abs_white_val += CENTER_ADVANTAGE;
            } else if (curpos.board[21] == 4){
                abs_black_val += OPPOSITE_CENTER_ADVANTAGE;
            }

            if (curpos.board[22] == 2){
                abs_white_val += CENTER_ADVANTAGE;
            } else if (curpos.board[22] == 4){
                abs_black_val += OPPOSITE_CENTER_ADVANTAGE;
            }

            if (curpos.board[23] == 2){
                abs_white_val += CENTER_ADVANTAGE;
            } else if (curpos.board[23] == 4){
                abs_black_val += OPPOSITE_CENTER_ADVANTAGE;
            }

            if (curpos.board[26] == 2){
                abs_white_val += OPPOSITE_CENTER_ADVANTAGE;
            } else if (curpos.board[26] == 4){
                abs_black_val += CENTER_ADVANTAGE;
            }

            if (curpos.board[27] == 2){
                abs_white_val += OPPOSITE_CENTER_ADVANTAGE;
            } else if (curpos.board[27] == 4){
                abs_black_val += CENTER_ADVANTAGE;
            }

            if (curpos.board[28] == 2){
                abs_white_val += OPPOSITE_CENTER_ADVANTAGE;
            } else if (curpos.board[28] == 4){
                abs_black_val += CENTER_ADVANTAGE;
            }

            // if one of the side is able to invade the opponent's side + corner (that piece is safe) it's really difficult
            // for the opponent to play

            if (curpos.board[30] == 2){
                abs_white_val += CORNER_INVASION_ADVANTAGE;
            }

            if (curpos.board[39] == 2){
                abs_white_val += CORNER_INVASION_ADVANTAGE;
            }

            if (curpos.board[40] == 2){
                abs_white_val += CORNER_INVASION_ADVANTAGE;
            }

            if (curpos.board[49] == 2){
                abs_white_val += CORNER_INVASION_ADVANTAGE;
            }

            if (curpos.board[19] == 4){
                abs_black_val += CORNER_INVASION_ADVANTAGE;
            }

            if (curpos.board[10] == 4){
                abs_black_val += CORNER_INVASION_ADVANTAGE;
            }

            if (curpos.board[9] == 4){
                abs_black_val += CORNER_INVASION_ADVANTAGE;
            }

            if (curpos.board[0] == 4){
                abs_black_val += CORNER_INVASION_ADVANTAGE;
            }

            byte forwardpawn_count = 0;
            for (int i=31; i<50; ++i){
                if (curpos.board[i] == 2) forwardpawn_count++;
            }
            abs_white_val += (forwardpawn_count / 2) * INVASION_ADVANTAGE; // for every 2 piece there is one invasion advantage

            forwardpawn_count = 0;
            for (int i=0; i<19; ++i){
                if (curpos.board[i] == 4) forwardpawn_count++;
            }
            abs_black_val += (forwardpawn_count / 2) * INVASION_ADVANTAGE; // for every 2 piece there is one invasion advantage
            
            // if back row is not moved then it is more advantageous
            if (curpos.board[0] == 2) abs_white_val += BACK_ROW_IN_PLACE;
            if (curpos.board[1] == 2) abs_white_val += BACK_ROW_IN_PLACE;
            if (curpos.board[2] == 2) abs_white_val += BACK_ROW_IN_PLACE;
            if (curpos.board[3] == 2) abs_white_val += BACK_ROW_IN_PLACE;
            if (curpos.board[4] == 2) abs_white_val += BACK_ROW_IN_PLACE;

            // if back row is not moved then it is more advantageous
            if (curpos.board[0] == 4) abs_black_val += BACK_ROW_IN_PLACE;
            if (curpos.board[1] == 4) abs_black_val += BACK_ROW_IN_PLACE;
            if (curpos.board[2] == 4) abs_black_val += BACK_ROW_IN_PLACE;
            if (curpos.board[3] == 4) abs_black_val += BACK_ROW_IN_PLACE;
            if (curpos.board[4] == 4) abs_black_val += BACK_ROW_IN_PLACE;


            // connected pieces + pawn pushing
            // king pawn should be more in the corner
            for (int i=0; i<50; ++i){
                if (curpos.board[i] == 2){
                    // white pawn
                    abs_white_val += (i/5) * ENDGAME_PAWN_PUSH; // greater is the row of the pawn more advantageous is the white
                    // now the advantage for connected pawns
                    if (i/5 & 1){
                        // row is even
                        if (i > 6){
                            if (curpos.board[i-6] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                            if (curpos.board[i-5] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                        } else if (i == 5){
                            if (curpos.board[i-5] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                        }

                        if (i < 44){
                            if (curpos.board[i+5] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                            if (curpos.board[i+4] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                        } else if (i == 45){
                            if (curpos.board[i+4] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                        }
                    } else {
                        // row is odd
                        if (i > 5){
                            if (curpos.board[i-5] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                            if (curpos.board[i-4] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                        } else if (i == 4){
                            if (curpos.board[i-4] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                        }

                        if (i < 43){
                            if (curpos.board[i+6] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                            if (curpos.board[i+5] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                        } else if (i == 44){
                            if (curpos.board[i+5] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                        }
                    }

                } else if (curpos.board[i] == 4){
                    // black pawn
                    abs_black_val += (9-(i/5)) * ENDGAME_PAWN_PUSH; // lesser is the row of the pawn more advantageous is the white
                    // now the advantage for connected pawns
                    if (i/5 & 1){
                        // row is even
                        if (i > 6){
                            if (curpos.board[i-6] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                            if (curpos.board[i-5] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                        } else if (i == 5){
                            if (curpos.board[i-5] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                        }

                        if (i < 44){
                            if (curpos.board[i+5] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                            if (curpos.board[i+4] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                        } else if (i == 45){
                            if (curpos.board[i+4] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                        }
                    } else {
                        // row is odd
                        if (i > 5){
                            if (curpos.board[i-5] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                            if (curpos.board[i-4] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                        } else if (i == 4){
                            if (curpos.board[i-4] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                        }

                        if (i < 43){
                            if (curpos.board[i+6] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                            if (curpos.board[i+5] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                        } else if (i == 44){
                            if (curpos.board[i+5] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                        }
                    }

                } else if (curpos.board[i] == 3){
                    // white king pawn
                    // it should be more in the corner as to avoid any forced captures
                    byte row = i/5;
                    byte col = i%5;
                    if (row == 9 || row == 0 || (row % 2 && col == 5) || (!(row % 2) && col == 0)){
                        // the piece is in the corner
                        abs_white_val += MIDGAME_KING_IN_CORNER;
                    }
                } else if (curpos.board[i] == 4){
                    // black king pawn
                    // it should be more in the corner as to avoid any forced captures
                    byte row = i/5;
                    byte col = i%5;
                    if (row == 9 || row == 0 || (row % 2 && col == 5) || (!(row % 2) && col == 0)){
                        // the piece is in the corner
                        abs_black_val += MIDGAME_KING_IN_CORNER;
                    }
                }
            }

        } else {
            // end game
            abs_white_val += whitekingpawn_count * ENDGAME_KINGPAWN_VALUE_INCREMENT;
            abs_black_val += blackkingpawn_count * ENDGAME_KINGPAWN_VALUE_INCREMENT;

            // some specific endgames whose result we know
            // n kings vs n-1 kings can't win (unless there is a specific position)
            // 2 kings vs 1 king can't win (unless there is a specific position)
            // 3 kings vs 1 king can't win (unless that one king is in the corner and the other king is at the other corner)
            // 4 kings vs 1 king can win
            // n kings vs n-2 kings can win (n > 3)
            if ((whitepawn_count + blackpawn_count) == 0){
                if ((whitekingpawn_count == 1 && blackkingpawn_count == 2) || (blackkingpawn_count == 1 && whitekingpawn_count == 2)){
                    return 0; // a draw situation
                }

                if ((whitekingpawn_count == 1 && blackkingpawn_count == 3)){
                    if ((curpos.board[0] == 3 && curpos.board[49] == 5) || (curpos.board[0] == 5 && curpos.board[49] == 3)){
                        // it is winning for black
                        return -ENDGAME_POSSIBLE_GAMEOVER;
                    } else {
                        return 0;
                    }
                }
                if ((blackkingpawn_count == 1 && whitekingpawn_count == 3)){
                    if ((curpos.board[0] == 3 && curpos.board[49] == 5) || (curpos.board[0] == 5 && curpos.board[49] == 3)){
                        // it is winning for white
                        return ENDGAME_POSSIBLE_GAMEOVER;
                    } else {
                        return 0;
                    }
                }

                if (std::max(whitekingpawn_count, blackkingpawn_count) >= 4){
                    if (((whitekingpawn_count - blackkingpawn_count) <= 1) || ((blackkingpawn_count - whitekingpawn_count) <= 1)){
                        return 0;
                    } else if (((whitekingpawn_count - blackkingpawn_count) > 1)){
                        return ENDGAME_POSSIBLE_GAMEOVER; // white winning possibly
                    } else if (((blackkingpawn_count - whitekingpawn_count) > 1)){
                        return -ENDGAME_POSSIBLE_GAMEOVER; // black winning possibly
                    }
                }
            }


            // some specific strategies

            // pushing the pawn towards the end of the board
            // and the king pawn should be in the center as much possible
            // and also advantage for connected pawns
            for (int i=0; i<50; ++i){
                if (curpos.board[i] == 2){
                    // white pawn
                    abs_white_val += (i/5) * ENDGAME_PAWN_PUSH; // greater is the row of the pawn more advantageous is the white
                    // now the advantage for connected pawns
                    if (i/5 & 1){
                        // row is even
                        if (i > 6){
                            if (curpos.board[i-6] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                            if (curpos.board[i-5] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                        } else if (i == 5){
                            if (curpos.board[i-5] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                        }

                        if (i < 44){
                            if (curpos.board[i+5] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                            if (curpos.board[i+4] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                        } else if (i == 45){
                            if (curpos.board[i+4] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                        }
                    } else {
                        // row is odd
                        if (i > 5){
                            if (curpos.board[i-5] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                            if (curpos.board[i-4] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                        } else if (i == 4){
                            if (curpos.board[i-4] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                        }

                        if (i < 43){
                            if (curpos.board[i+6] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                            if (curpos.board[i+5] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                        } else if (i == 44){
                            if (curpos.board[i+5] == 2) abs_white_val += ENDGAME_CONNECTED_PAWN;
                        }
                    }

                } else if (curpos.board[i] == 4){
                    // black pawn
                    abs_black_val += (9-(i/5)) * ENDGAME_PAWN_PUSH; // lesser is the row of the pawn more advantageous is the white
                    // now the advantage for connected pawns
                    if (i/5 & 1){
                        // row is even
                        if (i > 6){
                            if (curpos.board[i-6] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                            if (curpos.board[i-5] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                        } else if (i == 5){
                            if (curpos.board[i-5] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                        }

                        if (i < 44){
                            if (curpos.board[i+5] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                            if (curpos.board[i+4] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                        } else if (i == 45){
                            if (curpos.board[i+4] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                        }
                    } else {
                        // row is odd
                        if (i > 5){
                            if (curpos.board[i-5] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                            if (curpos.board[i-4] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                        } else if (i == 4){
                            if (curpos.board[i-4] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                        }

                        if (i < 43){
                            if (curpos.board[i+6] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                            if (curpos.board[i+5] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                        } else if (i == 44){
                            if (curpos.board[i+5] == 4) abs_black_val += ENDGAME_CONNECTED_PAWN;
                        }
                    }

                } else if (curpos.board[i] == 3){
                    // white king pawn
                    // it should be more in the center
                    abs_white_val += (6 - std::min(abs(i/5-5) + abs(i%5-3), abs(i/5-4) + abs(i%5-3))) * ENDGAME_KING_IN_CENTER;
                } else if (curpos.board[i] == 4){
                    // black king pawn
                    // it should be more in the center
                    abs_black_val += (6 - std::min(abs(i/5-5) + abs(i%5-3), abs(i/5-4) + abs(i%5-3))) * ENDGAME_KING_IN_CENTER;
                }
            }

        }

        return (abs_white_val - abs_black_val);
    #elif board_size == 52
        
    #elif board_size == 26
        // todo
    #else
        // should never reach here
        assert(0);
    #endif
}

byte isgameover(position curpos){
    #if board_size == 51
        uint32_t whitepiece = 0;
        uint32_t blackpiece = 0;
        for (int i=0; i<50; ++i){
            if (curpos.board[i] == 2){
                whitepiece++;
            } else if (curpos.board[i] == 3){
                whitepiece++;
            } else if (curpos.board[i] == 4){
                blackpiece++;
            } else if (curpos.board[i] == 5){
                blackpiece++;
            }
        }

        if (whitepiece != 0 && blackpiece != 0){
            return 0; // game is not over
        } else {
            if (whitepiece == 0){
                return -1; // black won
            } else {
                return 1; // white won
            }
        }
    #elif board_size == 52
        uint32_t whitepiece = 0;
        uint32_t blackpiece = 0;
        for (int i=0; i<50; ++i){
            if (curpos.board[i] == 2){
                whitepiece++;
            } else if (curpos.board[i] == 3){
                whitepiece++;
            } else if (curpos.board[i] == 4){
                blackpiece++;
            } else if (curpos.board[i] == 5){
                blackpiece++;
            }
        }

        if (whitepiece != 0 && blackpiece != 0){
            return 0; // game is not over
        } else {
            if (whitepiece == 0){
                return -1; // black won
            } else {
                return 1; // white won
            }
        }
    #elif board_size == 26
        // todo
    #else
        // should never reach here
        assert(0);
    #endif
}
