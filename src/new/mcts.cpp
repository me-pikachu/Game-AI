#include <bits/stdc++.h>

#define gametype 0
// 0 for checkers
// 1 for tictactoe
// you can add more if you want.....

#if (gametype == 0)
    #include "checkers.cpp"
#elif (gametype == 1)
    #include "tic-tac-toe.cpp"
#endif

#include <chrono>
#include <thread>
#include <omp.h>

#ifndef CONSTANT_C1 // if it not defined by the compiler at compilation time
    #define CONSTANT_C1 50000 // constant representing exploration and exploitation
#endif
#ifndef SIMULATION_DEPTH // if it not defined by the compiler at compilation time
    #define SIMULATION_DEPTH 0 // the depth of alpha beta
#endif
#ifndef MAX_EVAL_TIME // if it not defined by the compiler at compilation time
    #define MAX_EVAL_TIME 10000 // time in milliseconds
#endif
#ifndef MAX_ROOT_VISIT // if it not defined by the compiler at compilation time
    #define MAX_ROOT_VISIT INT_MAX // the no of root visits
#endif

#define timenow std::chrono::high_resolution_clock::now()
#define convert_to_milli(diff) std::chrono::duration_cast<std::chrono::milliseconds>(diff).count()

#define multithreading
#define showdepth

struct node{
    unsigned int n = 0; // no of visits to this node
    long long total_value = 0; // total_value
    void* child = nullptr;
    bool ismax = 1;
    // this child pointer either points to the vector having it's child node or to the current node's position (board, sidetomove)
    // if n==0 or n==1 it's a leaf node and hence the child pointer point's to the position else it points towards it's child
};

void free_mem(node& curnode){
    if (curnode.n == 0 || curnode.n == 1){
        // it is a leaf node and it has board in it's child
        if (curnode.child != nullptr) delete static_cast<position*>(curnode.child);
    } else {
        if (curnode.child != nullptr){
            auto child = static_cast<std::vector<node>*>(curnode.child);
            for (auto& i : *child){
                free_mem(i);
            }
            child->clear();
        }
    }
}

struct evaluation{
    move bestmove;
    int value = 0;
};

struct complete_eval{
    move bestmove;
    int value = 0;
    byte gameoverdepth = 0;
};

struct tt_entry{
    byte depth = -1;
    int value = 0;
    std::vector<position> move_order;
};

