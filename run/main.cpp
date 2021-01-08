#include <bits/stdc++.h>
using namespace std;

auto SEED = chrono::steady_clock::now().time_since_epoch().count();
mt19937 rng(SEED);
uniform_real_distribution<> rdouble(0.0, 1.0);

inline int rint(int a, int b) {return uniform_int_distribution<int>(a, b)(rng);}

const int N = 3;
const double INF = 1e9;

// -1 O 0 empty 1 X
struct Table {
    int state[N][N], winner;
    bool finish;

    Table () { // init
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                state[i][j] = 0;
            }
        }
        winner = 0;
        finish = false;
    }

    void update_winner_and_finish() {
        if (winner != 0) return;
        int row[N] = {}, col[N] = {}, diag[2] = {}, empty_cell = 0;
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                row[i] += state[i][j];
                col[j] += state[i][j];
                if (state[i][j] == 0) {
                    empty_cell++;
                }
            }
            diag[0] += state[i][i];
            diag[1] += state[i][N - i - 1];
        }
        for (int i = 0; i < N; ++i) {
            if (row[i] == N || row[i] == -N) {
                winner = row[i] / N;
            }
            if (col[i] == N || col[i] == -N) {
                winner = col[i] / N;
            }
        }
        for (int i = 0; i < 2; ++i) {
            if (diag[i] == N || diag[i] == -N) {
                winner = diag[i] / N;
            }
        }
        if (winner != 0 || empty_cell == 0) finish = true;
    }

    bool Finish() {
        return finish;
    }

    bool isEmpty(int x, int y) {
        return state[x][y] == 0;
    }

    int Winner() {
        return winner;
    }

    int get_state(int x, int y) {
        return state[x][y];
    }

    bool get_dead(int p) {
        for (int i = 0; i < N; ++i) {
            bool tmp[2]{};
            for (int j = 0; j < N; ++j) {
                if (state[i][j] == -p) tmp[0] = true;
                if (state[j][i] == -p) tmp[1] = true;
            }
            if (!tmp[0] || !tmp[1]) return false;
        }
        bool tmp = false;
        for (int i = 0; i < N; ++i) if (state[i][i] == -p) tmp = true;
        if (!tmp) return false;
        tmp = false;
        for (int i = 0; i < N; ++i) if (state[i][N - 1 - i] == -p) tmp = true;
        if (!tmp) return false;
        return true;
    }

    bool move(int x, int y, int p) { // p -> player
        if (Finish() || !isEmpty(x, y)) {
            return false;
        }
        state[x][y] = p;
        update_winner_and_finish();
        return true;
    }
};

struct Game {
    Table small[N][N], big;
    bool dead[N][N][2];
    int cur_player = 1, last_move_x = 0, last_move_y = 0, empty_big_cell = N * N;
    bool could_choose_all = true, finish = false;

    bool valid(int x, int y) {
        if (small[x / N][y / N].Finish() || !small[x / N][y / N].isEmpty(x % N, y % N)) {
            return false;
        }
        if (could_choose_all || (last_move_x == x / N && last_move_y == y / N)) {
            return true;
        }
        return false;
    }

    bool move(int x, int y) {
        if (!valid(x, y)) {
            return false;
        }
        small[x / N][y / N].move(x % N, y % N, cur_player);
        small[x / N][y / N].update_winner_and_finish();
        dead[x / N][y / N][cur_player == 1 ? 0 : 1] = small[x / N][y / N].get_dead(-cur_player);
        if (small[x / N][y / N].Winner() != 0) {
            big.move(x / N, y / N, small[x / N][y / N].Winner());
            big.update_winner_and_finish();
            empty_big_cell--;
        } else if (small[x / N][y / N].Finish()) {
            empty_big_cell--;
        }
        if (empty_big_cell == 0 || big.Winner() != 0) {
            finish = true;
        }
        last_move_x = x % N;
        last_move_y = y % N;
        cur_player = -cur_player;
        could_choose_all = false;
        if (small[last_move_x][last_move_y].Finish()) {
            could_choose_all = true;
        }
        return true;
    }

