const int depth = 2, line_num[3][3]{3, 2, 3, 2, 4, 2, 3, 2, 3};

struct Player {
    double line_cnt[3], s_op_rate[2], small_big_rate[2], next_now_rate[2];
    int win = 0;
    Player() {
        do {
            line_cnt[0] = rdouble(rng);
            line_cnt[1] = rdouble(rng);
        } while (line_cnt[0] + line_cnt[1] >= 1);
        line_cnt[2] = 1.0 - line_cnt[0] - line_cnt[1];
        sort(line_cnt, line_cnt + 3);
        s_op_rate[0] = rdouble(rng), s_op_rate[1] = 1.0 - s_op_rate[0];
        small_big_rate[0] = rdouble(rng), small_big_rate[1] = 1.0 - small_big_rate[0];
        if (small_big_rate[0] > small_big_rate[1])
            swap(small_big_rate[0], small_big_rate[1]);
        next_now_rate[0] = rdouble(rng), next_now_rate[1] = 1.0 - next_now_rate[0];
        if (next_now_rate[0] > next_now_rate[1])
            swap(next_now_rate[0], next_now_rate[1]);
    }
    pair<int, int> best_move(Game game, int dep = depth) {
        vector<pair<int, int>> moves = game.valid_moves();
        pair<int, int> best = moves.front();
        double best_score = -INF;
        for (auto i : moves) {
            double score = get_rating(game, i.first, i.second, dep);
            if (score > best_score)
                best_score = score, best = i;
        }
        return best;
    }
    double get_rating(Game game, int x, int y, int dep) {
        if (dep == 0) return 0;
        double line_sum = 0, rat_small, rat_big, rat_s, rat_op = 0;
        //small
        //row
        int tmp_s = 0, tmp_op = 0;
        for (int i = 0; i < N; ++i)
            if (game.small[x / N][y / N].get_state(x % N, i) == game.Player()) tmp_s++;
            else if (game.small[x / N][y / N].get_state(x % N, i) == -game.Player()) tmp_op++;
        if (tmp_s == 0 || tmp_op == 0)
            line_sum += line_cnt[tmp_s] * s_op_rate[0] + line_cnt[tmp_op] * s_op_rate[1];
        //col
        tmp_s = 0, tmp_op = 0;
        for (int i = 0; i < 3; ++i)
            if (game.small[x / N][y / N].get_state(i, y % N) == game.Player()) tmp_s++;
            else if (game.small[x / N][y / N].get_state(i, y % N) == -game.Player()) tmp_op++;
        if (tmp_s == 0 || tmp_op == 0)
            line_sum += line_cnt[tmp_s] * s_op_rate[0] + line_cnt[tmp_op] * s_op_rate[1];
        //
        if (x % N == y % N) {
            tmp_s = 0, tmp_op = 0;
            for (int i = 0; i < 3; ++i)
                if (game.small[x / N][y / N].get_state(i, i) == game.Player()) tmp_s++;
                else if (game.small[x / N][y / N].get_state(i, i) == -game.Player()) tmp_op++;
            if (tmp_s == 0 || tmp_op == 0)
                line_sum += line_cnt[tmp_s] * s_op_rate[0] + line_cnt[tmp_op] * s_op_rate[1];
        }
        if (x % N == 2 - y % N) {
            tmp_s = 0, tmp_op = 0;
            for (int i = 0; i < 3; ++i)
                if (game.small[x / N][y / N].get_state(i, 2 - i) == game.Player()) tmp_s++;
                else if (game.small[x / N][y / N].get_state(i, 2 - i) == -game.Player()) tmp_op++;
            if (tmp_s == 0 || tmp_op == 0)
                line_sum += line_cnt[tmp_s] * s_op_rate[0] + line_cnt[tmp_op] * s_op_rate[1];
        }
        rat_small = line_sum / line_num[x % N][y % N];
        //big
        line_sum = 0;
        //row
        tmp_s = 0, tmp_op = 0;
        for (int i = 0; i < 3; ++i)
            if (game.big.get_state(x / N, i) == game.Player()) tmp_s++;
            else if (game.big.get_state(x / N, i) == -game.Player()) tmp_op++;
        if (tmp_s == 0 || tmp_op == 0)
            line_sum += line_cnt[tmp_s] * s_op_rate[0] + line_cnt[tmp_op] * s_op_rate[1];
        //col
        tmp_s = 0, tmp_op = 0;
        for (int i = 0; i < 3; ++i)
            if (game.big.get_state(i, y / N) == game.Player()) tmp_s++;
            else if (game.big.get_state(i, y / N) == -game.Player()) tmp_op++;
        if (tmp_s == 0 || tmp_op == 0)
            line_sum += line_cnt[tmp_s] * s_op_rate[0] + line_cnt[tmp_op] * s_op_rate[1];
        //
        if (x / N == y / N) {
            tmp_s = 0, tmp_op = 0;
            for (int i = 0; i < 3; ++i)
                if (game.big.get_state(i, i) == game.Player()) tmp_s++;
                else if (game.big.get_state(i, i) == -game.Player()) tmp_op++;
            if (tmp_s == 0 || tmp_op == 0)
                line_sum += line_cnt[tmp_s] * s_op_rate[0] + line_cnt[tmp_op] * s_op_rate[1];
        }
        if (x / N == 2 - y / N) {
            tmp_s = 0, tmp_op = 0;
            for (int i = 0; i < 3; ++i)
                if (game.big.get_state(i, 2 - i) == game.Player()) tmp_s++;
                else if (game.big.get_state(i, 2 - i) == -game.Player()) tmp_op++;
            if (tmp_s == 0 || tmp_op == 0)
                line_sum += line_cnt[tmp_s] * s_op_rate[0] + line_cnt[tmp_op] * s_op_rate[1];
        }
        rat_big = line_sum / line_num[x / N][y / N];
        rat_s = rat_small * small_big_rate[0] + rat_big * small_big_rate[1];
        //op
        game.move(x, y);
        // check whether the move could win
        if (game.Finish()) {
            if (game.Winner() != 0) return INF;
            return 0;
        }
        pair<int, int> op_best = best_move(game, dep - 1);
        rat_op = get_rating(game, op_best.first, op_best.second, dep - 1);
        return rat_s * next_now_rate[0] - rat_op * next_now_rate[1];
    }
    bool operator < (const Player &other) {
        return win > other.win;
    }
};