// alpha beta code goes here ........
int alphabeta(position& curpos, byte depth, const byte& max_depth, int alpha, int beta, std::unordered_map<position, byte, position_hash>& prev_pos, std::unordered_map<position, tt_entry, position_hash>& transposition_table){

    auto it_find = prev_pos.find(curpos);
    if (it_find != prev_pos.end()){
        if (it_find->second <= depth){
            return 0; // repeated position no need to evaluate since there is a better possibility out there
        } else {
            // curpos is there is in the tree but is at higher depths
            it_find->second = std::min(it_find->second, depth);
        }
    } else {
        prev_pos[curpos] = depth;
    }

    auto it = transposition_table.find(curpos);
    if (it != transposition_table.end()){
        if (it->second.depth >= (max_depth-depth)){
            return it->second.value;
        }
    }

    // we would create the children nodes first
    auto moves = get_all_position(curpos);
    if (moves.size() == 0){
        // if the moves size is 0 then it means that either it's a draw or one of the player won
        // the static evaluation should return zero in case of a draw
        if (static_eval(curpos) == 0){
            // it's a draw
            return 0;
        } else {
            // one of the side won
            if (curpos.tomove & 1){
                // it is white to move
                return -GAMEOVEREVAL;
            } else {
                // it is black to move
                return GAMEOVEREVAL;
            }
        }
    }

    position newpos = curpos;
    while (moves.size() == 1){
        newpos = moves[0];
        moves.clear();
        moves = get_all_position(newpos);
        if (moves.size() == 0){
            // if the moves size is 0 then it means that either it's a draw or one of the player won
            // the static evaluation should return zero in case of a draw
            if (static_eval(newpos) == 0){
                // it's a draw
                return 0;
            } else {
                // one of the side won
                if (newpos.tomove & 1){
                    // it is white to move
                    return -GAMEOVEREVAL;
                } else {
                    // it is black to move
                    return GAMEOVEREVAL;
                }
            }
        }
    }

    if (depth == max_depth){
        // we are at root node
        return static_eval(newpos);
    }

    if (newpos.tomove & 1){
        // firstly do the move reordering
        

        // the player is maximiser if it is white
        int bestval = -GAMEOVEREVAL;
        int curval;
        int bestindex = -1;

        std::map<int, position> move_order;
        // using OpenMP for parallel processing
        #pragma omp parallel for schedule(dynamic)
        for (int i=0; i<moves.size(); ++i){

            curval = alphabeta(moves[i], depth+1, max_depth, alpha, beta, prev_pos, transposition_table);

            #pragma omp critical
            {
                move_order[curval] = moves[i];
            }

            if (curval > bestval && curval > alpha){
                #pragma omp critical
                {
                    // omp critical ensures that this lines of code are executed by
                    // only a single thread at a time to ensure safely updating of shared variables
                    // we would use critical only at the time of writing of shared variables
                    bestval = curval;
                    bestindex = i;
                    alpha = curval;
                }
            } else if (curval > bestval && curval <= alpha){
                #pragma omp critical
                {
                    // omp critical ensures that this lines of code are executed by
                    // only a single thread at a time to ensure safely updating of shared variables
                    // we would use critical only at the time of writing of shared variables
                    bestval = curval;
                    bestindex = i;
                }
            } else if (curval <= bestval && bestval > alpha){
                #pragma omp critical
                {
                    // omp critical ensures that this lines of code are executed by
                    // only a single thread at a time to ensure safely updating of shared variables
                    // we would use critical only at the time of writing of shared variables
                    alpha = bestval;
                }
            }

            if (alpha >= beta){
                break;
            }
        }
        
        if (bestval == GAMEOVEREVAL || bestval == -GAMEOVEREVAL){
            // no need for visiting this node again
            transposition_table[curpos].depth = 127; // to stop exploring more in this tree

        } else {
            // we now need to have the move order for current position
            for (auto i=move_order.begin(); i!=move_order.end(); ++i){
                transposition_table[curpos].move_order.push_back(i->second);
            }

            // clear all the move order for all the position except the best from current position
            for (int i=0; i<moves.size(); ++i){
                if (i == bestindex) continue;
                transposition_table[moves[i]].move_order.clear();
            }

            transposition_table[curpos].depth = (max_depth-depth);
        }
        transposition_table[curpos].value = bestval;
        
        return bestval;

    } else {
        // it is the minimiser player or black
        int bestval = GAMEOVEREVAL;
        int curval;
        int bestindex = -1;

        std::map<int, position> move_order;
        // using OpenMP for multithreading
        #pragma omp parallel for schedule(dynamic) 
        for (int i=0; i<moves.size(); ++i){

            curval = alphabeta(moves[i], depth+1, max_depth, alpha, beta, prev_pos, transposition_table);

            #pragma omp critical
            {
                move_order[curval] = moves[i];
            }

            if (curval < bestval && curval < beta){
                #pragma omp critical
                {
                    // omp critical ensures that this lines of code are executed by
                    // only a single thread at a time to ensure safely updating of shared variables
                    // we would use critical only at the time of writing of shared variables
                    bestval = curval;
                    bestindex = i;
                    beta = curval;
                }
            } else if (curval < bestval && curval >= beta){
                #pragma omp critical
                {
                    // omp critical ensures that this lines of code are executed by
                    // only a single thread at a time to ensure safely updating of shared variables
                    // we would use critical only at the time of writing of shared variables
                    bestval = curval;
                    bestindex = i;
                }
            } else if (curval >= bestval && bestval < beta){
                #pragma omp critical
                {
                    // omp critical ensures that this lines of code are executed by
                    // only a single thread at a time to ensure safely updating of shared variables
                    // we would use critical only at the time of writing of shared variables
                    beta = bestval;
                }
            }

            if (alpha >= beta){
                break;
            }
        }

        if (bestval == GAMEOVEREVAL || bestval == -GAMEOVEREVAL){
            // no need for visiting this node again
            transposition_table[curpos].depth = 127; // to stop exploring more in this tree

        } else {
            // we now need to have the move order for current position
            for (auto i=move_order.begin(); i!=move_order.end(); ++i){
                transposition_table[curpos].move_order.push_back(i->second);
            }

            // clear all the move order for all the position except the best from current position
            for (int i=0; i<moves.size(); ++i){
                if (i == bestindex) continue;
                transposition_table[moves[i]].move_order.clear();
            }

            transposition_table[curpos].depth = (max_depth-depth);
        }
        transposition_table[curpos].value = bestval;
        
        return bestval;
    }
}

