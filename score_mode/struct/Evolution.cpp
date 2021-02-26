void play(Player &p1, Player &p2) {
    Game game = Game();
    Player p[2]{p1, p2}; //p[0] 1 first
    int s = 0, round = 0;
    do {
        pair<int, int> next_move = p[s].best_move(game, (round >= 20 ? 3 : 2));
        game.move(next_move.first, next_move.second);
        s ^= 1;
        round++;
    } while (!game.finish);
    if (game.get_winner() == 1) p1.win++, p2.lose++;
    else if (game.get_winner() == -1) p2.win++, p1.lose++;
    else p1.draw++, p2.draw++;
}

Player generate_child(Player &p1, Player &p2) {
    Player par[2] = {p1, p2};
    Player ch;
    {
        int choose = rint(0, 1);
        ch.line_cnt[0] = par[choose].line_cnt[0];
        ch.line_cnt[1] = par[choose].line_cnt[1];
        ch.ready_rate[0] = par[choose].ready_rate[0];
        ch.ready_rate[1] = par[choose].ready_rate[1];
        ch.ready_rate[2] = par[choose].ready_rate[2];
    }
    {
        int choose = rint(0, 1);
        ch.occupy = par[choose].occupy;
    }
    {
        int choose = rint(0, 1);
        ch.s_op_rate[0] = par[choose].s_op_rate[0];
        ch.s_op_rate[1] = par[choose].s_op_rate[1];
    }
    {
        int choose = rint(0, 1);
        ch.next_now_rate[0] = par[choose].next_now_rate[0];
        ch.next_now_rate[1] = par[choose].next_now_rate[1];
    }
    {
        int choose = rint(0, 1);
        for (int i = 0; i < 8; ++i) {
            ch.omega[i] = par[choose].omega[i];
        }
    }
    {
        int choose = rint(0, 1);
        ch.alpha = par[choose].alpha;
    }
    // mutation
    if (rint(0, 999) <= 20) {
        double a[4];
        for (int i = 0; i < 4; ++i) a[i] = rdouble(rng);
        sort(a, a + 4);
        ch.line_cnt[0] = a[0];
        ch.line_cnt[1] = a[1];
        ch.ready_rate[0] = 0;
        ch.ready_rate[1] = a[2];
        ch.ready_rate[2] = a[3];
    }
    if (rint(0, 999) <= 20) {
        ch.occupy = rdouble(rng) + 1;
    }
    if (rint(0, 999) <= 20) {
        ch.s_op_rate[0] = rdouble(rng);
        ch.s_op_rate[1] = 1.0 - ch.s_op_rate[0];
    }
    if (rint(0, 999) <= 20) {
        ch.next_now_rate[0] = rdouble(rng) / 2;
        ch.next_now_rate[1] = 1.0 - ch.next_now_rate[0];
    }
    if (rint(0, 999) <= 20) {
        ch.omega[0] = 1;
        ch.omega[1] = rdouble(rng);
        for (int i = 2; i < 8; ++i) ch.omega[i] = ch.omega[i - 1] * ch.omega[1];
    }
    if (rint(0, 999) <= 20) {
        ch.alpha = rdouble(rng) + 1;
    }
    return ch;
}

double tt;

void evolution (vector<vector <Player>> &first, vector <vector <Player>> &second, int group, int population, int t) {
    int choose_number = population / 10, retire_number = population / 10 * 3;
    vector <int> id(group);
    for (int i = 0; i < group; ++i) id[i] = i;
    for (int _t = 1; _t <= t; ++_t) {
        cout << "Start " << _t << endl;
        shuffle(id.begin(), id.end(), rng);
        for (int cur_id = 0; cur_id < group; ++cur_id) {
            cout << "Matching " << cur_id << endl;
            cout << "Now Time " << (clock() - tt) / 1000 << endl;
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
                    first[firstid][i].reset();
                    second[secondid][i].reset();
                }
            }
            cout << "Finish child generation" << endl;
            cout << "Now Time " << (clock() - tt) / 1000 << endl;
            for (int i = 0; i < population; ++i) {
                for (int j = 0; j < population; ++j) {
                    play(first[firstid][i], second[secondid][j]);
                }
            }
            // retire first bad gene
            {
                shuffle(first[firstid].begin(), first[firstid].end(), rng);
                sort(first[firstid].begin(), first[firstid].end());
                for (int i = 0; i < population; ++i) first[firstid][i].reset();
                for (int i = 0; i < retire_number; ++i) first[firstid][population - 1 - i] = firstchild[i];
            }
            // retire second bad gene
            {
                shuffle(second[secondid].begin(), second[secondid].end(), rng);
                sort(second[secondid].begin(), second[secondid].end());
                for (int i = 0; i < population; ++i) second[secondid][i].reset();
                for (int i = 0; i < retire_number; ++i) second[secondid][population - 1 - i] = secondchild[i];
            }
        }
    }
}