    bool Finish() {
        return finish;
    }

    int Winner() {
        return big.Winner();
    }

    int Player() {
        return cur_player;
    }

    bool get_dead_s(int x, int y) {
        return dead[x][y][cur_player == -1 ? 0 : 1];
    }

    bool get_dead_op(int x, int y) {
        return dead[x][y][cur_player == 1 ? 0 : 1];
    }

    int get_output_picture(int x, int y) {
        int shift = 0;
        if (small[x / N][y / N].Winner() == 1) {
            shift = 6;
        }
        if (small[x / N][y / N].Winner() == -1) {
            shift = 3;
        }
        if (small[x / N][y / N].get_state(x % N, y % N) == 1) {
            return 2 + shift;
        }
        if (small[x / N][y / N].get_state(x % N, y % N) == -1) {
            return 3 + shift;
        }
        if (!Finish() && valid(x, y)) {
            return 0;
        }
        return 1 + shift;
    }

    vector<pair<int, int>> valid_moves() {
        vector<pair<int, int>> tmp;
        for (int i = 0; i < N * N; ++i) for (int j = 0; j < N * N; ++j)
                if (valid(i, j)) tmp.emplace_back(i, j);
        return tmp;
    }
};

const int depth = 3, line_num[3][3]{3, 2, 3, 2, 4, 2, 3, 2, 3};