evaluation alphabeta_root(position& root, const byte& max_depth, std::unordered_map<position, tt_entry, position_hash>& transposition_table){
    // current depth is 0
    // we are currently at the root node
    evaluation res;
    auto moves = get_all_moves(root);
    if (moves.size() == 0){
        // if the moves size is 0 then it means that either it's a draw or one of the player won
        // the static evaluation should return zero in case of a draw
        res.bestmove = {-1, -1};
        if (static_eval(root) == 0){
            // it's a draw
            res.value = 0;
            return res;
        } else {
            // one of the side won
            if (root.tomove & 1){
                // it is white to move
                res.value = -GAMEOVEREVAL;
                return res;
            } else {
                // it is black to move
                res.value = GAMEOVEREVAL;
                return res;
            }
        }
    }
    
    res.bestmove = moves[0].first;
    root = moves[0].second;
    bool wassize1 = false;
    while (moves.size() == 1){
        wassize1 = true;
        root = moves[0].second;
        moves.clear();
        moves = get_all_moves(root);
        if (moves.size() == 0){
            // if the moves size is 0 then it means that either it's a draw or one of the player won
            // the static evaluation should return zero in case of a draw
            if (static_eval(root) == 0){
                // it's a draw
                res.value = 0;
                return res;
            } else {
                // one of the side won
                if (root.tomove & 1){
                    // it is white to move
                    res.value = -GAMEOVEREVAL;
                    return res;
                } else {
                    // it is black to move
                    res.value = GAMEOVEREVAL;
                    return res;
                }
            }
        }
    }

    std::unordered_map<position, byte, position_hash> prev_pos;
    prev_pos[root] = 0; // root is at depth 0

    if (root.tomove & 1){
        // the player is maximiser if it is white
        res.value = -GAMEOVEREVAL;
        int curval;
        int alpha = -GAMEOVEREVAL;
        int beta = GAMEOVEREVAL;

        // using OpenMP for multithreading
        #pragma omp parallel for schedule(dynamic) 
        for (int i=0; i<moves.size(); ++i){
            curval = alphabeta(moves[i].second, 1, max_depth, alpha, beta, prev_pos, transposition_table);
            if (curval > res.value){
                #pragma omp critical
                {
                    // omp critical ensures that this lines of code are executed by
                    // only a single thread at a time to ensure safely updating of shared variables
                    // we would use critical only at the time of writing of shared variables
                    res.value = curval;
                    if (!wassize1){
                        res.bestmove = moves[i].first;
                        root = moves[i].second;
                    }
                }
            }

            if (res.value > alpha){
                #pragma omp critical
                {
                    // omp critical ensures that this lines of code are executed by
                    // only a single thread at a time to ensure safely updating of shared variables
                    // we would use critical only at the time of writing of shared variables
                    alpha = res.value;
                }
            }
            
            if (alpha >= beta){
                break;
            }
        }

        return res;

    } else {
        // it is the minimiser player or black
        res.value = GAMEOVEREVAL;
        int curval;
        byte gameover_depth = -1;
        int alpha = -GAMEOVEREVAL;
        int beta = GAMEOVEREVAL;

        // using OpenMP for multithreading
        #pragma omp parallel for schedule(dynamic) 
        for (int i=0; i<moves.size(); ++i){

            curval = alphabeta(moves[i].second, 1, max_depth, alpha, beta, prev_pos, transposition_table);
            if (curval < res.value){
                #pragma omp critical
                {
                    // omp critical ensures that this lines of code are executed by
                    // only a single thread at a time to ensure safely updating of shared variables
                    // we would use critical only at the time of writing of shared variables
                    res.value = curval;
                    if (!wassize1){
                        res.bestmove = moves[i].first;
                        root = moves[i].second; // update the root here
                    }
                }
            }

            if (res.value < beta){
                #pragma omp critical
                {
                    // omp critical ensures that this lines of code are executed by
                    // only a single thread at a time to ensure safely updating of shared variables
                    // we would use critical only at the time of writing of shared variables
                    beta = res.value;
                }
            }
            
            if (alpha >= beta){
                break;
            }
        }
        
        return res;
    }
}

