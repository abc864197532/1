#include <bits/stdc++.h>
using namespace std;

const double INF = 1e9, eps = 1e-12;
auto SEED = chrono::steady_clock::now().time_since_epoch().count();
mt19937 rng(SEED);
uniform_real_distribution<> rdouble(0.0, 1.0);

inline int rint(int a, int b) {return uniform_int_distribution<int>(a, b)(rng);}

struct Table {
    int state[3][3], blue_score, red_score, empty_cell;
    bool finish;

    Table () : blue_score(0), red_score(0), empty_cell(9), finish(false) { // init
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                state[i][j] = 0;
            }
        }
    }

    void update_score() {
        int row[3] = {}, col[3] = {}, diag[2] = {};
        red_score = blue_score = 0;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                row[i] += state[i][j];
                col[j] += state[i][j];
            }
            diag[0] += state[i][i];
            diag[1] += state[i][2 - i];
        }
        for (int i = 0; i < 3; ++i) {
            if (row[i] == 3) {
                red_score++;
            }
            if (row[i] == -3) {
                blue_score++;
            }
            if (col[i] == 3) {
                red_score++;
            }
            if (col[i] == -3) {
                blue_score++;
            }
        }
        for (int i = 0; i < 2; ++i) {
            if (diag[i] == 3) {
                red_score++;
            }
            if (diag[i] == -3) {
                blue_score++;
            }
        }
        finish = empty_cell == 0;
    }

    bool isEmpty(int x, int y) {
        return state[x][y] == 0;
    }

    bool move(int x, int y, int p) { // p -> player
        if (!isEmpty(x, y)) {
            return false;
        }
        state[x][y] = p;
        empty_cell--;
        update_score();
        return true;
    }
};

struct Game {
    Table small[3][3];
    int cur_player = 1, last_move_x = 0, last_move_y = 0, empty_cell = 81;
    int red_score = 0, blue_score = 0;
    bool could_choose_all = true, finish = false;

    bool valid(int x, int y) {
        if (!small[x / 3][y / 3].isEmpty(x % 3, y % 3)) {
            return false;
        }
        if (could_choose_all || (last_move_x == x / 3 && last_move_y == y / 3)) {
            return true;
        }
        return false;
    }

