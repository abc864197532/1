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