complete_eval iterative_deepening(position& root, const byte& max_depth, int max_eval_time){
    auto start = timenow;
    byte curdepth = 1;
    position temp = root;
    complete_eval res;
    evaluation alpha_beta;
    std::unordered_map<position, tt_entry, position_hash> transposition_table;

    while (curdepth <= max_depth && (convert_to_milli(timenow - start) < (3 * max_eval_time / 4))){
        #ifdef showdepth
        std::cout << "Depth " << (int)curdepth << " reached" << std::endl; 
        #endif

        temp = root; // creating a copy of root
        alpha_beta = alphabeta_root(temp, curdepth, transposition_table);

        if (res.value == GAMEOVEREVAL){
            res.bestmove = alpha_beta.bestmove;
            res.value = GAMEOVEREVAL;
            res.gameoverdepth = curdepth; // this is the minimum gameover depth
            root = temp;
            return res;
        } else if (res.value == -GAMEOVEREVAL){
            res.bestmove = alpha_beta.bestmove;
            res.value = -GAMEOVEREVAL;
            res.gameoverdepth = curdepth; // this is the minimum gameover depth
            root = temp;
            return res;
        }
        curdepth++; // increasing the depth
    }

    res.bestmove = alpha_beta.bestmove;
    res.value = alpha_beta.value;
    res.gameoverdepth = -1; // this is no gameover
    root = temp; // making changes in root
    return res;

}

int simulate(position curpos){
    // this simulation process is one of the most costliest process in this mcts
    // simulate the position according to basic heuristics
    return static_eval(curpos);
}

#ifdef showdepth
byte max_reached_depth = 0;
#endif

