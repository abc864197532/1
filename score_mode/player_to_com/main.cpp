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
    double line_cnt[3], s_op_rate[2], next_now_rate[2];
    int win = 0, draw = 0, lose = 0;
    Player() {
        line_cnt[0] = rdouble(rng);
        line_cnt[1] = rdouble(rng);
        line_cnt[2] = rdouble(rng);
        sort(line_cnt, line_cnt + 3);
        s_op_rate[0] = rdouble(rng);
        s_op_rate[1] = 1.0 - s_op_rate[0];
        next_now_rate[0] = rdouble(rng) / 2;
        next_now_rate[1] = 1.0 - next_now_rate[0];
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
                if (game.get_state(_i, _j, j, i) == -p) cnt = -3;
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
        line_cnt[2] = input_double();
        s_op_rate[0] = input_double();
        next_now_rate[0] = input_double();
        s_op_rate[1] = 1.0 - s_op_rate[0];
        next_now_rate[1] = 1.0 - next_now_rate[0];
    }
    friend ostream &operator<<(ostream &o, Player &p) {
        return o << p.line_cnt[0] << ' ' << p.line_cnt[1] << ' ' << p.line_cnt[2] << ' ' << p.s_op_rate[0] << ' ' << p.next_now_rate[0]
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
    Texture pi[9], db[10], dr[10];
    pi[0].loadFromFile(image_root + "/could_choose.png");
    pi[1].loadFromFile(image_root + "/empty.png");
    pi[2].loadFromFile(image_root + "/red.png");
    pi[3].loadFromFile(image_root + "/blue.png");
    pi[4].loadFromFile(image_root + "/blue_word.png");
    pi[5].loadFromFile(image_root + "/blue_win.png");
    pi[6].loadFromFile(image_root + "/red_word.png");
    pi[7].loadFromFile(image_root + "/red_win.png");
    pi[8].loadFromFile(image_root + "/block.png");
    for (int i = 0; i < 10; ++i) {
        db[i].loadFromFile(image_root + "/blue_digit/" + char('0' + i) + ".png");
        dr[i].loadFromFile(image_root + "/red_digit/" + char('0' + i) + ".png");
    }

    Sprite picture[9], blue_digit[10], red_digit[10];
    for (int i = 0; i < 9; ++i) picture[i] = Sprite(pi[i]);
    for (int i = 0; i < 10; ++i) blue_digit[i] = Sprite(db[i]);
    for (int i = 0; i < 10; ++i) red_digit[i] = Sprite(dr[i]);

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
                picture[8].setPosition(i * 3 * w + 5, j * 3 * w + 167);
                window.draw(picture[8]);
            }
        }
        int red_score = game.red_score, blue_score = game.blue_score;
        red_digit[red_score / 10].setPosition(0, 0);
        window.draw(red_digit[red_score / 10]);
        red_digit[red_score % 10].setPosition(w, 0);
        window.draw(red_digit[red_score % 10]);
        blue_digit[blue_score / 10].setPosition(7 * w, 0);
        window.draw(blue_digit[blue_score / 10]);
        blue_digit[blue_score % 10].setPosition(8 * w, 0);
        window.draw(blue_digit[blue_score % 10]);
        if (game.finish) {
            if (game.get_winner() == 1) {
                if (state == 0) {
                    picture[6].setPosition(2 * w, 0);
                    window.draw(picture[6]);
                } else {
                    picture[7].setPosition(2 * w, 0);
                    window.draw(picture[7]);
                }
            } else {
                if (state == 0) {
                    picture[4].setPosition(2 * w, 0);
                    window.draw(picture[4]);
                } else {
                    picture[5].setPosition(2 * w, 0);
                    window.draw(picture[5]);
                }
            }
        } else {
            if (game.cur_player == 1) {
                picture[6].setPosition(2 * w, 0);
                window.draw(picture[6]);
            } else {
                picture[4].setPosition(2 * w, 0);
                window.draw(picture[4]);
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
