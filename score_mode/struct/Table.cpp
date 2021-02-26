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