struct Player {
    double line_cnt[3], s_op_rate[2], small_big_rate[2], next_now_rate[2];
    int win = 0;
    Player() {
        do {
            line_cnt[0] = rdouble(rng);
            line_cnt[1] = rdouble(rng);
        } while (line_cnt[0] + line_cnt[1] >= 1);
        line_cnt[2] = 1.0 - line_cnt[0] - line_cnt[1];
        sort(line_cnt, line_cnt + 3);
        s_op_rate[0] = rdouble(rng), s_op_rate[1] = 1.0 - s_op_rate[0];
        small_big_rate[0] = rdouble(rng), small_big_rate[1] = 1.0 - small_big_rate[0];
        if (small_big_rate[0] > small_big_rate[1])
            swap(small_big_rate[0], small_big_rate[1]);
        next_now_rate[0] = rdouble(rng), next_now_rate[1] = 1.0 - next_now_rate[0];
        if (next_now_rate[0] > next_now_rate[1])
            swap(next_now_rate[0], next_now_rate[1]);
    }
    pair<int, int> best_move(Game game, int dep = depth) {
        vector<pair<int, int>> moves = game.valid_moves();
        pair<int, int> best = moves.front();
        double best_score = -INF;
        for (pair<int, int> i : moves) {
            double score = get_rating(game, i.first, i.second, dep);
            if (score > best_score)
                best_score = score, best = i;
        }
        return best;
    }
    double get_rating(Game game, int x, int y, int dep) {
        if (dep == 0) return 0;
        double line_sum = 0, rat_small, rat_big, rat_s, rat_op = 0;
        //small
        //row
        int tmp_s = 0, tmp_op = 0;
        for (int i = 0; i < N; ++i)
            if (game.small[x / N][y / N].get_state(x % N, i) == game.Player()) tmp_s++;
            else if (game.small[x / N][y / N].get_state(x % N, i) == -game.Player()) tmp_op++;
        if (tmp_s == 0 || tmp_op == 0)
            line_sum += line_cnt[tmp_s] * s_op_rate[0] + line_cnt[tmp_op] * s_op_rate[1];
        //col
        tmp_s = 0, tmp_op = 0;
        for (int i = 0; i < N; ++i)
            if (game.small[x / N][y / N].get_state(i, y % N) == game.Player()) tmp_s++;
            else if (game.small[x / N][y / N].get_state(i, y % N) == -game.Player()) tmp_op++;
        if (tmp_s == 0 || tmp_op == 0)
            line_sum += line_cnt[tmp_s] * s_op_rate[0] + line_cnt[tmp_op] * s_op_rate[1];
        //
        if (x % N == y % N) {
            tmp_s = 0, tmp_op = 0;
            for (int i = 0; i < N; ++i)
                if (game.small[x / N][y / N].get_state(i, i) == game.Player()) tmp_s++;
                else if (game.small[x / N][y / N].get_state(i, i) == -game.Player()) tmp_op++;
            if (tmp_s == 0 || tmp_op == 0)
                line_sum += line_cnt[tmp_s] * s_op_rate[0] + line_cnt[tmp_op] * s_op_rate[1];
        }
        if (x % N == 2 - y % N) {
            tmp_s = 0, tmp_op = 0;
            for (int i = 0; i < N; ++i)
                if (game.small[x / N][y / N].get_state(i, 2 - i) == game.Player()) tmp_s++;
                else if (game.small[x / N][y / N].get_state(i, 2 - i) == -game.Player()) tmp_op++;
            if (tmp_s == 0 || tmp_op == 0)
                line_sum += line_cnt[tmp_s] * s_op_rate[0] + line_cnt[tmp_op] * s_op_rate[1];
        }
        rat_small = line_sum / line_num[x % N][y % N];
        //big
        line_sum = 0;
        //row
        tmp_s = 0, tmp_op = 0;
        bool dead_s = false, dead_op = false;
        for (int i = 0; i < N; dead_s |= game.get_dead_s(x / N, i), dead_op |= game.get_dead_op(x / N, i), ++i)
            if (game.big.get_state(x / N, i) == game.Player()) tmp_s++;
            else if (game.big.get_state(x / N, i) == -game.Player()) tmp_op++;
        dead_s |= tmp_op > 0;
        dead_op |= tmp_s > 0;
        if (!dead_s)
            line_sum += line_cnt[tmp_s] * s_op_rate[0];
        if (!dead_op)
            line_sum += line_cnt[tmp_op] * s_op_rate[1];
        //col
        tmp_s = 0, tmp_op = 0;
        dead_s = false, dead_op = false;
        for (int i = 0; i < N; dead_s |= game.get_dead_s(i, y / N), dead_op |= game.get_dead_op(i, y / N), ++i)
            if (game.big.get_state(i, y / N) == game.Player()) tmp_s++;
            else if (game.big.get_state(i, y / N) == -game.Player()) tmp_op++;
        dead_s |= tmp_op > 0;
        dead_op |= tmp_s > 0;
        if (!dead_s)
            line_sum += line_cnt[tmp_s] * s_op_rate[0];
        if (!dead_op)
            line_sum += line_cnt[tmp_op] * s_op_rate[1];
        //
        if (x / N == y / N) {
            tmp_s = 0, tmp_op = 0;
            dead_s = false, dead_op = false;
            for (int i = 0; i < N; dead_s |= game.get_dead_s(i, i), dead_op |= game.get_dead_op(i, i), ++i)
                if (game.big.get_state(i, i) == game.Player()) tmp_s++;
                else if (game.big.get_state(i, i) == -game.Player()) tmp_op++;
            dead_s |= tmp_op > 0;
            dead_op |= tmp_s > 0;
            if (!dead_s)
                line_sum += line_cnt[tmp_s] * s_op_rate[0];
            if (!dead_op)
                line_sum += line_cnt[tmp_op] * s_op_rate[1];
        }
        if (x / N == 2 - y / N) {
            tmp_s = 0, tmp_op = 0;
            dead_s = false, dead_op = false;
            for (int i = 0; i < N; dead_s |= game.get_dead_s(i, i), dead_op |= game.get_dead_op(i, i), ++i)
                if (game.big.get_state(i, 2 - i) == game.Player()) tmp_s++;
                else if (game.big.get_state(i, 2 - i) == -game.Player()) tmp_op++;
            dead_s |= tmp_op > 0;
            dead_op |= tmp_s > 0;
            if (!dead_s)
                line_sum += line_cnt[tmp_s] * s_op_rate[0];
            if (!dead_op)
                line_sum += line_cnt[tmp_op] * s_op_rate[1];
        }
        rat_big = line_sum / line_num[x / N][y / N];
        rat_s = rat_small * small_big_rate[0] + rat_big * small_big_rate[1];
        //op
        game.move(x, y);
        // check whether the move could win
        if (game.Finish()) {
            if (game.Winner() != 0) return INF;
            return 0;
        }
        pair<int, int> op_best = best_move(game, dep - 1);
        rat_op = get_rating(game, op_best.first, op_best.second, dep - 1);
        return rat_s * next_now_rate[0] - rat_op * next_now_rate[1];
    }
    bool operator < (const Player &other) {
        return win > other.win;
    }
};

