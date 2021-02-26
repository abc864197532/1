#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
using namespace sf;
using namespace std;

const double INF = 1e9, eps = 1e-12;
auto SEED = chrono::steady_clock::now().time_since_epoch().count();
mt19937 rng(SEED);
uniform_real_distribution<> rdouble(0.0, 1.0);

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

bool waiting = false;

void wait(int time) {
    waiting = true;
    double t = clock();
    while (clock() - t < time);
    waiting = false;
}

int main() {
    srand(time(NULL));
    RenderWindow window(VideoMode(600, 760), "The Game!");

    std::string image_root = "C:/Users/smw93/Desktop/big_tic_tac_toe/image";
    Texture pi[15];
    pi[0].loadFromFile(image_root + "/could_choose.png");
    pi[1].loadFromFile(image_root + "/empty.png");
    pi[2].loadFromFile(image_root + "/red.png");
    pi[3].loadFromFile(image_root + "/blue.png");
    pi[4].loadFromFile(image_root + "/empty_blue.png");
    pi[5].loadFromFile(image_root + "/red_blue.png");
    pi[6].loadFromFile(image_root + "/blue_blue.png");
    pi[7].loadFromFile(image_root + "/empty_red.png");
    pi[8].loadFromFile(image_root + "/red_red.png");
    pi[9].loadFromFile(image_root + "/blue_red.png");
    pi[10].loadFromFile(image_root + "/blue_word.png");
    pi[11].loadFromFile(image_root + "/blue_win.png");
    pi[12].loadFromFile(image_root + "/red_word.png");
    pi[13].loadFromFile(image_root + "/red_win.png");
    pi[14].loadFromFile(image_root + "/block.png");

    Sprite picture[15];
    for (int i = 0; i < 15; ++i) picture[i] = Sprite(pi[i]);

    int w = 65, state = 0, round = 0;
    Game game;
    Player com;
    freopen("com_value.txt", "r", stdin);
    com.get_value_from_stdin();
    cout << fixed << setprecision(10);
    cout << com << endl;

    function<void()> draw = [&]() {
        window.clear(Color::White);
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                picture[14].setPosition(i * 3 * w + 5, j * 3 * w + 167);
                window.draw(picture[14]);
            }
        }
        if (game.finish) {
            if (game.get_winner() == 1) {
                if (state == 0) {
                    picture[12].setPosition(2 * w, 0);
                    window.draw(picture[12]);
                } else {
                    picture[13].setPosition(2 * w, 0);
                    window.draw(picture[13]);
                }
            } else if (game.get_winner() == -1) {
                if (state == 0) {
                    picture[10].setPosition(2 * w, 0);
                    window.draw(picture[10]);
                } else {
                    picture[11].setPosition(2 * w, 0);
                    window.draw(picture[11]);
                }
            }
        } else {
            if (game.cur_player == 1) {
                picture[12].setPosition(2 * w, 0);
                window.draw(picture[12]);
            } else {
                picture[10].setPosition(2 * w, 0);
                window.draw(picture[10]);
            }
        }
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                int id = game.get_output_picture(i, j);
                picture[id].setPosition(i * w, j * w + 160);
                window.draw(picture[id]);
            }
        }
        window.display();
    };

    bool com_first = true;  // first or second
    if (com_first) {
        pair<int, int> next_move = com.best_move(game, 3);
        game.move(next_move.first, next_move.second);
        cout << "Red: " << next_move.first << ' ' << next_move.second << endl;
        round++;
    }

    while (window.isOpen()) {
        Vector2i pos = Mouse::getPosition(window);

        int x = pos.x / w;
        int y = (pos.y - 160) / w;

        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                window.close();
            }

            if (!waiting && !game.finish && e.type == Event::MouseButtonPressed) {
                if (e.key.code == Mouse::Left) {
                    if (x >= 0 && x < 9 && y >= 0 && y < 9 && game.move(x, y)) {
                        cout << (!com_first ? "Red: " : "Blue: ") << x << ' ' << y << endl;
                        round++;
                        draw();
                        if (!game.finish)  {
                            wait(500);
                            pair<int, int> next_move = com.best_move(game, 3);
                            cout << (!com_first ? "Blue: " : "Red: ") << next_move.first << ' ' << next_move.second << endl;
                            game.move(next_move.first, next_move.second);
                            round++;
                        }
                    }
                }
            }
        }
        draw();
        if (game.finish) {
            wait(500);
            state ^= 1;
        }
    }

    return 0;
}
