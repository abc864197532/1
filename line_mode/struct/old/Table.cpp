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