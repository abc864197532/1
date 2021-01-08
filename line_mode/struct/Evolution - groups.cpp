void play(Player &p1, Player &p2) {
    Game game = Game();
    Player p[2]{p1, p2};//p[0] 1 first
    int s = 0;
    do {
        pair<int, int> next_move = p[s].best_move(game);
        game.move(next_move.first, next_move.second);
        s ^= 1;
    } while (!game.Finish());
    if (game.Winner() == 1) ++p1.win;
    else if (game.Winner() == -1) ++p2.win;
}

void print_player(Player &p) {
    for (int i = 0; i < 3; ++i) cout << p.line_cnt[i] << ' ';
    for (int i = 0; i < 2; ++i) cout << p.s_op_rate[i] << ' ';
    for (int i = 0; i < 2; ++i) cout << p.small_big_rate[i] << ' ';
    for (int i = 0; i < 2; ++i) cout << p.next_now_rate[i] << ' ';
    cout << "score " << p.win << endl;
}

Player generate_child(Player &p1, Player &p2) {
    Player par[2] = {p1, p2};
    Player ch;
    int choose = rint(0, 1);
    for (int i = 0; i < 3; i++) ch.line_cnt[i] = par[choose].line_cnt[i];
    choose = rint(0, 1);
    for (int i = 0; i < 2; i++) ch.s_op_rate[i] = par[choose].s_op_rate[i];
    choose = rint(0, 1);
    for (int i = 0; i < 2; i++) ch.small_big_rate[i] = par[choose].small_big_rate[i];
    choose = rint(0, 1);
    for (int i = 0; i < 2; i++) ch.next_now_rate[i] = par[choose].next_now_rate[i];
    // mutation
    if (rint(0, 999) < 20) {
        do {
            ch.line_cnt[0] = rdouble(rng);
            ch.line_cnt[1] = rdouble(rng);
        } while (ch.line_cnt[0] + ch.line_cnt[1] >= 1);
        ch.line_cnt[2] = 1.0 - ch.line_cnt[0] - ch.line_cnt[1];
        sort(ch.line_cnt, ch.line_cnt + 3);
    }
    if (rint(0, 999) < 20) {
        ch.s_op_rate[0] = rdouble(rng);
        ch.s_op_rate[1] = 1.0 - ch.s_op_rate[0];
    }
    if (rint(0, 999) < 20) {
        ch.small_big_rate[0] = rdouble(rng);
        ch.small_big_rate[1] = 1.0 - ch.small_big_rate[0];
        if (ch.small_big_rate[0] > ch.small_big_rate[1])
            swap(ch.small_big_rate[0], ch.small_big_rate[1]);
    }
    if (rint(0, 999) < 20) {
        ch.next_now_rate[0] = rdouble(rng), ch.next_now_rate[1] = 1.0 - ch.next_now_rate[0];
        if (ch.next_now_rate[0] > ch.next_now_rate[1])
            swap(ch.next_now_rate[0], ch.next_now_rate[1]);
    }
    return ch;
}

void evolution (vector<vector <Player>> &first, vector <vector <Player>> &second, int group, int population, int t) {
    int choose_number = population / 10, retire_number = population / 10 * 3;
    vector <int> id(group);
    for (int i = 0; i < group; ++i) id[i] = i;
    for (int _t = 1; _t <= t; ++_t) {
        shuffle(id.begin(), id.end(), rng);
        for (int cur_id = 0; cur_id < group; ++cur_id) {
            int firstid = cur_id, secondid = id[cur_id];
            vector <Player> firstchild, secondchild;
            for (int child_number = 0; child_number < retire_number; ++child_number) {
                shuffle(first[firstid].begin(), first[firstid].end(), rng);
                shuffle(second[secondid].begin(), second[secondid].end(), rng);
                for (int i = 0; i < choose_number; ++i) {
                    for (int j = 0; j < choose_number; ++j) {
                        play(first[firstid][i], second[secondid][j]);
                    }
                }
                // generate first child
                sort(first[firstid].begin(), first[firstid].end());
                firstchild.push_back(generate_child(first[firstid][0], first[firstid][1]));
                // generate second child
                sort(second[secondid].begin(), second[secondid].end());
                secondchild.push_back(generate_child(second[secondid][0], second[secondid][1]));
                for (int i = 0; i < choose_number; ++i) {
                    first[firstid][i].win = 0;
                    second[secondid][i].win = 0;
                }
            }
            for (int i = 0; i < population; ++i) {
                for (int j = 0; j < population; ++j) {
                    play(first[firstid][i], second[secondid][j]);
                }
            }
            // retire first bad gene
            {
                shuffle(first[firstid].begin(), first[firstid].end(), rng);
                sort(first[firstid].begin(), first[firstid].end());
                for (int i = 0; i < population; ++i) first[firstid][i].win = 0;
                for (int i = 0; i < retire_number; ++i) first[firstid][population - 1 - i] = firstchild[i];
            }
            // retire second bad gene
            {
                shuffle(second[secondid].begin(), second[secondid].end(), rng);
                sort(second[secondid].begin(), second[secondid].end());
                for (int i = 0; i < population; ++i) second[secondid][i].win = 0;
                for (int i = 0; i < retire_number; ++i) second[secondid][population - 1 - i] = secondchild[i];
            }
        }
    }
}
