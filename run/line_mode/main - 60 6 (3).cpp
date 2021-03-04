#include <bits/stdc++.h>
using namespace std;

const double INF = 1e9, eps = 1e-12;
auto SEED = chrono::steady_clock::now().time_since_epoch().count();
mt19937 rng(SEED);
uniform_real_distribution<> rdouble(0.0, 1.0);

inline int rint(int a, int b) {return uniform_int_distribution<int>(a, b)(rng);}

// -1 O 0 empty 1 X
struct Table {
    int state[3][3], winner;
    bool finish;

    Table () : winner(0), finish(false) { // init
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                state[i][j] = 0;
            }
        }
    }

    void update_winner_and_finish() {
        if (winner != 0) return;
        int row[3] = {}, col[3] = {}, diag[2] = {}, empty_cell = 0;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                row[i] += state[i][j];
                col[j] += state[i][j];
                if (state[i][j] == 0) {
                    empty_cell++;
                }
            }
            diag[0] += state[i][i];
            diag[1] += state[i][2 - i];
        }
        for (int i = 0; i < 3; ++i) {
            if (row[i] == 3 || row[i] == -3) {
                winner = row[i] / 3;
            }
            if (col[i] == 3 || col[i] == -3) {
                winner = col[i] / 3;
            }
        }
        for (int i = 0; i < 2; ++i) {
            if (diag[i] == 3 || diag[i] == -3) {
                winner = diag[i] / 3;
            }
        }
        if (winner != 0 || empty_cell == 0) {
            finish = true;
        }
    }

    bool isEmpty(int x, int y) {
        return state[x][y] == 0;
    }

    bool move(int x, int y, int p) { // p -> player
        if (finish || !isEmpty(x, y)) {
            return false;
        }
        state[x][y] = p;
        update_winner_and_finish();
        return true;
    }
};

struct Game {
    Table small[3][3], big;
    int cur_player = 1, last_move_x = 0, last_move_y = 0, empty_big_cell = 9;
    bool could_choose_all = true, finish = false;

    bool valid(int x, int y) {
        if (small[x / 3][y / 3].finish || !small[x / 3][y / 3].isEmpty(x % 3, y % 3)) {
            return false;
        }
        if (could_choose_all || (last_move_x == x / 3 && last_move_y == y / 3)) {
            return true;
        } else {
            return false;
        }
    }

    bool move(int x, int y) {
        if (!valid(x, y)) {
            return false;
        }
        small[x / 3][y / 3].move(x % 3, y % 3, cur_player);
        small[x / 3][y / 3].update_winner_and_finish();
        if (small[x / 3][y / 3].winner != 0) {
            big.move(x / 3, y / 3, small[x / 3][y / 3].winner);
            big.update_winner_and_finish();
            empty_big_cell--;
        } else if (small[x / 3][y / 3].finish) {
            empty_big_cell--;
        }
        if (empty_big_cell == 0 || big.winner != 0) {
            finish = true;
        }
        last_move_x = x % 3;
        last_move_y = y % 3;
        cur_player = -cur_player;
        if (small[last_move_x][last_move_y].finish) {
            could_choose_all = true;
        } else {
            could_choose_all = false;
        }
        return true;
    }

    int get_winner() {
        return big.winner;
    }

    int get_state(int x, int y, int i, int j) {
        return small[x][y].state[i][j];
    }

    int get_output_picture(int x, int y) {
        int shift;
        if (small[x / 3][y / 3].winner == 1) {
            shift = 6;
        } else if (small[x / 3][y / 3].winner == -1) {
            shift = 3;
        } else {
            shift = 0;
        }
        if (get_state(x / 3, y / 3, x % 3, y % 3) == 1) {
            return shift + 2;
        } else if (get_state(x / 3, y / 3, x % 3, y % 3) == -1) {
            return shift + 3;
        } else if (!finish && valid(x, y)) {
            return 0;
        } else {
            return 1 + shift;
        }
    }

    vector<pair<int, int>> valid_moves() {
        vector<pair<int, int>> tmp;
        for (int i = 0; i < 3 * 3; ++i) for (int j = 0; j < 3 * 3; ++j) {
            if (valid(i, j)) tmp.emplace_back(i, j);
        }
        return tmp;
    }
};

