struct Player {
    double line_cnt[2], ready_rate[3], occupy, s_op_rate[2], next_now_rate[2], omega[8], alpha;
    int win = 0, draw = 0, lose = 0;
    Player() {
        double a[4];
        for (int i = 0; i < 4; ++i) a[i] = rdouble(rng);
        sort(a, a + 4);
        line_cnt[0] = a[0];
        line_cnt[1] = a[1];
        ready_rate[0] = 0;
        ready_rate[1] = a[2];
        ready_rate[2] = a[3];
        occupy = rdouble(rng) + 1;
        s_op_rate[0] = rdouble(rng);
        s_op_rate[1] = 1.0 - s_op_rate[0];
        next_now_rate[0] = rdouble(rng) / 2;
        next_now_rate[1] = 1.0 - next_now_rate[0];
        omega[0] = 1;
        omega[1] = rdouble(rng);
        for (int i = 2; i < 8; ++i) omega[i] = omega[i - 1] * omega[1];
        alpha = rdouble(rng) + 1;
    }
    pair<int, int> best_move(Game game, int dep) {
        vector<pair<int, int>> moves = game.valid_moves();
        pair<int, int> best = moves.front();
        double best_score = -INF;
        for (pair<int, int> i : moves) {
            double score = get_rating(game, i.first, i.second, dep);
            if (score > best_score)
                best_score = score, best = i;
        }
        return best;
    }
    double get_small_attack_rating(Game game, int x, int y, int p) { // p -> player
        if (game.small[x][y].finish) {
            if (game.small[x][y].winner == p) {
                return occupy;
            } else {
                return 0;
            }
        }
        bool ready[3][3];
        int ready_cnt = 0;
        for (int i = 0; i < 3; ++i) for (int j = 0; j < 3; ++j) {
            ready[i][j] = false;
            if (game.get_state(x, y, i, j) == 0) {
                {
                    // row
                    int cnt = 0;
                    for (int k = 0; k < 3; ++k) if (j != k) {
                        cnt += game.get_state(x, y, i, k);
                    }
                    if (cnt == p * 2) {
                        ready[i][j] = true;
                    }
                }
                {
                    // col
                    int cnt = 0;
                    for (int k = 0; k < 3; ++k) if (i != k) {
                        cnt += game.get_state(x, y, k, j);
                    }
                    if (cnt == p * 2) {
                        ready[i][j] = true;
                    }
                }
                if (i == j) {
                    // diag1
                    int cnt = 0;
                    for (int k = 0; k < 3; ++k) if (i != k) {
                        cnt += game.get_state(x, y, k, k);
                    }
                    if (cnt == p * 2) {
                        ready[i][j] = true;
                    }
                }
                if (i == 2 - j) {
                    // diag2
                    int cnt = 0;
                    for (int k = 0; k < 3; ++k) if (i != k) {
                        cnt += game.get_state(x, y, k, 2 - k);
                    }
                    if (cnt == p * 2) {
                        ready[i][j] = true;
                    }
                }
            }
            if (ready[i][j]) ready_cnt++;
        }
        double ready_score = ready_rate[min(ready_cnt, 2)], line_score = 0;
        vector <double> all_line;
        // row
        for (int i = 0; i < 3; ++i) {
            int cnt = 0;
            bool has_enemy = false, already = false;
            for (int j = 0; j < 3; ++j) {
                if (game.get_state(x, y, i, j) == p) {
                    cnt++;
                } else if (game.get_state(x, y, i, j) == -p) {
                    has_enemy = true;
                }
                if (ready[i][j]) already = true;
            }
            if (!already) {
                if (has_enemy) all_line.push_back(0);
                else all_line.push_back(line_cnt[cnt]);
            }
        }
        // col
        for (int j = 0; j < 3; ++j) {
            int cnt = 0;
            bool has_enemy = false, already = false;
            for (int i = 0; i < 3; ++i) {
                if (game.get_state(x, y, i, j) == p) {
                    cnt++;
                } else if (game.get_state(x, y, i, j) == -p) {
                    has_enemy = true;
                }
                if (ready[i][j]) already = true;
            }
            if (!already) {
                if (has_enemy) all_line.push_back(0);
                else all_line.push_back(line_cnt[cnt]);
            }
        }
        // diag1
        {
            int cnt = 0;
            bool has_enemy = false, already = false;
            for (int i = 0; i < 3; ++i) {
                if (game.get_state(x, y, i, i) == p) {
                    cnt++;
                } else if (game.get_state(x, y, i, i) == -p) {
                    has_enemy = true;
                }
                if (ready[i][i]) already = true;
            }
            if (!already) {
                if (has_enemy) all_line.push_back(0);
                else all_line.push_back(line_cnt[cnt]);
            }
        }
        // diag2
        {
            int cnt = 0;
            bool has_enemy = false, already = false;
            for (int i = 0; i < 3; ++i) {
                if (game.get_state(x, y, i, 2 - i) == p) {
                    cnt++;
                } else if (game.get_state(x, y, i, 2 - i) == -p) {
                    has_enemy = true;
                }
                if (ready[i][2 - i]) already = true;
            }
            if (!already) {
                if (has_enemy) all_line.push_back(0);
                else all_line.push_back(line_cnt[cnt]);
            }
        }
        if (!all_line.empty()) {
            sort(all_line.rbegin(), all_line.rend());
            double base = 0;
            for (int i = 0; i < all_line.size(); ++i) {
                line_score += all_line[i] * omega[i];
                base += omega[i];
            }
            line_score /= base;
        }
        return (ready_score + line_score) / 2;
    }
    double get_small_rating(Game game, int x, int y) {
        return get_small_attack_rating(game, x, y, game.cur_player) * s_op_rate[0] - get_small_attack_rating(game, x, y, -game.cur_player) * s_op_rate[1];
    }
    double get_big_rating(Game game) {
        if (game.finish) {
            if (game.get_winner() == game.cur_player) {
                return INF;
            } else if (game.get_winner() == -game.cur_player) {
                return -INF;
            } else {
                return 0;
            }
        }
        double small_rating[3][3], ready_score = 0, line_score = 0;
        vector <double> all_ready, all_line;
        for (int i = 0; i < 3; ++i) for (int j = 0; j < 3; ++j) {
            small_rating[i][j] = get_small_rating(game, i, j);
        }
        bool ready[3][3];
        for (int i = 0; i < 3; ++i) for (int j = 0; j < 3; ++j) {
            ready[i][j] = false;
            if (!game.small[i][j].finish) {
                {
                    // row
                    int cnt = 0;
                    for (int k = 0; k < 3; ++k) if (j != k) {
                        if (game.small[i][k].winner == game.cur_player) cnt++;
                    }
                    if (cnt == 2) {
                        ready[i][j] = true;
                    }
                }
                {
                    // col
                    int cnt = 0;
                    for (int k = 0; k < 3; ++k) if (i != k) {
                        if (game.small[k][j].winner == game.cur_player) cnt++;
                    }
                    if (cnt == 2) {
                        ready[i][j] = true;
                    }
                }
                if (i == j) {
                    // diag1
                    int cnt = 0;
                    for (int k = 0; k < 3; ++k) if (i != k) {
                        if (game.small[k][k].winner == game.cur_player) cnt++;
                    }
                    if (cnt == 2) {
                        ready[i][j] = true;
                    }
                }
                if (i == 2 - j) {
                    // diag2
                    int cnt = 0;
                    for (int k = 0; k < 3; ++k) if (i != k) {
                        if (game.small[k][2 - k].winner == game.cur_player) cnt++;
                    }
                    if (cnt == 2) {
                        ready[i][j] = true;
                    }
                }
            }
            if (ready[i][j]) all_ready.push_back(pow(alpha, small_rating[i][j]));
        }
        if (!all_ready.empty()) {
            sort(all_ready.rbegin(), all_ready.rend());
            double base = 0;
            for (int i = 0; i < all_ready.size(); ++i) {
                ready_score += all_ready[i] * omega[i];
                base += omega[i];
            }
            ready_score /= base;
        }
        // row
        for (int i = 0; i < 3; ++i) {
            double sum = 0;
            bool already = false;
            for (int j = 0; j < 3; ++j) {
                sum += small_rating[i][j];
                if (ready[i][j]) already = true;
            }
            if (!already) {
                all_line.push_back(pow(alpha, sum / 3));
            }
        }
        // col
        for (int j = 0; j < 3; ++j) {
            double sum = 0;
            bool already = false;
            for (int i = 0; i < 3; ++i) {
                sum += small_rating[i][j];
                if (ready[i][j]) already = true;
            }
            if (!already) {
                all_line.push_back(pow(alpha, sum / 3));
            }
        }
        // diag1
        {
            double sum = 0;
            bool already = false;
            for (int i = 0; i < 3; ++i) {
                sum += small_rating[i][i];
                if (ready[i][i]) already = true;
            }
            if (!already) {
                all_line.push_back(pow(alpha, sum / 3));
            }
        }
        // diag2
        {
            double sum = 0;
            bool already = false;
            for (int i = 0; i < 3; ++i) {
                sum += small_rating[i][2 - i];
                if (ready[i][2 - i]) already = true;
            }
            if (!already) {
                all_line.push_back(pow(alpha, sum / 3));
            }
        }
        if (!all_line.empty()) {
            sort(all_line.rbegin(), all_line.rend());
            double base = 0;
            for (int i = 0; i < all_line.size(); ++i) {
                line_score += all_line[i] * omega[i];
                base += omega[i];
            }
            line_score /= base;
        }
        return (ready_score + line_score) / 2;
    }
    double get_rating(Game game, int x, int y, int dep) {
        if (dep == 0) return 0;
        double rat_s = -get_big_rating(game), rat_op;
        game.move(x, y);
        // check whether the move could win
        if (game.finish) {
            if (game.get_winner() != 0) {
                return INF;
            } else {
                return 0;
            }
        }
        rat_s += get_big_rating(game);
        pair<int, int> op_best = best_move(game, dep - 1);
        rat_op = get_rating(game, op_best.first, op_best.second, dep - 1);
        return rat_s * next_now_rate[1] - rat_op * next_now_rate[0];
    }
    void reset() {
        win = draw = lose = 0;
    }
    void get_value_from_stdin() {
        cin >> line_cnt[0] >> line_cnt[1] >> ready_rate[1] >> ready_rate[2];
        ready_rate[0] = 0;
        cin >> occupy >> s_op_rate[0] >> next_now_rate[0] >> omega[1] >> alpha;
        s_op_rate[1] = 1.0 - s_op_rate[0];
        next_now_rate[1] = 1.0 - next_now_rate[0];
        omega[0] = 1;
        for (int i = 2; i < 8; ++i) omega[i] = omega[i - 1] * omega[1];
    }
    friend ostream &operator<<(ostream &o, Player &p) {
        return o << p.line_cnt[0] << ' ' << p.line_cnt[1] << ' ' << p.ready_rate[1] << ' ' << p.ready_rate[2] << ' '
        << p.occupy << ' ' << p.s_op_rate[0] << ' ' << p.next_now_rate[0] << ' ' << p.omega[1] << ' ' << p.alpha
        << " Score " << p.win * 3 + p.draw << ' ' << p.win << "W " << p.draw << "D " << p.lose << "L";
    }
    bool operator < (const Player &other) const {
        return win * 3 + draw > other.win * 3 + other.draw;
    }
};
