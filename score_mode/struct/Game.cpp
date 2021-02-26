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