struct Player {
    double line_cnt[2], ready_rate[3], occupy, s_op_rate[2], next_now_rate[2], omega[8], alpha;
    int win = 0, draw = 0, lose = 0;
    Player() {
        double a[4];
        for (int i = 0; i < 4; ++i) a[i] = rdouble(rng);
        sort(a, a + 4);
        line_cnt[0] = a[0];
        line_cnt[1] = a[1];
        ready_rate[0] = 0;
        ready_rate[1] = a[2];
        ready_rate[2] = a[3];
        occupy = rdouble(rng) + 1;
        s_op_rate[0] = rdouble(rng);
        s_op_rate[1] = 1.0 - s_op_rate[0];
        next_now_rate[0] = rdouble(rng) / 2;
        next_now_rate[1] = 1.0 - next_now_rate[0];
        omega[0] = 1;
        omega[1] = rdouble(rng);
        for (int i = 2; i < 8; ++i) omega[i] = omega[i - 1] * omega[1];
        alpha = rdouble(rng) + 1;
    }
    pair<int, int> best_move(Game game, int dep) {
        vector<pair<int, int>> moves = game.valid_moves();
        shuffle(moves.begin(), moves.end(), rng);
        pair<int, int> best = moves.front();
        double best_score = -INF;
        for (pair<int, int> i : moves) {
            double score = get_rating(game, i.first, i.second, dep);
            if (score - best_score > eps)
                best_score = score, best = i;
        }
        return best;
    }
    double get_small_attack_rating(Game game, int x, int y, int p) { // p -> player
        if (game.small[x][y].finish) {
            if (game.small[x][y].winner == p) {
                return occupy;
            } else {
                return 0;
            }
        }
        bool ready[3][3];
        int ready_cnt = 0;
        for (int i = 0; i < 3; ++i) for (int j = 0; j < 3; ++j) {
            ready[i][j] = false;
            if (game.get_state(x, y, i, j) == 0) {
                {
                    // row
                    int cnt = 0;
                    for (int k = 0; k < 3; ++k) if (j != k) {
                        cnt += game.get_state(x, y, i, k);
                    }
                    if (cnt == p * 2) {
                        ready[i][j] = true;
                    }
                }
                {
                    // col
                    int cnt = 0;
                    for (int k = 0; k < 3; ++k) if (i != k) {
                        cnt += game.get_state(x, y, k, j);
                    }
                    if (cnt == p * 2) {
                        ready[i][j] = true;
                    }
                }
                if (i == j) {
                    // diag1
                    int cnt = 0;
                    for (int k = 0; k < 3; ++k) if (i != k) {
                        cnt += game.get_state(x, y, k, k);
                    }
                    if (cnt == p * 2) {
                        ready[i][j] = true;
                    }
                }
                if (i == 2 - j) {
                    // diag2
                    int cnt = 0;
                    for (int k = 0; k < 3; ++k) if (i != k) {
                        cnt += game.get_state(x, y, k, 2 - k);
                    }
                    if (cnt == p * 2) {
                        ready[i][j] = true;
                    }
                }
            }
            if (ready[i][j]) ready_cnt++;
        }
        double ready_score = ready_rate[min(ready_cnt, 2)], line_score = 0;
        vector <double> all_line;
        // row
        for (int i = 0; i < 3; ++i) {
            int cnt = 0;
            bool has_enemy = false, already = false;
            for (int j = 0; j < 3; ++j) {
                if (game.get_state(x, y, i, j) == p) {
                    cnt++;
                } else if (game.get_state(x, y, i, j) == -p) {
                    has_enemy = true;
                }
                if (ready[i][j]) already = true;
            }
            if (!already) {
                if (has_enemy) all_line.push_back(0);
                else all_line.push_back(line_cnt[cnt]);
            }
        }
        // col
        for (int j = 0; j < 3; ++j) {
            int cnt = 0;
            bool has_enemy = false, already = false;
            for (int i = 0; i < 3; ++i) {
                if (game.get_state(x, y, i, j) == p) {
                    cnt++;
                } else if (game.get_state(x, y, i, j) == -p) {
                    has_enemy = true;
                }
                if (ready[i][j]) already = true;
            }
            if (!already) {
                if (has_enemy) all_line.push_back(0);
                else all_line.push_back(line_cnt[cnt]);
            }
        }
        // diag1
        {
            int cnt = 0;
            bool has_enemy = false, already = false;
            for (int i = 0; i < 3; ++i) {
                if (game.get_state(x, y, i, i) == p) {
                    cnt++;
                } else if (game.get_state(x, y, i, i) == -p) {
                    has_enemy = true;
                }
                if (ready[i][i]) already = true;
            }
            if (!already) {
                if (has_enemy) all_line.push_back(0);
                else all_line.push_back(line_cnt[cnt]);
            }
        }
        // diag2
        {
            int cnt = 0;
            bool has_enemy = false, already = false;
            for (int i = 0; i < 3; ++i) {
                if (game.get_state(x, y, i, 2 - i) == p) {
                    cnt++;
                } else if (game.get_state(x, y, i, 2 - i) == -p) {
                    has_enemy = true;
                }
                if (ready[i][2 - i]) already = true;
            }
            if (!already) {
                if (has_enemy) all_line.push_back(0);
                else all_line.push_back(line_cnt[cnt]);
            }
        }
        if (!all_line.empty()) {
            sort(all_line.rbegin(), all_line.rend());
            double base = 0;
            for (int i = 0; i < all_line.size(); ++i) {
                line_score += all_line[i] * omega[i];
                base += omega[i];
            }
            line_score /= base;
        }
        return (ready_score + line_score) / 2;
    }
    double get_small_rating(Game game, int x, int y, int p) {
        return get_small_attack_rating(game, x, y, p) * s_op_rate[0] - get_small_attack_rating(game, x, y, -p) * s_op_rate[1];
    }
    double get_big_rating(Game game, int p) {
        if (game.finish) {
            if (game.get_winner() == p) {
                return INF;
            } else if (game.get_winner() == -p) {
                return -INF;
            } else {
                return 0;
            }
        }
        double small_rating[3][3], ready_score = 0, line_score = 0;
        vector <double> all_line;
        for (int i = 0; i < 3; ++i) for (int j = 0; j < 3; ++j) {
            small_rating[i][j] = get_small_rating(game, i, j, p);
        }
        bool ready[3][3];
        for (int i = 0; i < 3; ++i) for (int j = 0; j < 3; ++j) {
            ready[i][j] = false;
            if (!game.small[i][j].finish) {
                {
                    // row
                    int cnt = 0;
                    for (int k = 0; k < 3; ++k) if (j != k) {
                        if (game.small[i][k].winner == p) cnt++;
                    }
                    if (cnt == 2) {
                        ready[i][j] = true;
                    }
                }
                {
                    // col
                    int cnt = 0;
                    for (int k = 0; k < 3; ++k) if (i != k) {
                        if (game.small[k][j].winner == p) cnt++;
                    }
                    if (cnt == 2) {
                        ready[i][j] = true;
                    }
                }
                if (i == j) {
                    // diag1
                    int cnt = 0;
                    for (int k = 0; k < 3; ++k) if (i != k) {
                        if (game.small[k][k].winner == p) cnt++;
                    }
                    if (cnt == 2) {
                        ready[i][j] = true;
                    }
                }
                if (i == 2 - j) {
                    // diag2
                    int cnt = 0;
                    for (int k = 0; k < 3; ++k) if (i != k) {
                        if (game.small[k][2 - k].winner == p) cnt++;
                    }
                    if (cnt == 2) {
                        ready[i][j] = true;
                    }
                }
            }
            if (ready[i][j]) ready_score += pow(alpha, small_rating[i][j]);
        }
        // row
        for (int i = 0; i < 3; ++i) {
            double sum = 0;
            bool has_enemy = false, already = false;
            for (int j = 0; j < 3; ++j) {
                sum += small_rating[i][j];
                if (ready[i][j]) already = true;
                if (game.small[i][j].winner != p && game.small[i][j].finish) has_enemy = true;
            }
            if (!already) {
                if (has_enemy) all_line.push_back(0);
                else all_line.push_back(pow(alpha, sum / 3));
            }
        }
        // col
        for (int j = 0; j < 3; ++j) {
            double sum = 0;
            bool has_enemy = false, already = false;
            for (int i = 0; i < 3; ++i) {
                sum += small_rating[i][j];
                if (ready[i][j]) already = true;
                if (game.small[i][j].winner != p && game.small[i][j].finish) has_enemy = true;
            }
            if (!already) {
                if (has_enemy) all_line.push_back(0);
                else all_line.push_back(pow(alpha, sum / 3));
            }
        }
        // diag1
        {
            double sum = 0;
            bool has_enemy = false, already = false;
            for (int i = 0; i < 3; ++i) {
                sum += small_rating[i][i];
                if (ready[i][i]) already = true;
                if (game.small[i][i].winner != p && game.small[i][i].finish) has_enemy = true;
            }
            if (!already) {
                if (has_enemy) all_line.push_back(0);
                else all_line.push_back(pow(alpha, sum / 3));
            }
        }
        // diag2
        {
            double sum = 0;
            bool has_enemy = false, already = false;
            for (int i = 0; i < 3; ++i) {
                sum += small_rating[i][2 - i];
                if (ready[i][2 - i]) already = true;
                if (game.small[i][2 - i].winner != p && game.small[i][2 - i].finish) has_enemy = true;
            }
            if (!already) {
                if (has_enemy) all_line.push_back(0);
                else all_line.push_back(pow(alpha, sum / 3));
            }
        }
        if (!all_line.empty()) {
            sort(all_line.rbegin(), all_line.rend());
            double base = 0;
            for (int i = 0; i < all_line.size(); ++i) {
                line_score += all_line[i] * omega[i];
                base += omega[i];
            }
            line_score /= base;
        }
        return (ready_score + line_score) / 2;
    }
    double get_rating(Game game, int x, int y, int dep) {
        if (dep == 0) return 0;
        int p = game.cur_player;
        double rat_s = -get_big_rating(game, p), rat_op;
        game.move(x, y);
        // check whether the move could win
        if (game.finish) {
            if (game.get_winner() != 0) {
                return INF;
            } else {
                return 0;
            }
        }
        rat_s += get_big_rating(game, p);
        pair<int, int> op_best = best_move(game, dep - 1);
        rat_op = get_rating(game, op_best.first, op_best.second, dep - 1);
        return rat_s * next_now_rate[1] - rat_op * next_now_rate[0];
    }
    void reset() {
        win = draw = lose = 0;
    }
    double input_double() {
        string s;
        cin >> s;
        double cur = 0, base = 1;
        for (char &c : s) if (c != '.') {
            cur += (c - '0') * base;
            base /= 10;
        }
        return cur;
    }
    void get_value_from_stdin() {
        line_cnt[0] = input_double();
        line_cnt[1] = input_double();
        ready_rate[1] = input_double();
        ready_rate[2] = input_double();
        ready_rate[0] = 0;
        occupy = input_double();
        s_op_rate[0] = input_double();
        next_now_rate[0] = input_double();
        omega[1] = input_double();
        alpha = input_double();
        s_op_rate[1] = 1.0 - s_op_rate[0];
        next_now_rate[1] = 1.0 - next_now_rate[0];
        omega[0] = 1;
        for (int i = 2; i < 8; ++i) omega[i] = omega[i - 1] * omega[1];
    }
    friend ostream &operator<<(ostream &o, Player &p) {
        return o << p.line_cnt[0] << ' ' << p.line_cnt[1] << ' ' << p.ready_rate[1] << ' ' << p.ready_rate[2] << ' '
        << p.occupy << ' ' << p.s_op_rate[0] << ' ' << p.next_now_rate[0] << ' ' << p.omega[1] << ' ' << p.alpha
        << " Score " << p.win * 3 + p.draw << ' ' << p.win << "W " << p.draw << "D " << p.lose << "L";
    }
    bool operator < (const Player &other) const {
        return win * 3 + draw > other.win * 3 + other.draw;
    }
};

