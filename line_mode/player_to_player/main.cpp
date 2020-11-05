#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
using namespace sf;
using namespace std;

const int N = 3;

// -1 O 0 empty 1 X
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

void wait(int time) {
    double t = clock();
    while (clock() - t < time);
}

int main() {
    srand(time(NULL));
    RenderWindow window(VideoMode(600, 760), "The Game!");

    std::string image_root = "C:/Users/smw93/Desktop/big_tic_tac_toe/image";
    Texture pi[14];
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

    Sprite picture[14];
    for (int i = 0; i < 14; ++i) picture[i] = Sprite(pi[i]);

    int w = 65, state = 0;
    Game game;

    while (window.isOpen()) {
        Vector2i pos = Mouse::getPosition(window);

        int x = pos.x / w;
        int y = (pos.y - 160) / w;

        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                window.close();
            }

            if (!game.Finish() && e.type == Event::MouseButtonPressed) {
                if (e.key.code == Mouse::Left) {
                    if (x >= 0 && x < 9 && y >= 0 && y < 9) {
                        cout << (game.Player() == 1 ? "Red " : "Blue: ") << x << ' ' << y << endl;
                        game.move(x, y);
                    }
                }
            }
        }

        window.clear(Color::White);
        if (game.Finish()) {
            if (game.Winner() == 1) {
                if (state == 0) {
                    picture[12].setPosition(2 * w, 0);
                    window.draw(picture[12]);
                } else {
                    picture[13].setPosition(2 * w, 0);
                    window.draw(picture[13]);
                }
            } else {
                if (state == 0) {
                    picture[10].setPosition(2 * w, 0);
                    window.draw(picture[10]);
                } else {
                    picture[11].setPosition(2 * w, 0);
                    window.draw(picture[11]);
                }
            }
        } else {
            if (game.Player() == 1) {
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
        if (game.Finish()) {
            wait(500);
            state ^= 1;
        }
    }

    return 0;
}
