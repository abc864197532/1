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
    cout << endl;
}

void evolution (vector<Player> &init, int population, int t) {
    int choose_number = population / 10, retire_number = population / 10 * 3;
    for (int _t = 1; _t <= t; ++_t) {
        vector <Player> child;
        for (int choose; child.size() < retire_number;) {
            shuffle(init.begin(), init.end(), rng);
            for (int i = 0; i < choose_number; ++i) {
                for (int j = 0; j < choose_number; ++j) {
                    if (i == j) continue;
                    play(init[i], init[j]);
                }
            }
            sort(init.begin(), init.end());
            Player par[2] = {init[0], init[1]};
            Player ch;
            choose = rint(0, 1);
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

            child.push_back(ch);
            for (int i = 0; i < choose_number; ++i) init[i].win = 0;
        }
        for (int i = 0; i < population; ++i) {
            for (int j = 0; j < population; ++j) {
                if (i == j) continue;
                play(init[i], init[j]);
            }
        }
        shuffle(init.begin(), init.end(), rng);
        sort(init.begin(), init.end());
        for (int i = 0; i < population; ++i) init[i].win = 0;
        for (int i = 0; i < retire_number; ++i) init[population - 1 - i] = child[i];
        cout << "Evolution " << _t << ":\n";
        cout << "Below are the vectors\n";
        for (Player &p : init) print_player(p);
    }
}

int main() {
    srand(time(NULL));
    int population = 20, number_of_change = 2;
    double tt = clock();
    cout << fixed << setprecision(6);
    freopen("output.txt", "w", stdout);
    vector <Player> P(population);
    evolution(P, population, number_of_change);
    for (int i = 0; i < population; ++i) {
        for (int j = 0; j < population; ++j) {
            if (i == j) continue;
            play(P[i], P[j]);
        }
    }
    sort(P.begin(), P.end());
    cout << "Finished!\n";
    cout << "Time: " << clock() - tt << endl;
    cout << "Below are the vectors\n";
    for (Player &p : P) print_player(p);
}
