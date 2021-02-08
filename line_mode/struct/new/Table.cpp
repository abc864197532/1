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