    void update_score() {
        red_score = 0;
        blue_score = 0;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                red_score += small[i][j].red_score;
                blue_score += small[i][j].blue_score;
            }
        }
        finish = empty_cell == 0;
    }

    bool move(int x, int y) {
        if (!valid(x, y)) {
            return false;
        }
        small[x / 3][y / 3].move(x % 3,y % 3, cur_player);
        small[x / 3][y / 3].update_score();
        empty_cell--;
        update_score();
        last_move_x = x % 3;
        last_move_y = y % 3;
        cur_player = -cur_player;
        could_choose_all = false;
        if (small[last_move_x][last_move_y].finish) {
            could_choose_all = true;
        }
        return true;
    }

    int get_winner() {
        if (red_score > blue_score) return 1;
        if (red_score < blue_score) return -1;
        return 0;
    }

    int get_state(int x, int y, int i, int j) {
        return small[x][y].state[i][j];
    }

    int get_output_picture(int x, int y) {
        int p = get_state(x / 3, y / 3, x % 3, y % 3);
        if (p == 1) {
            return 2;
        }
        if (p == -1) {
            return 3;
        }
        if (!finish && valid(x, y)) {
            return 0;
        }
        return 1;
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
    double line_cnt[2], s_op_rate[2], next_now_rate[2];
    int win = 0, draw = 0, lose = 0;
    Player() {
        line_cnt[0] = rdouble(rng);
        line_cnt[1] = rdouble(rng);
        if (line_cnt[0] > line_cnt[1]) swap(line_cnt[0], line_cnt[1]);
        s_op_rate[0] = rdouble(rng);
        s_op_rate[1] = 1.0 - s_op_rate[0];
        next_now_rate[0] = rdouble(rng) / 2;
        next_now_rate[1] = 1.0 - next_now_rate[0];
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
    double small_rating(Game game, int _i, int _j, int p){
        double sum = 0;
        for (int i = 0; i < 3; i++){
            int cnt = 0;
            for (int j = 0; j < 3; j++){
                cnt += game.get_state(_i, _j, i, j) == p;
                if (game.get_state(_i, _j, i, j) == -p) cnt = -3;
            }
            if (cnt >= 0) sum += cnt == 3 ? 1 : line_cnt[cnt];
        }
        for (int i = 0; i < 3; i++){
            int cnt = 0;
            for (int j = 0; j < 3; j++){
                cnt += game.get_state(_i, _j, j, i) == p;
                if (game.get_state(_i, _j, i, j) == -p) cnt = -3;
            }
            if (cnt >= 0) sum += cnt == 3 ? 1 : line_cnt[cnt];
        }
        int cnt = 0;
        for (int i = 0; i < 3; i++){
            if (game.get_state(_i, _j, i, i) == -p) cnt = -3;
            else cnt += game.get_state(_i, _j, i, i) == p;
        }
        if (cnt >= 0) sum += cnt == 3 ? 1 : line_cnt[cnt];
        cnt = 0;
        for (int i = 0; i < 3; i++){
            if (game.get_state(_i, _j, i, 2 - i) == -p) cnt = -3;
            else cnt += game.get_state(_i, _j, i, 2 - i) == p;
        }
        if (cnt >= 0) sum += cnt == 3 ? 1 : line_cnt[cnt];
        return sum;
    }
    double delta_rating(Game game, int x, int y){
        int p  = game.cur_player;
        double ans = -small_rating(game, x / 3, y / 3, p) * s_op_rate[0] + small_rating(game, x / 3, y / 3, -p) * s_op_rate[1];
        game.move(x, y);
        ans += small_rating(game, x / 3, y / 3, p) * s_op_rate[0] - small_rating(game, x / 3, y / 3, -p) * s_op_rate[1];
        return ans;
    }
    double get_rating(Game game, int x, int y, int dep) {
        if (dep == 0) return 0;
        int p = game.cur_player;
        double rat_s = delta_rating(game, x, y), rat_op;
        game.move(x, y);
        if (game.finish) {
        	if (game.get_winner() == p) {
        		return INF;
			} else {
				return 0;
			}
		}
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
        s_op_rate[0] = input_double();
        next_now_rate[0] = input_double();
        s_op_rate[1] = 1.0 - s_op_rate[0];
        next_now_rate[1] = 1.0 - next_now_rate[0];
    }
    friend ostream &operator<<(ostream &o, Player &p) {
        return o << p.line_cnt[0] << ' ' << p.line_cnt[1] << ' ' << p.s_op_rate[0] << ' ' << p.next_now_rate[0]
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
        pair<int, int> next_move = p[s].best_move(game, 3);
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
        ch.line_cnt[2] = par[choose].line_cnt[2];
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
    // mutation
    if (rint(0, 999) <= 20) {
		ch.line_cnt[0] = rdouble(rng);
        ch.line_cnt[1] = rdouble(rng);
        ch.line_cnt[2] = rdouble(rng);
        sort(ch.line_cnt, ch.line_cnt + 3);
    }
    if (rint(0, 999) <= 20) {
        ch.s_op_rate[0] = rdouble(rng);
        ch.s_op_rate[1] = 1.0 - ch.s_op_rate[0];
    }
    if (rint(0, 999) <= 20) {
        ch.next_now_rate[0] = rdouble(rng) / 2;
        ch.next_now_rate[1] = 1.0 - ch.next_now_rate[0];
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
    int population = 150, number_of_change = 30, number_of_group = 6;
    tt = clock();
    cout << fixed << setprecision(10);
    freopen("output - 150 6 (4).txt", "w", stdout);
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