void play(Player &p1, Player &p2) {
    Game game = Game();
    Player p[2]{p1, p2}; //p[0] 1 first
    int s = 0, round = 0;
    do {
        pair<int, int> next_move = p[s].best_move(game, (round >= 20 ? 3 : 2));
        game.move(next_move.first, next_move.second);
        s ^= 1;
        round++;
    } while (!game.finish);
    if (game.get_winner() == 1) p1.win++, p2.lose++;
    else if (game.get_winner() == -1) p2.win++, p1.lose++;
    else p1.draw++, p2.draw++;
}

Player generate_child(Player &p1, Player &p2) {
    Player par[2] = {p1, p2};
    Player ch;
    {
        int choose = rint(0, 1);
        ch.line_cnt[0] = par[choose].line_cnt[0];
        ch.line_cnt[1] = par[choose].line_cnt[1];
        ch.ready_rate[0] = par[choose].ready_rate[0];
        ch.ready_rate[1] = par[choose].ready_rate[1];
        ch.ready_rate[2] = par[choose].ready_rate[2];
    }
    {
        int choose = rint(0, 1);
        ch.occupy = par[choose].occupy;
    }
    {
        int choose = rint(0, 1);
        ch.s_op_rate[0] = par[choose].s_op_rate[0];
        ch.s_op_rate[1] = par[choose].s_op_rate[1];
    }
    {
        int choose = rint(0, 1);
        ch.next_now_rate[0] = par[choose].next_now_rate[0];
        ch.next_now_rate[1] = par[choose].next_now_rate[1];
    }
    {
        int choose = rint(0, 1);
        for (int i = 0; i < 8; ++i) {
            ch.omega[i] = par[choose].omega[i];
        }
    }
    {
        int choose = rint(0, 1);
        ch.alpha = par[choose].alpha;
    }
    // mutation
    if (rint(0, 999) <= 20) {
        double a[4];
        for (int i = 0; i < 4; ++i) a[i] = rdouble(rng);
        sort(a, a + 4);
        ch.line_cnt[0] = a[0];
        ch.line_cnt[1] = a[1];
        ch.ready_rate[0] = 0;
        ch.ready_rate[1] = a[2];
        ch.ready_rate[2] = a[3];
    }
    if (rint(0, 999) <= 20) {
        ch.occupy = rdouble(rng) + 1;
    }
    if (rint(0, 999) <= 20) {
        ch.s_op_rate[0] = rdouble(rng);
        ch.s_op_rate[1] = 1.0 - ch.s_op_rate[0];
    }
    if (rint(0, 999) <= 20) {
        ch.next_now_rate[0] = rdouble(rng) / 2;
        ch.next_now_rate[1] = 1.0 - ch.next_now_rate[0];
    }
    if (rint(0, 999) <= 20) {
        ch.omega[0] = 1;
        ch.omega[1] = rdouble(rng);
        for (int i = 2; i < 8; ++i) ch.omega[i] = ch.omega[i - 1] * ch.omega[1];
    }
    if (rint(0, 999) <= 20) {
        ch.alpha = rdouble(rng) + 1;
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
                    first[firstid][i].reset();
                    second[secondid][i].reset();
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
                for (int i = 0; i < population; ++i) first[firstid][i].reset();
                for (int i = 0; i < retire_number; ++i) first[firstid][population - 1 - i] = firstchild[i];
            }
            // retire second bad gene
            {
                shuffle(second[secondid].begin(), second[secondid].end(), rng);
                sort(second[secondid].begin(), second[secondid].end());
                for (int i = 0; i < population; ++i) second[secondid][i].reset();
                for (int i = 0; i < retire_number; ++i) second[secondid][population - 1 - i] = secondchild[i];
            }
        }
    }
}

int main() {
    int population = 60, number_of_change = 40, number_of_group = 6;
    tt = clock();
    cout << fixed << setprecision(10);
    freopen("output - 80 5 (2).txt", "w", stdout);
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
        for (Player &p : first[firstid]) cout << p << endl;
    }
    for (int secondid = 0; secondid < number_of_group; ++secondid) {
        cout << "Second Group " << secondid + 1 << endl;
        sort(second[secondid].begin(), second[secondid].end());
        for (Player &p : second[secondid]) cout << p << endl;
    }
}
