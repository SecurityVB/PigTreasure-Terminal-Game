#include <iostream>
#include <random>
#include <cmath>
#include <conio.h>
#include <thread>
#include <chrono>
#include <cstdlib> 
using namespace std;


// Constants for visualization
const char                  pig{ 'o' };
const char                  space{ ' ' };
const char                  grass{ '#' };
const char                  earth{ '%' };
const char                  border_char{ '%' };

// Movement to the treasure
const int 					ROAD{ 100 }; // limit
random_device 				rd;
mt19937 					gen(rd());
uniform_int_distribution<> 	place_rand(0, ROAD);

// Excavation of treasure
const int 					x_limit{ 10 };
const int 					y_limit{ 5 };
const int                   bombs_count{ 4 }; // Number of bombs
uniform_int_distribution<> 	x_rand(0, x_limit);
uniform_int_distribution<> 	y_rand(0, y_limit);

// Game
void lose();
void win();

// Constituent
void check_markup(int ch, int* move, bool status); // true - movement_to_the_treasure()

// Movement to the treasure
void Movement_to_the_treasure();
void movement_make_move(int* move, int* pig_place, int* treasure_place, int* road);
void check_nearby_treasure(int* pig_place, int* treasure_place);
void area_of_visibility(int* pig_place, int* treasure_place, int* road);

// Excavation of treasure
void Excavation_of_treasure();
void check_locations(int* pig_place, int* treasure_place);
void generate_bombs(int (*bombs)[2], int* limits, int* treasure_place);
int excavation_make_move(int* move, int* pig_place, int* treasure_place, int(*bombs)[2], int* limits, int count);
void area_excavations(int* pig_place, int* treasure_place, int* limits);

// Output
void table_out(int* pig_place, string line1, string line2, bool bombs);
void printFrame(const std::string& frame);
void print_you_lose();
void print_you_won();

// Support
void formation_line1_line2(int* pig_place, bool bombs);
void formation_grounds_spaces(int* pig_place, int& grounds, int& line2_spaces);





// Constituent
int main()
{
    Movement_to_the_treasure();

    system("cls");

    Excavation_of_treasure();
    
    return 0;
}



void check_markup(int ch, int* move, bool status)
{
    move[0] = 0;
    move[1] = 0;
    if (status == true) {
        switch (ch) {
        case 72: // arrow to up
            move[1] = 1;
            break;
        case 80: // arrow to down
            move[1] = -1;
            break;
        case 75: // arrow еo left
            move[0] = 1;
            break;
        case 77: // arrow to right
            move[0] = -1;
            break;
        default:
            break;
        }
    }
    else {
        switch (ch) {
        case 72: // arrow to up
            move[1] = -1;
            break;
        case 80: // arrow to down
            move[1] = 1;
            break;
        case 75: // arrow еo left
            move[0] = -1;
            break;
        case 77: // arrow to right
            move[0] = 1;
            break;
        default:
            break;
        }
    }
}





// Movement to treasure
void Movement_to_the_treasure() {
    int 						road[2]{ 0, ROAD };
    int 						pig_place[2]{ place_rand(gen), place_rand(gen) };
    int 						treasure_place[2]{ place_rand(gen), place_rand(gen) };
    int 						move[2]{};
    int 						ch{};


    while (pig_place[0] != treasure_place[0] || pig_place[1] != treasure_place[1])
    {
        system("cls");
        check_nearby_treasure(pig_place, treasure_place);
        area_of_visibility(pig_place, treasure_place, road);
        formation_line1_line2(pig_place, false);

        while (!_kbhit());
        ch = _getch();
        check_markup(ch, move, true);

        movement_make_move(move, pig_place, treasure_place, road);
    }
}



void movement_make_move(int* move, int* pig_place, int* treasure_place, int* road)
{
    int new_pig_place[2]{ pig_place[0] + move[0], pig_place[1] + move[1] };

    if (new_pig_place[0] >= road[0] && new_pig_place[0] <= road[1] &&
        new_pig_place[1] >= road[0] && new_pig_place[1] <= road[1])
    {
        pig_place[0] = new_pig_place[0];
        pig_place[1] = new_pig_place[1];
    }
    
}



void check_nearby_treasure(int* pig_place, int* treasure_place)
{
    int difference[2]{ abs(pig_place[0] - treasure_place[0]), abs(pig_place[1] - treasure_place[1]) };

    if (difference[0] <= 7 && difference[1] <= 7)
    {
        for (int i{}; i < 25; i++) {
            cout << border_char;
        }
        cout << '\n';

        cout << border_char << border_char << "   " << "Treasure nearby" << "   "  << border_char << border_char << '\n';

        for (int i{}; i < 25; i++) {
            cout << border_char;
        }
        cout << '\n';
    }
}