int traverse(node& curnode, std::unordered_map<position, byte, position_hash>& prev_pos, byte cur_reached_depth = 0){
    // prev_pos stores a list of previous positions which is helpful in case ...
    // ... there is move repititions leading to a draw situation
    // we have to select one of the child node based upon UCB1 value
    auto child = static_cast<std::vector<node>*>(curnode.child); // getting the children's node vector
    float UCB1val = INT_MIN;
    float curUCB1val;
    node* selnode = nullptr;
    for (auto& i:*child){
        if (i.n == 0){
            // we have to perform the simulation here
            // this node has the highest priority among it's brothers
            // if this position is already present in the tree (repetition of moves) ...
            // ... no need to simulate just return 0 (draw)
            if (prev_pos.find(*(static_cast<position*>(i.child))) != prev_pos.end()){
                if (prev_pos.find(*(static_cast<position*>(i.child)))->second < cur_reached_depth){
                    // current position present in the tree
                    i.total_value = 0;
                    i.n = 2;
                    i.child = nullptr; // no need to evaluate this position ever again
                    curnode.n++;
                    return 0;
                }
            }
            // it was not in the tree continue the normal simulation

            int eval = simulate(*(static_cast<position*>(i.child)));
            i.total_value = eval;
            i.n = 1;
            curnode.n++;
            curnode.total_value += eval;

            #ifdef showdepth
            cur_reached_depth++;
            if (cur_reached_depth > max_reached_depth){
                max_reached_depth = cur_reached_depth;
                std::cout << "Depth " << (int)cur_reached_depth << " reached" << std::endl;
            }
            #endif

            return eval;
        }

        // UCB1 = avg val + C * sqrt(logN/ ni) where N is the parent's visit and ni is current node's visit
        if (curnode.ismax) curUCB1val = (i.total_value * 1.0 / i.n) + CONSTANT_C1 * sqrt(log(curnode.n) / i.n);
        else curUCB1val = -1*(i.total_value * 1.0 / i.n) + CONSTANT_C1 * sqrt(log(curnode.n) / i.n);

        if (curUCB1val > UCB1val){
            UCB1val = curUCB1val;
            selnode = &i;
        }
    }

    //  we now have the node we have to visit
    if (selnode == nullptr){
        std::cout << "No node was selected during selection phase ?!?!" << std::endl;
        assert(0);
    }

    if (selnode->n == 1){
        // we have to do expansion
        // this node has been visited only once and that for simluation purpose

        // the position was not present in the tree
        auto moves = get_all_moves(*(static_cast<position*>(selnode->child)));
        if (moves.size() == 0){
            // if the moves size is 0 then it means that either it's a draw or one of the player won
            // the static evaluation should return zero in case of a draw
            if (static_eval((*(static_cast<position*>(selnode->child)))) == 0){
                // it's a draw
                selnode->n = 2;
                selnode->child = nullptr;
                curnode.n++;
                return 0;
            } else {
                // one of the side won
                if ((*(static_cast<position*>(selnode->child))).tomove & 1){
                    // white to move originally and now white did not have any moves
                    // no node creation
                    selnode->n = 2;
                    selnode->total_value += -GAMEOVEREVAL; // it is winning for black
                    selnode->child = nullptr;
                    curnode.n++;
                    curnode.total_value += -GAMEOVEREVAL;
                    return -GAMEOVEREVAL;
                } else {
                    // black to move originally and now black did not have any moves
                    // no node creation
                    selnode->n = 2;
                    selnode->total_value += GAMEOVEREVAL; // it is winning for white
                    selnode->child = nullptr;
                    curnode.n++;
                    curnode.total_value += GAMEOVEREVAL;
                    return GAMEOVEREVAL;
                }
            }
        }

        while (moves.size() == 1){
            // if no of moves is 1 then directly replace it with it's children (of not unity size)
            position newpos = moves[0].second;
            moves.clear();
            moves = get_all_moves(newpos);
            if (moves.size() == 0){
                // it reached a game over state
                // if the moves size is 0 then it means that either it's a draw or one of the player won
                // the static evaluation should return zero in case of a draw
                if (static_eval(newpos) == 0){
                    // it's a draw
                    selnode->n = 2;
                    selnode->total_value += 0; // it is winning for black
                    selnode->child = nullptr;
                    curnode.n++;
                    curnode.total_value += 0;
                    return 0;
                } else {
                    // one of the side won
                    if (newpos.tomove & 1){
                        // white to move originally and now white did not have any moves
                        // no node creation
                        selnode->n = 2;
                        selnode->total_value += -GAMEOVEREVAL; // it is winning for black
                        selnode->child = nullptr;
                        curnode.n++;
                        curnode.total_value += -GAMEOVEREVAL;
                        return -GAMEOVEREVAL;
                    } else {
                        // black to move originally and now black did not have any moves
                        // no node creation
                        selnode->n = 2;
                        selnode->total_value += GAMEOVEREVAL; // it is winning for white
                        selnode->child = nullptr;
                        curnode.n++;
                        curnode.total_value += GAMEOVEREVAL;
                        return GAMEOVEREVAL;
                    }
                }
            }
        }

        position thispos = *(static_cast<position*>(selnode->child));
        if (prev_pos.find(thispos) != prev_pos.end()){
            // the value would be the minimum of the depths
            prev_pos[thispos] = std::min(prev_pos[thispos], (byte)(cur_reached_depth+1));
        } else {
            prev_pos[thispos] = cur_reached_depth + 1;
        }      

        auto children = new std::vector<node>(moves.size()); // creating the nodes on heap memory
        for (int i=0; i<moves.size(); ++i){
            auto newpos = new position(moves[i].second); // creating a copy of the position
            (*children)[i].child = static_cast<void*>(newpos);
            (*children)[i].ismax = (newpos->tomove & 1);
        }

        delete static_cast<position*>(selnode->child); // delete the position as it is not required now
        selnode->child = static_cast<void*>(children); // changing it's value

        int eval = traverse(*selnode, prev_pos, cur_reached_depth+1);
        curnode.n++;
        curnode.total_value = eval;
        return eval;

    } else {
        if (selnode->child == nullptr && selnode->n > 1){
            // selnode->n > 1 and selnode->child == nullptr 
            // it means that it is a game over position
            // if it is white to play then white does not have any moves to play
            // same case with black
            if (selnode->total_value > 0){
                // white was winning previously and the same result goes here
                selnode->n += 1;
                selnode->total_value += GAMEOVEREVAL;
                curnode.n++;
                curnode.total_value += GAMEOVEREVAL;
                return GAMEOVEREVAL;
            } else if (selnode->total_value == 0){
                // it was a case of draw
                selnode->n += 1;
                curnode.n++;
                return 0;
            } else {
                // black was winning previously and the same result goes here
                selnode->n += 1;
                selnode->total_value += -GAMEOVEREVAL;
                curnode.n++;
                curnode.total_value += -GAMEOVEREVAL;
                return -GAMEOVEREVAL;
            }
        } else {
            int eval = traverse(*selnode, prev_pos, cur_reached_depth+1);
            curnode.n++;
            curnode.total_value += eval;
            return eval;
        }
    }
}