void wait(int time) {
    double t = clock();
    while (clock() - t < time);
}

void play(Player &p1, Player &p2) {
    Game game = Game();
    Player p[2]{p1, p2}; //p[0] 1 first
    int s = 0;
    do {
        pair<int, int> next_move = p[s].best_move(game);
        game.move(next_move.first, next_move.second);
        s ^= 1;
    } while (!game.Finish());
    if (game.Winner() == 1) p1.win += 3;
    else if (game.Winner() == -1) p2.win += 3;
    else p1.win++, p2.win++;
}

void print_player(Player &p) {
    for (int i = 0; i < 3; ++i) cout << p.line_cnt[i] << ' ';
    for (int i = 0; i < 2; ++i) cout << p.s_op_rate[i] << ' ';
    for (int i = 0; i < 2; ++i) cout << p.small_big_rate[i] << ' ';
    for (int i = 0; i < 2; ++i) cout << p.next_now_rate[i] << ' ';
    cout << "score " << p.win << endl;
}

Player generate_child(Player &p1, Player &p2) {
    Player par[2] = {p1, p2};
    Player ch;
    int choose = rint(0, 1);
    for (int i = 0; i < 3; i++) ch.line_cnt[i] = par[choose].line_cnt[i];
    choose = rint(0, 1);
    for (int i = 0; i < 2; i++) ch.s_op_rate[i] = par[choose].s_op_rate[i];
    choose = rint(0, 1);
    for (int i = 0; i < 2; i++) ch.small_big_rate[i] = par[choose].small_big_rate[i];
    choose = rint(0, 1);
    for (int i = 0; i < 2; i++) ch.next_now_rate[i] = par[choose].next_now_rate[i];
    // mutation
    if (rint(0, 999) < 20) {
        do {
            ch.line_cnt[0] = rdouble(rng);
            ch.line_cnt[1] = rdouble(rng);
        } while (ch.line_cnt[0] + ch.line_cnt[1] >= 1);
        ch.line_cnt[2] = 1.0 - ch.line_cnt[0] - ch.line_cnt[1];
        sort(ch.line_cnt, ch.line_cnt + 3);
    }
    if (rint(0, 999) < 20) {
        ch.s_op_rate[0] = rdouble(rng);
        ch.s_op_rate[1] = 1.0 - ch.s_op_rate[0];
    }
    if (rint(0, 999) < 20) {
        ch.small_big_rate[0] = rdouble(rng);
        ch.small_big_rate[1] = 1.0 - ch.small_big_rate[0];
        if (ch.small_big_rate[0] > ch.small_big_rate[1])
            swap(ch.small_big_rate[0], ch.small_big_rate[1]);
    }
    if (rint(0, 999) < 20) {
        ch.next_now_rate[0] = rdouble(rng), ch.next_now_rate[1] = 1.0 - ch.next_now_rate[0];
        if (ch.next_now_rate[0] > ch.next_now_rate[1])
            swap(ch.next_now_rate[0], ch.next_now_rate[1]);
    }
    return ch;
}

double tt;