void area_of_visibility(int* pig_place, int* treasure_place, int* road)
{
    char nothing{ '.' };
    char treasure{ 'x' };

    int x_visibility[5]{
        pig_place[0] + 2, pig_place[0] + 1, pig_place[0], pig_place[0] - 1, pig_place[0] - 2
    };
    int y_visibility[5]{
        pig_place[1] + 2, pig_place[1] + 1, pig_place[1], pig_place[1] - 1, pig_place[1] - 2
    };

    for (int y : y_visibility)
    {
        for (int x : x_visibility)
        {
            if (x >= road[0] && x <= road[1] && y >= road[0] && y <= road[1])
            {
                if (x == pig_place[0] && y == pig_place[1])
                {
                    cout << pig << space;
                }
                else if (x == treasure_place[0] && y == treasure_place[1])
                {
                    cout << treasure << space;
                }
                else
                {
                    cout << nothing << space;
                }
            }
            else
            {
                cout << space << space;
            }
        }
        cout << '\n';
    }
    cout << '\n';
}





// Excavation
void Excavation_of_treasure() {
    int                         treasure_place[2]{ x_rand(gen), y_rand(gen) };
    int 						limits[2]{ x_limit, y_limit };
    int 						pig_place[2]{ x_rand(gen), -1};
    int 						move[2]{};
    int 						ch{};
    int                         count{ 0 }; // For count -1 [y]
    int                         bombs[bombs_count][2]{};
    int                         lines_count{2};

    check_locations(pig_place, treasure_place);
    generate_bombs(bombs, limits, treasure_place);
    area_excavations(pig_place, treasure_place, limits);

    while (pig_place[0] != treasure_place[0] || pig_place[1] != treasure_place[1])
    {
        formation_line1_line2(pig_place, true);

        while (!_kbhit());
        ch = _getch();
        check_markup(ch, move, false);

        system("cls");

        count = excavation_make_move(move, pig_place, treasure_place, bombs, limits, count);
        area_excavations(pig_place, treasure_place, limits);
    }
    win();
}



void check_locations(int* pig_place, int* treasure_place) {
    if (pig_place[0] == x_limit) {
        pig_place[0] -= 1;
    }
    else if (treasure_place[0] == x_limit) {
        treasure_place[0] -= 1;
    }
    else if (treasure_place[1] == y_limit) {
        treasure_place[1] -= 1;
    }
}



void generate_bombs(int (*bombs)[2], int* limits, int* treasure_place) {
    for (int i{ 1 }; i <= bombs_count; i++) {
        bombs[bombs_count][0] = x_rand(gen);
        bombs[bombs_count][1] = y_rand(gen);

        while (bombs[bombs_count][0] == treasure_place[0] && bombs[bombs_count][1] == treasure_place[1]) {
            bombs[bombs_count][0] = x_rand(gen);
            bombs[bombs_count][1] = y_rand(gen);
        }

        if (bombs[bombs_count][0] == limits[0]) {
            bombs[bombs_count][0] -= 1;
        }
        else if (bombs[bombs_count][1] == limits[1]) {
            bombs[bombs_count][1] -= 1;
        }
    }
    
}



int excavation_make_move(int* move, int* pig_place, int* treasure_place, int (*bombs)[2], int* limits, int count) {
    int new_pig_place[2]{ pig_place[0] + move[0], pig_place[1] + move[1] };

    if (new_pig_place[0] >= 0 && new_pig_place[0] < limits[0] &&
        new_pig_place[1] >= 0 && new_pig_place[1] < limits[1])
    {
        pig_place[0] = new_pig_place[0];
        pig_place[1] = new_pig_place[1];
        ++count;
    }
    else if (new_pig_place[0] >= 0 && new_pig_place[0] < limits[0] &&
        new_pig_place[1] == -1 && count == 0)
    {
        pig_place[0] = new_pig_place[0];
        pig_place[1] = new_pig_place[1];
    }

    for (int i{ 1 }; i <= bombs_count; i++) {
        if (new_pig_place[0] == bombs[bombs_count][0] && new_pig_place[1] == bombs[bombs_count][1]) {
            lose();
        }
    }

    return count;
}



void area_excavations(int* pig_place, int* treasure_place, int* limits) {
    char    material{};

    if (pig_place[1] == -1) {
        for (int xx{}; xx < x_limit+1; xx++) {
            if (pig_place[0] == xx) {
                cout << pig << space;
            }
            else {
                cout << space << space;
            }
        }
        cout << '\n';
    }

    for (int y{}; y < y_limit; y++) {
        if (y == 0) {
            material = grass;
        }
        else {
            material = earth;
        }
        for (int x{}; x < x_limit; x++) {
                if (pig_place[0] == x && pig_place[1] == y) {
                    cout << pig << space;
                }
                else {
                    cout << material << space;
                }
        }
        cout << '\n';
    }
    cout << '\n';
}





