#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <time.h>

int welcome_menu();

static int window();

void draw_map(const int size, char field[][size]);

int solution(const int size, char field[][size], char player);

int check_cover(const int size, char field[][size], int x, int y, char player);

void territory_draw(const int size, char file[][size], int x, int y, char player);

char bot(const int size, char position[2]);


int main() {
    srand(time(NULL));
    // initialize the library
    initscr();
    //-------COLOR SET---------//
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);

    //----------INTRO----------//
    welcome_menu();
    getchar();
    //--------------------MAIN CODE HERE-----------------//
    int counter_a = 0;
    int counter_b = 0;
    char answer = 'y';

    while (answer != 'n') {

        //------MENU WINDOW--------//
        int size = window();
        //set cursor visible after menu
        curs_set(TRUE);
        //now we can typing
        nodelay(stdscr, FALSE);
        refresh();

        //drawing map chosen for menu
        char player = 'B';
        char field[size][size];
        draw_map(size, field);

        //score
        int MaxY, MaxX;
        getmaxyx(stdscr, MaxY, MaxX);
        mvprintw(0, (MaxX /2 ) +15, "%d", counter_a);
        mvprintw(1, (MaxX /2 ) +15, "%d", counter_b);
        refresh();

        // brain code goes here
        while (solution(size, field, player)) {
            // if its B switch to A and when its A switch to B
            //we need chance right position of A/B
            player = (player == 'A') ? 'B' : 'A';
            mvprintw(4 + size * 2, 12, "%c", player);
            mvprintw(4 + size * 2, 15, "     ");
            move(4 + size * 2, 15);

            //scan for player X Y move
            int x, y;
            //if bot turn go BOT else YOU
            if (player == 'B') {
                char position[2];
                bot(size, position);
                x = position[0];
                y = position[1];
            } else {
                scanw("%d %d", &x, &y);
            }

            // !! warnings  !! , first clean 'warning line'
            mvprintw(5 + size * 2, 0, "                          ");
            if (check_cover(size, field, x, y, player) != 1) {
                //we need change player for repeat move
                player = (player == 'A') ? 'B' : 'A';
                continue;
            }
            //rendering of the world
            territory_draw(size, field, x, y, player);
        }
        //score
        if (player == 'A') {
            counter_a++;
            mvprintw(0, MaxX / 2 + 15, "%d", counter_a);
        }
        if (player == 'B') {
            counter_b++;
            mvprintw(1, MaxX / 2 + 15, "%d", counter_b);
        }
        refresh();
        //next game
        mvprintw(7 + size * 2, 0, "Play again ? y/n ");
        scanw("%c", &answer);
        curs_set(FALSE);
        if (answer == 'n') {
            endwin();
            return EXIT_SUCCESS;
        }
        wmove(stdscr, 0, 0);
    }
    //---------------------------//
    //make sure that program is waiting for exiting
    getchar();
    // end curses
    endwin();
    return EXIT_SUCCESS;
}

char bot(const int size, char position[2]) {
    int x, y;
    //get random position
    x = (rand() % size) + 1;
    y = (rand() % size) + 1;
    position[0] = x, position[1] = y;
    refresh();
    return *position;
}

int solution(const int size, char field[][size], char player) {
    //browsing field
    for (int y = 1; y <= size; y++) {
        for (int x = 1; x <= size; x++) {
            //return '1' if there clear space
            if (field[y][x] != '*') {
                return 1;
            }
        }
    }
    //return '0' if there is no more clear space to play
    mvprintw(6 + size * 2, 0, "END OF THE GAME, player %c WIN!!!", player);
    refresh();
    return 0;
}

int check_cover(const int size, char field[][size], int x, int y, char player) {
    if (x < 1 || y < 1 || x > size || y > size) {
        //wrong position
        if (player != 'B') {
            mvprintw(5 + size * 2, 0, "Wrong position!   \n");
        }
        refresh();
        return 0;
    }

    if (field[y][x] == '*') {
        //something already here
        if (player != 'B') {
            mvprintw(5 + size * 2, 0, "Symbol is already there");
        }
        refresh();
        return -1;
    }
    return 1;
}

