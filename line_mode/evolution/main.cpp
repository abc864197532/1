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
        for (auto i : moves) {
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
        for (int i = 0; i < 3; ++i)
            if (game.small[x / N][y / N].get_state(i, y % N) == game.Player()) tmp_s++;
            else if (game.small[x / N][y / N].get_state(i, y % N) == -game.Player()) tmp_op++;
        if (tmp_s == 0 || tmp_op == 0)
            line_sum += line_cnt[tmp_s] * s_op_rate[0] + line_cnt[tmp_op] * s_op_rate[1];
        //
        if (x % N == y % N) {
            tmp_s = 0, tmp_op = 0;
            for (int i = 0; i < 3; ++i)
                if (game.small[x / N][y / N].get_state(i, i) == game.Player()) tmp_s++;
                else if (game.small[x / N][y / N].get_state(i, i) == -game.Player()) tmp_op++;
            if (tmp_s == 0 || tmp_op == 0)
                line_sum += line_cnt[tmp_s] * s_op_rate[0] + line_cnt[tmp_op] * s_op_rate[1];
        }
        if (x % N == 2 - y % N) {
            tmp_s = 0, tmp_op = 0;
            for (int i = 0; i < 3; ++i)
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
        for (int i = 0; i < 3; ++i)
            if (game.big.get_state(x / N, i) == game.Player()) tmp_s++;
            else if (game.big.get_state(x / N, i) == -game.Player()) tmp_op++;
        if (tmp_s == 0 || tmp_op == 0)
            line_sum += line_cnt[tmp_s] * s_op_rate[0] + line_cnt[tmp_op] * s_op_rate[1];
        //col
        tmp_s = 0, tmp_op = 0;
        for (int i = 0; i < 3; ++i)
            if (game.big.get_state(i, y / N) == game.Player()) tmp_s++;
            else if (game.big.get_state(i, y / N) == -game.Player()) tmp_op++;
        if (tmp_s == 0 || tmp_op == 0)
            line_sum += line_cnt[tmp_s] * s_op_rate[0] + line_cnt[tmp_op] * s_op_rate[1];
        //
        if (x / N == y / N) {
            tmp_s = 0, tmp_op = 0;
            for (int i = 0; i < 3; ++i)
                if (game.big.get_state(i, i) == game.Player()) tmp_s++;
                else if (game.big.get_state(i, i) == -game.Player()) tmp_op++;
            if (tmp_s == 0 || tmp_op == 0)
                line_sum += line_cnt[tmp_s] * s_op_rate[0] + line_cnt[tmp_op] * s_op_rate[1];
        }
        if (x / N == 2 - y / N) {
            tmp_s = 0, tmp_op = 0;
            for (int i = 0; i < 3; ++i)
                if (game.big.get_state(i, 2 - i) == game.Player()) tmp_s++;
                else if (game.big.get_state(i, 2 - i) == -game.Player()) tmp_op++;
            if (tmp_s == 0 || tmp_op == 0)
                line_sum += line_cnt[tmp_s] * s_op_rate[0] + line_cnt[tmp_op] * s_op_rate[1];
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

void mutation(double &v) {
    double a = rdouble(rng);
    v = (v * 3 + a * 2) / 5;
}

void play(Player &p1, Player &p2) {
    Game game = Game();
    Player p[2]{p1, p2};//p[0] 1 first
    int s = 0;
    do {
        pair<int, int> next_move = p[s].best_move(game);
        game.move(next_move.first, next_move.second);
        s ^= 1;
    } while (!game.Finish());
    if (game.Winner() == 1) ++p1.win;
    else if (game.Winner() == -1) ++p2.win;
}

void evolution (vector<Player> &init, int population, int t) {
    int choose_number = population / 10, retire_number = population / 10 * 3;
    while (t--) {
        vector <Player> child;
        for (int choose; child.size() < retire_number;) {
            shuffle(init.begin(),init.end(), rng);
            for (int i = 0; i < choose_number; ++i) {
                for (int j = 0; j < choose_number; ++j) {
                    if (i == j) continue;
                    play(init[i], init[j]);
                }
            }
            sort(init.begin(), init.end());
            Player par[2] = {init[0], init[1]};
            Player ch;
            choose = rint(0, 1);
            for (int i = 0; i < 3; i++) ch.line_cnt[i] = par[choose].line_cnt[i];
            choose = rint(0, 1);
            for (int i = 0; i < 2; i++) ch.s_op_rate[i] = par[choose].s_op_rate[i];
            choose = rint(0, 1);
            for (int i = 0; i < 2; i++) ch.small_big_rate[i] = par[choose].small_big_rate[i];
            choose = rint(0, 1);
            for (int i = 0; i < 2; i++) ch.next_now_rate[i] = par[choose].next_now_rate[i];
            // mutation
            if (rint(0, 999) < 2) {
                for (int i = 0; i < 2; ++i) {
                    do {
                        mutation(ch.line_cnt[0]);
                        mutation(ch.line_cnt[1]);
                    } while (ch.line_cnt[0] + ch.line_cnt[1] >= 1);
                    ch.line_cnt[2] = 1.0 - ch.line_cnt[0] - ch.line_cnt[1];
                }
            }
            if (rint(0, 999) < 2) {
                mutation(ch.s_op_rate[0]);
                ch.s_op_rate[1] = 1.0 - ch.s_op_rate[0];
            }
            if (rint(0, 999) < 2) {
                mutation(ch.small_big_rate[0]);
                ch.small_big_rate[1] = 1.0 - ch.small_big_rate[0];
                if (ch.small_big_rate[0] > ch.small_big_rate[1])
                    swap(ch.small_big_rate[0], ch.small_big_rate[1]);
            }
            if (rint(0, 999) < 2) {
                mutation(ch.next_now_rate[0]);
                ch.next_now_rate[1] = 1.0 - ch.next_now_rate[0];
                if (ch.next_now_rate[0] > ch.next_now_rate[1])
                    swap(ch.next_now_rate[0], ch.next_now_rate[1]);
            }

            child.push_back(ch);
            for (int i = 0; i < choose_number; ++i) init[i].win = 0;
        }
        for (int i = 0; i < population; ++i) {
            for (int j = 0; j < population; ++j) {
                if (i == j) continue;
                play(init[i], init[j]);
            }
        }
        sort(init.begin(), init.end());
        for (int i = 0; i < population; ++i) init[i].win = 0;
        for (int i = 0; i < retire_number; ++i) init[population - 1 - i] = child[i];
    }
}

int main() {
    srand(time(NULL));
    int population = 50, number_of_change = 20;
    double tt = clock();
    vector <Player> P(population);
    evolution(P, population, number_of_change);
    for (int i = 0; i < population; ++i) {
        for (int j = 0; j < population; ++j) {
            if (i == j) continue;
            play(P[i], P[j]);
        }
    }
    sort(P.begin(), P.end());
    freopen("output.txt", "w", stdout);
    cout << fixed << setprecision(6);
    cout << "Finished!\n";
    cout << "Time: " << clock() - tt << endl;
    cout << "Below are the vectors\n";
    for (Player &p : P) {
        for (int i = 0; i < 3; ++i) cout << p.line_cnt[i] << ' ';
        for (int i = 0; i < 2; ++i) cout << p.s_op_rate[i] << ' ';
        for (int i = 0; i < 2; ++i) cout << p.small_big_rate[i] << ' ';
        for (int i = 0; i < 2; ++i) cout << p.next_now_rate[i] << ' ';
        cout << endl;
    }
}