// Game 
void lose() {
    system("cls");

    const std::string frames[] = {
            "    *    \n"
            "   ***   \n"
            "  *****  \n"
            "    *    \n"
            "         \n",

            "         \n"
            "  * * *  \n"
            "   ***   \n"
            "  * * *  \n"
            "         \n",

            "         \n"
            "  * * *  \n"
            " *  *  * \n"
            "  * * *  \n"
            "         \n",

            "         \n"
            " * * * * \n"
            "* * * * *\n"
            " * * * * \n"
            "         \n",

            "         \n"
            "  * * *  \n"
            " *  *  * \n"
            "  * * *  \n"
            "         \n",

            "         \n"
            "   ***   \n"
            "  *****  \n"
            "   ***   \n"
            "         \n",

            "         \n"
            "         \n"
            "    *    \n"
            "         \n"
            "         \n"
    };

    const int numFrames = sizeof(frames) / sizeof(frames[0]);

    for (int i = 0; i < numFrames; ++i) {
        printFrame(frames[i]);
    }
    system("cls");
    print_you_lose();
    system("pause");
    exit(0);
}



void win() {
    system("cls");
    print_you_won();
    system("pause");
    exit(0);
}





// Output functions
void table_out(int* pig_place, string line1, string line2, bool bombs) {
    int     grounds{ 36 };
    int     line2_spaces{ 18 };
    string  side{ border_char, border_char };


    if (bombs == false) {
        formation_grounds_spaces(pig_place, grounds, line2_spaces);
    }

    // up ground
    for (int i{ 1 }; i <= grounds; i++) {
        cout << border_char;
    }
    cout << '\n';

    // line 1
    cout << side << "   " << line1 << "   " << side << '\n';

    // line 2
    cout << side << "   " << line2;
    for (int i{ 1 }; i <= line2_spaces; i++) {
        cout << ' ';
    }
    cout << side << '\n';

    // down ground
    for (int i{ 1 }; i <= grounds; i++) {
        cout << border_char;
    }
    cout << '\n';
}


void printFrame(const std::string& frame) {
    system("cls");
    std::cout << frame << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}


void print_you_lose() {
    cout << '\n';
    cout << "  %%    %%  %%%%%%  %%    %%      %%      %%%%%%  %%%%%%  %%%%%  \n";
    cout << "   %%  %%   %%  %%  %%    %%      %%      %%  %%  %%      %%     \n";
    cout << "     %%     %%  %%  %%    %%      %%      %%  %%  %%%%%%  %%%%%  \n";
    cout << "     %%     %%  %%   %%  %%       %%  %%  %%  %%      %%  %%     \n";
    cout << "     %%     %%%%%%    %%%%        %%%%%%  %%%%%%  %%%%%%  %%%%%  \n";
    cout << '\n';
}

void print_you_won() {
    cout << '\n';
    cout << "  %%    %%  %%%%%%  %%    %%      %%         %%  %%%%%%  %%%   %%  \n";
    cout << "   %%  %%   %%  %%  %%    %%      %%         %%  %%  %%  %%%%  %%  \n";
    cout << "     %%     %%  %%  %%    %%      %%   %%%   %%  %%  %%  %% %% %%  \n";
    cout << "     %%     %%  %%   %%  %%        %% %% %% %%   %%  %%  %%  %%%%  \n";
    cout << "     %%     %%%%%%    %%%%          %%%   %%%    %%%%%%  %%   %%%  \n";
    cout << '\n';
}





// support functions
void formation_line1_line2(int* pig_place, bool bombs) {
    string      spaces{ "    " };
    string      line1{};
    string      line2{};

    if (bombs == true) {
        if (-pig_place[1] < 0) {
            spaces = "   ";
        }
        line1 = "Pig place: " + to_string(pig_place[0]) + ' '
            + to_string(-pig_place[1]) + spaces + "Bombs: "
            + to_string(bombs_count);
    }
    else{
        line1 = "Pig place: " + to_string(pig_place[0]) + ' '
            + to_string(pig_place[1]);
    }
    line2 = "Make a move";


    table_out(pig_place, line1, line2, bombs);
}


void formation_grounds_spaces(int* pig_place, int& grounds, int& line2_spaces) {
    grounds = 24;
    line2_spaces = 6;

    for (int i{}; i < 2; i++) {
        if (pig_place[i] > 9 && pig_place[i] <= 99) {
            grounds += 1;
            line2_spaces += 1;
        }
        else if (pig_place[i] > 99) {
            grounds += 2;
            line2_spaces += 2;
        }
    }
}