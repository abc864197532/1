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