void territory_draw(const int size, char file[][size], int x, int y, char player) {
    //move to the right position
    int xx = x * 4;
    int yy = (size - y) * 2 + 1;
    move(yy, xx);
    // we need -- x,y because of offset in cycle(start in 0 not 1)
    //print Player and territory around
    int counter = 0;
    if (player == 'A') {

        //add '*'/something to files x,y
        file[y][x] = '*';
        attron(COLOR_PAIR(1));
        printw("X");
        attron(COLOR_PAIR(5));
    } else {
        file[y][x] = '*';
        attron(COLOR_PAIR(2));
        printw("O");
        attron(COLOR_PAIR(5));
        //delay for bot 0.9sec
        struct timespec ts = {
                .tv_sec = 0,
                .tv_nsec = 0.9 * 1000000000L
        };
        //delay
        nanosleep(&ts, NULL);
    }
    //offset repair
    //top right
    if (x == size && y == size) {
        file[y - 1][x] = '*';
        file[y][x - 1] = '*';
        file[y - 1][x - 1] = '*';
        mvprintw(yy, xx - 4, "*");
        mvprintw(yy + 2, xx, "*");
        mvprintw(yy + 2, xx - 4, "*");
        counter++;
    }
    //bottom right
    if (x == size && y == 1) {
        file[y + 1][x] = '*';
        file[y][x - 1] = '*';
        file[y + 1][x - 1] = '*';
        mvprintw(yy, xx - 4, "*");
        mvprintw(yy - 2, xx, "*");
        mvprintw(yy - 2, xx - 4, "*");
        counter++;
    }
    //top left
    if (x == 1 && y == size) {
        file[y - 1][x] = '*';
        file[y][x + 1] = '*';
        file[y - 1][x + 1] = '*';
        mvprintw(yy, xx + 4, "*");
        mvprintw(yy + 2, xx, "*");
        mvprintw(yy + 2, xx + 4, "*");
        counter++;
    }
    //bottom left
    if (x == 1 && y == 1) {
        file[y + 1][x] = '*';
        file[y][x + 1] = '*';
        file[y + 1][x + 1] = '*';
        mvprintw(yy, xx + 4, "*");
        mvprintw(yy - 2, xx, "*");
        mvprintw(yy - 2, xx + 4, "*");
        counter++;
    }
    // x == 1
    if (x == 1 && (y > 1 && y < size)) {
        file[y][x + 1] = '*';
        file[y + 1][x + 1] = '*';
        file[y - 1][x + 1] = '*';
        file[y + 1][x] = '*';
        file[y - 1][x] = '*';
        mvprintw(yy, xx + 4, "*");
        mvprintw(yy + 2, xx + 4, "*");
        mvprintw(yy - 2, xx + 4, "*");
        mvprintw(yy + 2, xx, "*");
        mvprintw(yy - 2, xx, "*");
        counter++;
    }
    // x == size
    if (x == size && (y > 1 && y < size)) {
        file[y][x - 1] = '*';
        file[y + 1][x - 1] = '*';
        file[y - 1][x - 1] = '*';
        file[y + 1][x] = '*';
        file[y - 1][x] = '*';
        mvprintw(yy, xx - 4, "*");
        mvprintw(yy + 2, xx - 4, "*");
        mvprintw(yy - 2, xx - 4, "*");
        mvprintw(yy + 2, xx, "*");
        mvprintw(yy - 2, xx, "*");
        counter++;
    }
    // y == size
    if (y == size && (x > 1 && x < size)) {
        file[y - 1][x] = '*';
        file[y - 1][x - 1] = '*';
        file[y - 1][x + 1] = '*';
        file[y][x - 1] = '*';
        file[y][x + 1] = '*';
        mvprintw(yy + 2, xx, "*");
        mvprintw(yy + 2, xx - 4, "*");
        mvprintw(yy + 2, xx + 4, "*");
        mvprintw(yy, xx - 4, "*");
        mvprintw(yy, xx + 4, "*");
        counter++;
    }
    // y == 1
    if (y == 1 && (x > 1 && x < size)) {
        file[y + 1][x] = '*';
        file[y + 1][x - 1] = '*';
        file[y + 1][x + 1] = '*';
        file[y][x - 1] = '*';
        file[y][x + 1] = '*';
        mvprintw(yy - 2, xx, "*");
        mvprintw(yy - 2, xx - 4, "*");
        mvprintw(yy - 2, xx + 4, "*");
        mvprintw(yy, xx - 4, "*");
        mvprintw(yy, xx + 4, "*");
        counter++;
    }
    // >>middle, normal position<<
    if (counter == 0) {
        // horizontal
        file[y][x + 1] = '*';
        mvprintw(yy, xx + 4, "*");
        file[y][x - 1] = '*';
        mvprintw(yy, xx - 4, "*");
        // vertica
        file[y + 1][x] = '*';
        mvprintw(yy + 2, xx, "*");
        file[y - 1][x] = '*';
        mvprintw(yy - 2, xx, "*");
        // corner
        file[y + 1][x + 1] = '*';
        mvprintw(yy + 2, xx + 4, "*");
        file[y + 1][x - 1] = '*';
        mvprintw(yy + 2, xx - 4, "*");
        file[y - 1][x + 1] = '*';
        mvprintw(yy - 2, xx + 4, "*");
        file[y - 1][x - 1] = '*';
        mvprintw(yy - 2, xx - 4, "*");
        counter--;
    }
    attroff(COLOR_PAIR(5));
    refresh();

}