void evolution (vector<vector <Player>> &first, vector <vector <Player>> &second, int group, int population, int t) {
    int choose_number = population / 10, retire_number = population / 10 * 3;
    vector <int> id(group);
    for (int i = 0; i < group; ++i) id[i] = i;
    for (int _t = 1; _t <= t; ++_t) {
        cout << "Start " << _t << endl;
		shuffle(id.begin(), id.end(), rng);
        for (int cur_id = 0; cur_id < group; ++cur_id) {
        	cout << "Matching " << cur_id << endl;
        	cout << "Now Time " << (clock() - tt) / 1000 << endl;
            int firstid = cur_id, secondid = id[cur_id];
            vector <Player> firstchild, secondchild;
            for (int child_number = 0; child_number < retire_number; ++child_number) {
                shuffle(first[firstid].begin(), first[firstid].end(), rng);
                shuffle(second[secondid].begin(), second[secondid].end(), rng);
                for (int i = 0; i < choose_number; ++i) {
                    for (int j = 0; j < choose_number; ++j) {
                        play(first[firstid][i], second[secondid][j]);
                    }
                }
                // generate first child
                sort(first[firstid].begin(), first[firstid].end());
                firstchild.push_back(generate_child(first[firstid][0], first[firstid][1]));
                // generate second child
                sort(second[secondid].begin(), second[secondid].end());
                secondchild.push_back(generate_child(second[secondid][0], second[secondid][1]));
                for (int i = 0; i < choose_number; ++i) {
                    first[firstid][i].win = 0;
                    second[secondid][i].win = 0;
                }
            }
            cout << "Finish child generation" << endl;
        	cout << "Now Time " << (clock() - tt) / 1000 << endl;
			for (int i = 0; i < population; ++i) {
                for (int j = 0; j < population; ++j) {
                    play(first[firstid][i], second[secondid][j]);
                }
            }
            // retire first bad gene
            {
                shuffle(first[firstid].begin(), first[firstid].end(), rng);
                sort(first[firstid].begin(), first[firstid].end());
                for (int i = 0; i < population; ++i) first[firstid][i].win = 0;
                for (int i = 0; i < retire_number; ++i) first[firstid][population - 1 - i] = firstchild[i];
            }
            // retire second bad gene
            {
                shuffle(second[secondid].begin(), second[secondid].end(), rng);
                sort(second[secondid].begin(), second[secondid].end());
                for (int i = 0; i < population; ++i) second[secondid][i].win = 0;
                for (int i = 0; i < retire_number; ++i) second[secondid][population - 1 - i] = secondchild[i];
            }
        }
    }
}

int main() {
    srand(time(NULL));
    int population = 120, number_of_change = 40, number_of_group = 5;
    tt = clock();
    cout << fixed << setprecision(6);
    freopen("output.txt", "w", stdout);
    vector <vector <Player>> first(number_of_group), second(number_of_group);
    for (int i = 0; i < number_of_group; ++i) {
        for (int j = 0; j < population; ++j) {
            first[i].push_back(Player());
            second[i].push_back(Player());
        }
    }
    evolution(first, second, number_of_group, population, number_of_change);
    cout << "Finished evolution" << endl;
    cout << "Now Time " << (clock() - tt) / 1000 << endl;
    for (int firstid = 0; firstid < number_of_group; ++firstid) {
        for (int secondid = 0; secondid < number_of_group; ++secondid) {
            for (int i = 0; i < population; ++i) {
                for (int j = 0; j < population; ++j) {
                    play(first[firstid][i], second[secondid][j]);
                }
            }
        }
    }
    cout << "Finished!" << endl;
    cout << "Time: " << (clock() - tt) / 1000 << endl;
    cout << "Below are the genes:" << endl;
    for (int firstid = 0; firstid < number_of_group; ++firstid) {
        cout << "First Group " << firstid + 1 << endl;
        sort(first[firstid].begin(), first[firstid].end());
        for (Player &p : first[firstid]) print_player(p);
    }
    for (int secondid = 0; secondid < number_of_group; ++secondid) {
        cout << "Second Group " << secondid + 1 << endl;
        sort(second[secondid].begin(), second[secondid].end());
        for (Player &p : second[secondid]) print_player(p);
    }
}
