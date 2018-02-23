#ifndef MAZE_H
#define MAZE_H

#include<vector>
#include<iostream>
class Maze
{
    public:
        Maze(int, int, int, double, double);

        int LENGTH;
        int WIDTH;
        int HEIGHT;

        static const uint8_t FLOOR     =  1;   // 00 00 00 01;
        static const uint8_t EAST      =  2;   // 00 00 00 10;
        static const uint8_t NORTH     =  4;   // 00 00 01 00;
        static const uint8_t WEST      =  8;   // 00 00 10 00;
        static const uint8_t SOUTH     = 16;   // 00 01 00 00;
        static const uint8_t CEIL      = 32;   // 00 10 00 00;

        std::vector<int> cell_data;

        int &operator()(int row, int col, int floor){
            return cell_data.at(col + LENGTH*row + LENGTH*WIDTH*floor);
        }
        void build();
        void print();
        virtual ~Maze();

    protected:

    private:
        // Variables
        double EAST_WALL_THRESHOLD;
        double SOUTH_WALL_THRESHOLD;
        std::vector< std::tuple<int, int, int, int, int, int> > passages;
        typedef std::tuple<int, int, int> pos;

        // Methods
        int get_val(int, int, int);
        void set_val(int, int, int, int);
        void calc_cell_values();
};

#endif // MAZE_H
