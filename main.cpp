/**
    3D MAZE GENERATOR using a modified Eller's algorithm.


                            Made by Fredrik Ulvin
                                (because it's fun, and probably come in handy)
**/

#include <iostream>
#include "Maze.h"

int main()
{
    Maze t(10, 5, 4, 0.5, 0.5);
    t.build();

    t.print();

    for(int lvl = 0; lvl < t.HEIGHT; lvl++){
        for(int row = 0; row < t.WIDTH; row++){
            for(int col = 0; col < t.LENGTH; col++){
                std::cout << t(row, col, lvl) << "  ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    return 0;
}