void draw_map(const int size, char field[][size]) {
    clear();
    attron(COLOR_PAIR(4));
    //set cursor invisible
    curs_set(FALSE);
    //move cursor on [0,0]
    wmove(stdscr, 0, 0);
    int MaxY, MaxX;
    getmaxyx(stdscr, MaxY, MaxX);
    //first create first UP line
    printw("  +");
    for (int i = 0; i < size; i++) {
        printw("---+");
    }
    printw("\n");

    //than with cycle we will create other |  |
    //and ---+ as well
    for (int j = size; j > 0; j--) {
        // for the Y number
        attroff(COLOR_PAIR(4));
        printw("%d", j);
        attron(COLOR_PAIR(4));
        printw(" |");
        for (int i = 0; i < size; i++) {
            printw("   |", field[size - j][i]);
        }
        printw("\n");

        printw("  +");
        for (int i = 0; i < size; i++) {
            printw("---+");
        }
        printw("\n");
    }
    attroff(COLOR_PAIR(4));
    //for the X numbers
    printw("  ");
    for (int i = 0; i < size; i++) {
        printw("  %d ", i + 1);
    }

    printw("\n\n\nMove player A:");
    //clear hole window
    for (int i = 0; i < MaxY; i++) {
        printw("\n");
    }
    mvprintw(0, MaxX / 2 + 5, "Player A:");
    mvprintw(1, MaxX / 2 + 5, "Player B:");
    refresh();
    //set cursor visible
    curs_set(TRUE);

}

static int window() {
    clear();
    curs_set(FALSE);
    // window is draw immediately
    nodelay(stdscr, TRUE);
    attron(COLOR_PAIR(4));
    int size;
    //---GET SIZE OF SCREEN----//
    int MaxY, MaxX;
    getmaxyx(stdscr, MaxY, MaxX);
    //-----MENU---WINDOW--------//
    WINDOW *menu_window = newwin(8, MaxX - 12, (MaxY / 2) - 4, 7);
    //color for window
    wbkgd(menu_window, COLOR_PAIR(4));
    box(menu_window, 0, 0);
    refresh();
    wrefresh(menu_window);

    //make if we can use arrow keys
    keypad(menu_window, true);
    attroff(COLOR_PAIR(4));
    //chose form menu
    const char *choices[3] = {"6x6", "7x7", "8x8"};
    int choice = getch();
    int key_index = 1;
    attron(COLOR_PAIR(4));
    mvprintw(10, MaxX / 2 - 8, "Select size of map:");
    attroff(COLOR_PAIR(4));
    for (int i = 0; i < 3; i++) {
        mvprintw((MaxY / 2) - 1 + i, MaxX / 2, "%s", choices[i]);
    }
    refresh();
    //-------MENU BROWSING-------//
    //int size;
    while (1) {
        for (int i = 0; i < 3; i++) {
            if (i == key_index) {
                attron(A_REVERSE);
            }
            mvprintw((MaxY / 2) - 1 + i, MaxX / 2, "%s", choices[i]);
            attroff(A_REVERSE);

        }
        refresh();
        //KEYS UP & DOWN scrolling menu...
        choice = wgetch(menu_window);
        switch (choice) {
            case KEY_UP: {
                key_index--;
                if (key_index == -1) {
                    //offset
                    key_index = 0;
                }
                break;
            }
            case KEY_DOWN: {
                key_index++;
                if (key_index == 3) {
                    //offset
                    key_index = 2;
                }
                break;
            }
            case '\n': {
                if (key_index == 0) {
                    size = 6;
                    break;
                } else if (key_index == 1) {
                    size = 7;
                    break;
                } else if (key_index == 2) {
                    size = 8;
                    break;
                }
                refresh();
                break;
            }
            default:
                break;
        }
        refresh();
        //if ENTER break
        if (choice == 10) {
            break;
        }
    }
    return size;
}

int welcome_menu() {
    // invisible cursor, visibility of cursor (0,1,2)
    curs_set(FALSE);
    // getch() will be non-blocking
    nodelay(stdscr, TRUE);
    //apply needed function to create new window
    int MaxY, MaxX;
    getmaxyx(stdscr, MaxY, MaxX);
    keypad(stdscr, TRUE);
    curs_set(FALSE);

    char logo[6][81] = {
            {"                                                                               "},
            {"  _____ ____  _____  ______  _______  __   __ _____  ______   _  _____ ___ __  "},
            {" |     | ,  ||     ||__  __||  §  __||  | |  |   __||__  __| | ||     |    | | "},
            {" |  |  | __| |  ---|  | |   |    |   |  | |  |  |     | |    | ||  |  |    | | "},
            {" |  |  | ,  ||---  |  | |   |  |_ |  |  |_|  |  |__   | |    | ||  |  |  |   | "},
            {" |_____|____||_____|  |_|   |__| -_| |_______|_____|  |_|    |_||_____|__|___| "}
    };
    mvprintw(MaxY / 2 + 10, MaxX / 2 - 14, "Press any button for START");
    attron(COLOR_PAIR(4));
    int center = COLS / 2 - strlen(logo[0]) / 2;
    int target = 15;
    for (int row_count = 5; row_count >= 1; row_count--) {

        for (int y = 1; y <= target; y++) {
            move(y, center);
            printw("%s", logo[row_count]);
            refresh();
            // clear
            move(y, center);
            printw(logo[0]);
        }

        move(target, center);
        printw(logo[row_count]);

        target--;
    }
    attroff(COLOR_PAIR(4));


    return 0;
}
