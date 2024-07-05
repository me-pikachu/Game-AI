#include <bits/stdc++.h>

#define byte char

const int GAMEOVEREVAL = 10;

struct position{
    byte board[9];
    // 0 for empty
    // 2 for cross (X)
    // 4 for zero (O)
    // rest all are invalid
    byte tomove; // 0 for zero (O) and 1 for cross (X)
};

struct move{
    byte initialsq;
    byte finalsq;
    move(){
        initialsq = -1;
        finalsq = -1;
    }

    move(int sq){
        // a constructor
        initialsq = -1;
        finalsq = sq;
    }
    
    move(int sq1, int sq2){
        initialsq = sq1;
        finalsq = sq2;
    }
};

void print_board(position pos){
    for (int i=0; i<9; ++i){
        if (i%3 == 0){
            std::cout << std::endl;
        }
        
        if (pos.board[i] == 0){
            std::cout << ". ";
        } else if (pos.board[i] == 2){
            std::cout << "X ";
        } else if (pos.board[i] == 4){
            std::cout << "O ";
        } else {
            std::cout << "Not a valid entry: " << pos.board[i] << std::endl;
            assert(0);
        }
    }
    std::cout << std::endl;
}

std::vector<std::pair<move, position>> get_all_moves(position curpos){
    std::vector<std::pair<move, position>> moves;
    // if anyone of them one then return an empty vector
    // firstly checking whether X is the winner or not
    if (curpos.board[0] == 2 && curpos.board[1] == 2 && curpos.board[2] == 2){
		return moves;
	} else if (curpos.board[3] == 2 && curpos.board[4] == 2 && curpos.board[5] == 2){
		return moves;
	} else if (curpos.board[6] == 2 && curpos.board[7] == 2 && curpos.board[8] == 2){
		return moves;
	}
	
	// checking column wise
	if (curpos.board[0] == 2 && curpos.board[3] == 2 && curpos.board[6] == 2){
		return moves;
	} else if (curpos.board[1] == 2 && curpos.board[4] == 2 && curpos.board[7] == 2){
		return moves;
	} else if (curpos.board[2] == 2 && curpos.board[5] == 2 && curpos.board[8] == 2){
		return moves;
	}
	
	// checking diagonally
	// there are only two diagonals
	if (curpos.board[0] == 2 && curpos.board[4] == 2 && curpos.board[8] == 2){
		return moves;
	} else if (curpos.board[2] == 2 && curpos.board[4] == 2 && curpos.board[6] == 2){
		return moves;
	}
	
	// secondly checking whether O is the winner or not
	
	// checking row wise
	if (curpos.board[0] == 4 && curpos.board[1] == 4 && curpos.board[2] == 4){
		return moves;
	} else if (curpos.board[3] == 4 && curpos.board[4] == 4 && curpos.board[5] == 4){
		return moves;
	} else if (curpos.board[6] == 4 && curpos.board[7] == 4 && curpos.board[8] == 4){
		return moves;
	}
	
	// checking column wise
	if (curpos.board[0] == 4 && curpos.board[3] == 4 && curpos.board[6] == 4){
		return moves;
	} else if (curpos.board[1] == 4 && curpos.board[4] == 4 && curpos.board[7] == 4){
		return moves;
	} else if (curpos.board[2] == 4 && curpos.board[5] == 4 && curpos.board[8] == 4){
		return moves;
	}
	
	// checking diagonally
	// there are only two diagonals
	if (curpos.board[0] == 4 && curpos.board[4] == 4 && curpos.board[8] == 4){
		return moves;
	} else if (curpos.board[2] == 4 && curpos.board[4] == 4 && curpos.board[6] == 4){
		return moves;
	}

    // now no is currently the winner
    if (curpos.tomove){
        // it's cross to move
        for (int i=0; i<9; ++i){
            if (curpos.board[i] == 0){
                position newpos = curpos;
                newpos.board[i] = 2;
                newpos.tomove = 0;
                moves.push_back({move(i), newpos});
            }
        }
    } else {
        // it's zero to move
        for (int i=0; i<9; ++i){
            if (curpos.board[i] == 0){
                position newpos = curpos;
                newpos.board[i] = 4;
                newpos.tomove = 1;
                moves.push_back({move(i), newpos});
            }
        }
    }
    return moves;
}

int basic_static_eval(position curpos){
    // check whether the game is over or not
    // if X won the game then it returns 1
    // if O won the game then it returns -1
    // else it returns 0
    
    // firstly checking whether X is the winner or not
    if (curpos.board[0] == 2 && curpos.board[1] == 2 && curpos.board[2] == 2){
		return 1;
	} else if (curpos.board[3] == 2 && curpos.board[4] == 2 && curpos.board[5] == 2){
		return 1;
	} else if (curpos.board[6] == 2 && curpos.board[7] == 2 && curpos.board[8] == 2){
		return 1;
	}
	
	// checking column wise
	if (curpos.board[0] == 2 && curpos.board[3] == 2 && curpos.board[6] == 2){
		return 1;
	} else if (curpos.board[1] == 2 && curpos.board[4] == 2 && curpos.board[7] == 2){
		return 1;
	} else if (curpos.board[2] == 2 && curpos.board[5] == 2 && curpos.board[8] == 2){
		return 1;
	}
	
	// checking diagonally
	// there are only two diagonals
	if (curpos.board[0] == 2 && curpos.board[4] == 2 && curpos.board[8] == 2){
		return 1;
	} else if (curpos.board[2] == 2 && curpos.board[4] == 2 && curpos.board[6] == 2){
		return 1;
	}
	
	// secondly checking whether O is the winner or not
	
	// checking row wise
	if (curpos.board[0] == 4 && curpos.board[1] == 4 && curpos.board[2] == 4){
		return -1;
	} else if (curpos.board[3] == 4 && curpos.board[4] == 4 && curpos.board[5] == 4){
		return -1;
	} else if (curpos.board[6] == 4 && curpos.board[7] == 4 && curpos.board[8] == 4){
		return -1;
	}
	
	// checking column wise
	if (curpos.board[0] == 4 && curpos.board[3] == 4 && curpos.board[6] == 4){
		return -1;
	} else if (curpos.board[1] == 4 && curpos.board[4] == 4 && curpos.board[7] == 4){
		return -1;
	} else if (curpos.board[2] == 4 && curpos.board[5] == 4 && curpos.board[8] == 4){
		return -1;
	}
	
	// checking diagonally
	// there are only two diagonals
	if (curpos.board[0] == 4 && curpos.board[4] == 4 && curpos.board[8] == 4){
		return -1;
	} else if (curpos.board[2] == 4 && curpos.board[4] == 4 && curpos.board[6] == 4){
		return -1;
	}
	
	// now if all above are not the case then no one is the winner
	return 0;
}