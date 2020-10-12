#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
using namespace sf;

const int N = 3;

// -1 O 0 empty 1 X
struct Table {
    int state[N][N], blue_score, red_score, empty_cell;
    bool finish;

    Table () { // init
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                state[i][j] = 0;
            }
        }
        blue_score = 0;
        red_score = 0;
        empty_cell = N * N;
        finish = false;
    }

    void update_score() {
        int row[N] = {}, col[N] = {}, diag[2] = {};
        red_score = 0;
        blue_score = 0;
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                row[i] += state[i][j];
                col[j] += state[i][j];
            }
            diag[0] += state[i][i];
            diag[1] += state[i][N - i - 1];
        }
        for (int i = 0; i < N; ++i) {
            if (row[i] == N) {
                red_score++;
            }
            if (row[i] == -N) {
                blue_score++;
            }
            if (col[i] == N) {
                red_score++;
            }
            if (col[i] == -N) {
                blue_score++;
            }
        }
        for (int i = 0; i < 2; ++i) {
            if (diag[i] == N) {
                red_score++;
            }
            if (diag[i] == -N) {
                blue_score++;
            }
        }
    }

    bool Finish() {
        return empty_cell == 0;
    }

    bool isEmpty(int x, int y) {
        return state[x][y] == 0;
    }

    int Get_Red_Score() {
        return red_score;
    }

    int Get_Blue_Score() {
        return blue_score;
    }

    int get_state(int x, int y) {
        return state[x][y];
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
    Table small[N][N];
    int cur_player = 1, last_move_x = 0, last_move_y = 0, empty_cell = N * N * N * N;
    int red_score = 0, blue_score = 0;
    bool could_choose_all = true;

    bool valid(int x, int y) {
        if (!small[x / N][y / N].isEmpty(x % N, y % N)) {
            return false;
        }
        if (could_choose_all || (last_move_x == x / N && last_move_y == y / N)) {
            return true;
        }
        return false;
    }

    void update_score() {
        red_score = 0;
        blue_score = 0;
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                red_score += small[i][j].Get_Red_Score();
                blue_score += small[i][j].Get_Blue_Score();
            }
        }
    }

    bool move(int x, int y) {
        if (!valid(x, y)) {
            return false;
        }
        small[x / N][y / N].move(x % N,y % N, cur_player);
        small[x / N][y / N].update_score();
        update_score();
        empty_cell--;
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
        return empty_cell == 0;
    }

    int Winner() {
        if (red_score > blue_score) return 1;
        if (red_score < blue_score) return -1;
        return 0;
    }

    int Player() {
        return cur_player;
    }

    int Get_Red_Score() {
        return red_score;
    }

    int Get_Blue_Score() {
        return blue_score;
    }

    int get_output_picture(int x, int y) {
        if (small[x / N][y / N].get_state(x % N, y % N) == 1) {
            return 2;
        }
        if (small[x / N][y / N].get_state(x % N, y % N) == -1) {
            return 3;
        }
        if (!Finish() && valid(x, y)) {
            return 0;
        }
        return 1;
    }
};

int get_random() {
    return rand() % 9;
}

void wait(int time) {
    double t = clock();
    while (clock() - t < time);
}

int main() {
    srand(time(NULL));
    RenderWindow window(VideoMode(600, 760), "The Game!");

    std::string image_root = "C:/Users/smw93/Desktop/big_tic_tac_toe/image";
    Texture pi[8], db[10], dr[10];
    pi[0].loadFromFile(image_root + "/could_choose.png");
    pi[1].loadFromFile(image_root + "/empty.png");
    pi[2].loadFromFile(image_root + "/red.png");
    pi[3].loadFromFile(image_root + "/blue.png");
    pi[4].loadFromFile(image_root + "/blue_word.png");
    pi[5].loadFromFile(image_root + "/blue_win.png");
    pi[6].loadFromFile(image_root + "/red_word.png");
    pi[7].loadFromFile(image_root + "/red_win.png");
    for (int i = 0; i < 10; ++i) {
        db[i].loadFromFile(image_root + "/blue_digit/" + char('0' + i) + ".png");
        dr[i].loadFromFile(image_root + "/red_digit/" + char('0' + i) + ".png");
    }

    Sprite picture[8], blue_digit[10], red_digit[10];
    for (int i = 0; i < 8; ++i) picture[i] = Sprite(pi[i]);
    for (int i = 0; i < 10; ++i) blue_digit[i] = Sprite(db[i]);
    for (int i = 0; i < 10; ++i) red_digit[i] = Sprite(dr[i]);

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
                    if (x >= 0 && x < N * N && y >= 0 && y < N * N) {
                        game.move(x, y);
                    }
                }
            }
        }

        window.clear(Color::White);
        int red_score = game.Get_Red_Score(), blue_score = game.Get_Blue_Score();
        red_digit[red_score / 10].setPosition(0, 0);
        window.draw(red_digit[red_score / 10]);
        red_digit[red_score % 10].setPosition(w, 0);
        window.draw(red_digit[red_score % 10]);
        blue_digit[blue_score / 10].setPosition(7 * w, 0);
        window.draw(blue_digit[blue_score / 10]);
        blue_digit[blue_score % 10].setPosition(8 * w, 0);
        window.draw(blue_digit[blue_score % 10]);
        if (game.Finish()) {
            if (game.Winner() == 1) {
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
            if (game.Player() == 1) {
                picture[6].setPosition(2 * w, 0);
                window.draw(picture[6]);
            } else {
                picture[4].setPosition(2 * w, 0);
                window.draw(picture[4]);
            }
        }
        for (int i = 0; i < N * N; ++i) {
            for (int j = 0; j < N * N; ++j) {
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
