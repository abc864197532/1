struct Player {
    double line_cnt[2], s_op_rate[2], next_now_rate[2];
    int win = 0, draw = 0, lose = 0;
    Player() {
        line_cnt[0] = rdouble(rng);
        line_cnt[1] = rdouble(rng);
        if (line_cnt[0] > line_cnt[1]) swap(line_cnt[0], line_cnt[1]);
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
                if (game.get_state(_i, _j, i, j) == -p) cnt = -3;
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
        s_op_rate[0] = input_double();
        next_now_rate[0] = input_double();
        s_op_rate[1] = 1.0 - s_op_rate[0];
        next_now_rate[1] = 1.0 - next_now_rate[0];
    }
    friend ostream &operator<<(ostream &o, Player &p) {
        return o << p.line_cnt[0] << ' ' << p.line_cnt[1] << ' ' << p.s_op_rate[0] << ' ' << p.next_now_rate[0]
                 << " Score " << p.win * 3 + p.draw << ' ' << p.win << "W " << p.draw << "D " << p.lose << "L";
    }
    bool operator < (const Player &other) const {
        return win * 3 + draw > other.win * 3 + other.draw;
    }
};