evaluation MCTS(position& curpos){
    // do a monte carlo tree search (MCTS) on current position
    #ifdef showdepth
    max_reached_depth = 0;
    #endif
    auto start = timenow;
    auto moves = get_all_moves(curpos);

    if (moves.size() == 0){
        // either it's a draw or one player won the game
        if (static_eval(curpos) == 0){
            // it's a draw
            evaluation cureval;
            cureval.bestmove = {-1, -1};
            cureval.value = 0;
            return cureval;
        } else {
            // the opposite player already have won the game
            if (curpos.tomove & 1){
                // it is white to move
                evaluation cureval;
                cureval.bestmove = {-1, -1};
                cureval.value = -GAMEOVEREVAL;
                return cureval;
            } else {
                evaluation cureval;
                cureval.bestmove = {-1, -1};
                cureval.value = GAMEOVEREVAL;
                return cureval;
            }
        }
    }

    evaluation cureval;
    cureval.bestmove = moves[0].first;
    curpos = moves[0].second;

    bool wassize1 = false;
    while (moves.size() == 1){
        wassize1 = true;
        position newpos = moves[0].second;
        moves.clear();
        moves = get_all_moves(newpos);
        if (moves.size() == 0){
            // either it's a draw or one of the player won
            if (static_eval(newpos) == 0){
                // it's a draw
                cureval.value = 0;
                return cureval;
            } else {
                // one of the side won
                if (newpos.tomove & 1){
                    // it was white to move
                    cureval.value = -GAMEOVEREVAL;
                    return cureval;
                } else {
                    // it was black to move
                    cureval.value = GAMEOVEREVAL;
                    return cureval;
                }
            }
        }
    }

    node root;
    root.ismax = (curpos.tomove & 1);
    auto rootchildren = new std::vector<node>(moves.size());
    root.child = static_cast<void*>(rootchildren);
    for (int i=0; i<moves.size(); ++i){
        // rootchild would store the board instead of it's children
        auto newpos = new position(moves[i].second); // creating a copy of the position
        (*rootchildren)[i].child = static_cast<void*>(newpos);
        (*rootchildren)[i].ismax = (newpos->tomove & 1);
    }

    // repeat till either root.n is less than maximum root visit allowed or time limit exceded
    std::unordered_map<position, byte, position_hash> prev_pos;
    prev_pos[curpos] = 0;
    while (root.n < MAX_ROOT_VISIT && (convert_to_milli(timenow-start)) < MAX_EVAL_TIME){
        traverse(root, prev_pos); // root is at depth 0
    }

    // now find the best move
    if ((*rootchildren)[0].n == 0){
        // either MAX_EVAL_TIME or MAX_ROOT_VISIT is very less
        std::cout << "Position can't be evaluated in the given constraints (MAX_EVAL_TIME or MAX_ROOT_VISIT)" << std::endl;
        assert(0);
    }

    //cureval.value = root.total_value * 1.0 / root.n;
    cureval.value = (*rootchildren)[0].total_value * 1.0 / (*rootchildren)[0].n;
    float eval;
    if (!wassize1){
        if (curpos.tomove & 1){
            // it was white to move so we have to maximise
            // gives the root's child with the highest average value
            for (int i=1; i<moves.size(); ++i){
                if ((*rootchildren)[i].n == 0){
                    // either MAX_EVAL_TIME or MAX_ROOT_VISIT is very less
                    std::cout << "Position can't be evaluated in the given constraints (MAX_EVAL_TIME or MAX_ROOT_VISIT)" << std::endl;
                    assert(0);
                }
                eval = (*rootchildren)[i].total_value * 1.0 / (*rootchildren)[i].n;
                if (eval > cureval.value){
                    cureval.value = eval;
                    cureval.bestmove = moves[i].first;
                    curpos = moves[i].second;
                }
            }
        } else {
            // it is black to move so we have to minimise
            // gives the root's child with the least average value
            for (int i=1; i<moves.size(); ++i){
                if ((*rootchildren)[i].n == 0){
                    // either MAX_EVAL_TIME or MAX_ROOT_VISIT is very less
                    std::cout << "Position can't be evaluated in the given constraints (MAX_EVAL_TIME or MAX_ROOT_VISIT)" << std::endl;
                    assert(0);
                }
                eval = (*rootchildren)[i].total_value * 1.0 / (*rootchildren)[i].n;
                if (eval < cureval.value){
                    cureval.value = eval;
                    cureval.bestmove = moves[i].first;
                    curpos = moves[i].second;
                }
            }
        }

    } else {
        if (curpos.tomove & 1){
            // it was white to move so we have to maximise
            // gives the root's child with the highest average value
            for (int i=1; i<moves.size(); ++i){
                if ((*rootchildren)[i].n == 0){
                    // either MAX_EVAL_TIME or MAX_ROOT_VISIT is very less
                    std::cout << "Position can't be evaluated in the given constraints (MAX_EVAL_TIME or MAX_ROOT_VISIT)" << std::endl;
                    assert(0);
                }
                eval = (*rootchildren)[i].total_value * 1.0 / (*rootchildren)[i].n;
                if (eval > cureval.value){
                    cureval.value = eval;
                }
            }
        } else {
            // it is black to move so we have to minimise
            // gives the root's child with the least average value
            for (int i=1; i<moves.size(); ++i){
                if ((*rootchildren)[i].n == 0){
                    // either MAX_EVAL_TIME or MAX_ROOT_VISIT is very less
                    std::cout << "Position can't be evaluated in the given constraints (MAX_EVAL_TIME or MAX_ROOT_VISIT)" << std::endl;
                    assert(0);
                }
                eval = (*rootchildren)[i].total_value * 1.0 / (*rootchildren)[i].n;
                if (eval < cureval.value){
                    cureval.value = eval;
                }
            }
        }
    }

    // now free the memory
    free_mem(root);
    return cureval;
}

float round(float num, u_int32_t places){
    // if places is 3 and num is 1.23456 then it returns 1.235
    for (int i=0; i<places; ++i){
        num *= 10;
    }

    float temp = (int)num;
    if (num-temp >= 0.5){
        temp++;
    }

    for (int i=0; i<places; ++i){
        temp /= 10;
    }
    return temp;
}

// code for simulation here
int main(){
    std::ofstream outfile("output.txt");
    if (!outfile){
        std::cerr << "The 'output.txt' file cannot be opened" << std::endl;
        return 1;
    }
    std::cout.rdbuf(outfile.rdbuf());

    position game = get_input_pos();
    bool toplay = true;
    print_board(game);
    std::cout << std::endl;

    while (toplay){
        if (game.tomove & 1){
            // alphabeta plays as white
            auto start = timenow;
            auto output = iterative_deepening(game, 30, MAX_EVAL_TIME);
            std::cout << "It took " << convert_to_milli(timenow-start) << "ms" << std::endl;
            if (output.bestmove.finalsq == -1) toplay = false;
            std::cout << "Alphabeta Evaluation: " << round((double)output.value/1000, 3) << "(" << (int)output.gameoverdepth << ")" << std::endl;
        } else {
            auto start = timenow;
            auto output = MCTS(game);
            std::cout << "It took " << convert_to_milli(timenow-start) << "ms" << std::endl;
            if (output.bestmove.finalsq == -1) toplay = false;
            std::cout << "MCTS Evaluation: " << round((double)output.value/1000, 3) << std::endl;
        }
        print_board(game);
        std::cout << std::endl;
    }
    return 0;
}