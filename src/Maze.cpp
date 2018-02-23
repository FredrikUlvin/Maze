/*****************************************************************************************
 **                     3D MAZE (Now less cyclic!)                                      **
 **         Makes and contains descriptive information of a maze in three dimensions.   **
 **         A modified Eller's algorithm is used to generate the maze.                  **
 **         As of now it is an imperfect implementation, since each set on each floor   **
 **         only have one access point to the floor above.                              **
 **             (This will be remedied in the future)                                   **
 **                                                                                     **
 *****************************************************************************************/

#include "Maze.h"
#include <iostream>
#include <vector>
#include <set>
#include <tuple>
#include <map>
#include <random>
#include <time.h>
#include <numeric>

Maze::Maze(int columns, int rows, int floors, double horizontal_bias, double vertical_bias)
{
    /* The Maze class contructor
     *       Input:
     *           int columns - Set the number of columns in the maze. Correspond to the X axis (length).
     *           int rows    - Set the number of rows in the maze.    Correspond to the Y axis (width).
     *           int floors  - Set the number of floors in the maze.  Correspond to the Z axis (height).
     *
     *                          All three must be larger than 1.
     *
     *           double horizontal_bias - Sets the likelihood of a passage between rooms on the same row.
     *           double vertical_bias   - Sets the likelihood of a passage between rooms in the same column.
     *
     *                       Both must be between 0 and 1 exclusive.
     *                       Higher number gives higher likelihood of a passage.
     *
     *       Output:
     *           Maze object
     */
    if (rows < 1 || columns < 1 || floors < 1){
        std::cout << "A maze must have dimensions greater than zero.\n";
        throw std::invalid_argument("A maze must have dimensions greater than zero.\n");
    }
    LENGTH = columns;
    WIDTH  = rows;
    HEIGHT = floors;

    if (horizontal_bias <= 0 || horizontal_bias >= 1 || vertical_bias <= 0 || vertical_bias >= 1){
        std::cout << "Biases must be between 0 and 1 exclusive.\n";
        throw std::invalid_argument("Biases must be between 0 and 1 exclusive.\n");
    }
    EAST_WALL_THRESHOLD = horizontal_bias;
    SOUTH_WALL_THRESHOLD = vertical_bias;

    //Creating a vector of all rooms.
    cell_data.assign(LENGTH*WIDTH*HEIGHT, -1);

    // Give each room a different set number (ascending).
    // (Which is used during maze generation)
    std::iota(std::begin(cell_data), std::end(cell_data), 0);

};

int Maze::get_val(int i, int col, int floor){
    // Gets the value stored at row i, column j, level k, in cell_data;
    return cell_data.at(col + LENGTH*i + LENGTH*WIDTH*floor);
}

void Maze::set_val(int row, int col, int floor, int val){
    // Set the value of cell at (row, col, floor) to val
    cell_data[col + LENGTH*row + LENGTH*WIDTH*floor] = val;
    return;
}

void Maze::build(){
    /*      This function generates a maze.
     *
     *      It uses sets to keep track of areas created by removing walls.
     *      Initially each room has it's own set, known by it's room value.
     *      The room sets grows as walls are removed and sets joined.
     *      To keep track, each room value is mapped to itself, or the room value it has joined.
     *      The mapping and sets are reset at each floor.
     *
     *      Summary:
     *      It goes through each cell, removing the walls randomly.
     *      (First WEST to EAST, then NORTH to SOUTH, then DOWN to UP.
     *          Just as one write in English, putting each new page on top of the other)
     *      If a wall is between two rooms in the same set, it is not removed.
     *
     *      Each room value denote which set it belongs to
     *
     *      Each wall removed is stored in as a passage between the two rooms.
     *          (The rooms position is stored in Maze::passages)
     *
     *          If the current room value is not mapped, then map it to itself.
     *          Set current room value to it's mapped value.
     *
     *          Add the room to the set given by room value
     *
     *          If a EASTERN wall is removed, then:
     *              - The set of the connected room is added to the set of this room.
     *              - The set of the connected room is deleted.
     *              - Map the connected room value to the current room value.
     *              - Change the connected room value to the current room value.
     *              - Add connected room to set.
     *
     *          If a SOUTHERN wall is removed, then:
     *              - Change the connected room value to the current room value.
     *              - Add the connected room to set.
     *
     *      After all the rooms of the current floor has been visited:
     *          - Go through each set, pick a random room, and make a passage up.
     *          - Clear the sets, and mapping.
     *
     *      On the last floor, each set on each row must have a passage south.
     *
     *      On the last row on the last floor, passages between unconnected sets are removed.
     *
     *      Finally each room value is set according to which walls remain. (Including floor and ceiling)
     */


    // The room sets
    std::map<std::size_t, std::vector<pos>> room_set;

    // The mapping of connected rooms
    std::map<int, int> merged_room_sets;


    int room_value;
    int east_room_value;
    int south_room_value;

    // To get different mazes each time
    srand(time(NULL));

    for (int floor = 0; floor < HEIGHT - 1; floor++){ // Go through each floor but the last.

        for (int row = 0; row < WIDTH; row++){ // Go through each row

            for(int col = 0; col < LENGTH; col++){ // Go through each element in row (Each column)

                room_value = get_val(row, col, floor);

                // If this room value is mapped to another, get that value: else map value to itself
                if(merged_room_sets.find(room_value) != merged_room_sets.end()){
                    room_value = merged_room_sets[room_value];
                } else {
                    merged_room_sets[room_value] = room_value;
                }

                // If not the last cell in row, store eastern room value
                if (col<LENGTH-1){
                                       //Is the eastern room value mapped?
                    east_room_value = (merged_room_sets.find(get_val(row, col+1, floor))!=merged_room_sets.end()) ?
                                            // Yes: Use mapped value
                                                merged_room_sets[get_val(row, col+1, floor)] :
                                            // No: Use stored value
                                                get_val(row, col+1, floor);
                }

                // If not the last row, store southern room value
                if (row < WIDTH-1){
                                        //Is the southern room value mapped?
                    south_room_value = (merged_room_sets.find(get_val(row + 1, col, floor))!=merged_room_sets.end()) ?

                                            // Yes: Use mapped value
                                                merged_room_sets[get_val(row + 1, col, floor)] :

                                            // No:  Use stored value
                                                get_val(row + 1, col, floor);
                }

                // Add this room to the set of room value
                room_set[room_value].push_back(std::make_tuple(row, col, floor));

                //If last room in row, skip (don't remove eastern boundary wall)
                if(col == LENGTH - 1){}

                // If this and eastern room not in same set, maybe remove wall
                else if((rand() < EAST_WALL_THRESHOLD * RAND_MAX) && (room_value != east_room_value)){
                    //      EASTERN WALL REMOVED

                    // Get set of eastern room
                    std::vector<pos> next_room_position_set = room_set[get_val(row, col+1, floor)];

                    // Add set of eastern room to this set.
                    room_set[room_value].insert(room_set[room_value].end(), next_room_position_set.begin(), next_room_position_set.end() );

                    // Remove eastern room set
                    room_set.erase(get_val(row, col+1, floor));

                    // Map eastern room value to this room value.
                    merged_room_sets[get_val(row,col+1,floor)] = room_value;

                    // Set eastern room value to this
                    set_val(row, col+1, floor, room_value);

                    // Add the new horizontal passage created.
                    passages.push_back(std::make_tuple(row, col, floor, row, col+1, floor));

                }   // Else the eastern wall remain

                // Don't remove southern boundary wall.
                if(row == WIDTH - 1){}

                // If current and southern room is not in same set, then maybe remove wall
                else if(rand() < SOUTH_WALL_THRESHOLD * RAND_MAX && room_value != south_room_value){

                    // Set southern room value to this one
                    set_val(row+1, col, floor, room_value);

                    // Add the newly created vertical passage.
                    passages.push_back(std::make_tuple(row, col, floor, row+1, col, floor));
                }   //Else wall down remain

            } // All rooms in this row has been visited

        } // All rows on this floor has been visited

        // Clear room set mapping
        merged_room_sets.clear();

        // Go through all sets this floor
        for(auto entry : room_set)
        {
            auto group = entry.second;
            int x, y, z;

            // Picking out a random room in each group
            // Room position saved in matrix notation (i, j, k = y, x, z)
            std::tie(y, x, z) = group.at(rand() % group.size());

            // Only one passage up for each group. To avoid graph cycles
            passages.push_back(std::make_tuple(y, x, z, y, x, z+1));
        }
        // Passages up have been set. Clearing room groups for next floor.
        room_set.clear();

    }// Finished with all but last floor


    int floor = HEIGHT - 1;

    std::set<int> can_go_south;

    //deal with last floor, but leave the last row
    for(int row = 0; row < WIDTH - 1; row++){

        for(int col = 0; col<LENGTH; col++){
            room_value = get_val(row, col, floor);

            // If this room value is mapped to another, get that value: else map value to itself
            if(merged_room_sets.find(room_value) != merged_room_sets.end()){
                    room_value = merged_room_sets[room_value];
            } else {
                    merged_room_sets[room_value] = room_value;
            }

            // If not the last cell in row, get eastern room value
            if (col<LENGTH-1){
                    east_room_value = (merged_room_sets.find(get_val(row, col+1, floor)) != merged_room_sets.end()) ?
                                       merged_room_sets[get_val(row, col+1, floor)] : get_val(row, col+1, floor);
            }
            // Get southern room value
            south_room_value = (merged_room_sets.find(get_val(row + 1, col, floor)) != merged_room_sets.end()) ?
                                merged_room_sets[get_val(row + 1, col, floor)] : get_val(row + 1, col, floor);

            // Try and make passage east
            if(col == LENGTH -1){}
            else if((rand() < EAST_WALL_THRESHOLD * RAND_MAX) && (room_value != east_room_value)){

                passages.push_back(std::make_tuple(row, col, floor, row, col+1, floor));
                merged_room_sets[east_room_value] = room_value;
                set_val(row, col+1, floor, room_value);
            }

            // Try and make passage south
            if( (      (rand() < SOUTH_WALL_THRESHOLD * RAND_MAX)
                    || (can_go_south.find(room_value) == can_go_south.end()) ) // Make sure all sets this row have a passage down
                && (room_value != south_room_value))
            {
                passages.push_back(std::make_tuple(row, col, floor, row+1, col, floor));
                merged_room_sets[south_room_value] = room_value;
                set_val(row+1, col, floor, room_value);
                can_go_south.insert(room_value);
            }
        }

        can_go_south.clear();
    }

    // Go through the last row on the last floor
    int row = WIDTH - 1;
    for (int col = 0; col < LENGTH - 1; col++)
    {
        // Get room values
        room_value = get_val(row, col, floor);

        if(merged_room_sets.find(room_value) != merged_room_sets.end()){
            room_value = merged_room_sets[room_value];
        } else {
            merged_room_sets[room_value] = room_value;
        }

        // Get eastern room value
        east_room_value = (merged_room_sets.find(get_val(row, col+1, floor))!=merged_room_sets.end()) ?
                           merged_room_sets[get_val(row, col+1, floor)] : get_val(row, col+1, floor);

        // If this and eastern room is in different sets
        if(room_value != east_room_value)
        {
            // Make passage and merge sets
            passages.push_back(std::make_tuple(row, col, floor, row, col+1, floor));
            merged_room_sets[east_room_value] = room_value;
            set_val(row, col+1, floor, room_value);
        }
    }

    // Calculate the room values based on remaining walls (and floors & ceilings)
    Maze::calc_cell_values();
};

void Maze::calc_cell_values(){
    /* This function set each room value according to the remaining walls in that room.
     * The walls are represented as bits of a 8-bit int.
     * Floor          = 0000 0001
     * Eastern wall   = 0000 0010
     * Northern wall  = 0000 0100
     * Western wall   = 0000 1000
     * Southern wall  = 0001 0000
     * Ceiling        = 0010 0000
     */

    cell_data.clear();

    // 63 = 0011 1111 = all walls, floor, and ceiling
    cell_data.assign(LENGTH*WIDTH*HEIGHT, 63);

    int row_from, row_to,
        col_from, col_to,
        lvl_from, lvl_to;

    uint8_t cell_num_to;
    uint8_t cell_num_from;

    // Go through all passages
    for(auto pass : passages){
        std::tie(row_from, col_from, lvl_from,
            row_to,   col_to,   lvl_to   ) = pass;

        cell_num_from = get_val(row_from, col_from, lvl_from);
        cell_num_to   = get_val(row_to,   col_to,   lvl_to);

        // North to South passage
        if(row_from != row_to){
            set_val(row_from, col_from, lvl_from, cell_num_from & ~(SOUTH));
            set_val(row_to,   col_to,   lvl_to,   cell_num_to   & ~(NORTH));

        // West to East passage
        } else if (col_from != col_to){
            set_val(row_from, col_from, lvl_from, cell_num_from & ~(EAST));
            set_val(row_to,   col_to,   lvl_to,   cell_num_to   & ~(WEST));

        // Passage Up
        } else if(lvl_from != lvl_to){
            set_val(row_from, col_from, lvl_from, cell_num_from & ~(CEIL));
            set_val(row_to,   col_to,   lvl_to,   cell_num_to   & ~(FLOOR));
        }
        // Else the passage is in several directions and in this case discarded
    }
}

void Maze::print(){
    /*  Print the maze to std::cout
     *
     *  Prints floor in ascending order (top floor last)
     *  | - represent vertical and horizontal walls respectively
     *  + is the room corners
     *  U D B represent stairs up, down, and both, respectively
     *
     *  Maze::passages s used, so remember to run Maze::build first.
     */

    // Maze map canvas
    std::string map_drawing[2 * WIDTH + 1][HEIGHT];

    std::string wall_row = "+";
    std::string room_row = "|";

    // Make a row of walls and rooms
    for (int col = 0; col < LENGTH; col++){
        wall_row.append("-+");
        room_row.append(" |");
    }
    // Drawing initial map
    for (int floor = 0; floor < HEIGHT; floor++){
        map_drawing[2*WIDTH][floor] = wall_row;

        for (int row = 0; row < WIDTH; row++){
            map_drawing[2*row][floor] = wall_row;
            map_drawing[2*row + 1][floor] = room_row;
        }
    }

    // Making passages
    for ( auto pass : passages){
        int row_from, row_to,
            col_from, col_to,
            lvl_from, lvl_to;

        std::tie(row_from, col_from, lvl_from,
            row_to,   col_to,   lvl_to) = pass;

        // North - South passage
        if(row_from != row_to){
            map_drawing[2 * row_from + 2][lvl_from].replace(2 * col_from + 1, 1, " ");

        // West - East passage
        } else if (col_from != col_to){
            map_drawing[2*row_from + 1][lvl_from].replace(2 * col_from + 2, 1, " ");

        // Up - Down passage
        } else if (lvl_from != lvl_to){

            // If this room have a passage down, you can go up and down from it
            if (map_drawing[2 * row_from + 1][lvl_from].at(2 * col_from + 1) == 'D'){
                map_drawing[2 * row_from + 1][lvl_from].replace(2 * col_from + 1, 1, "B");
            } else {
                map_drawing[2 * row_from + 1][lvl_from].replace(2 * col_from + 1, 1, "U");
            }

            // The passages should come in order, so floor above have no passages up
            map_drawing[2 * row_to + 1][lvl_to].replace(2 * col_to + 1, 1, "D");
        }
    }

    // Printing the maze, one row at a time.
    for (int floor = 0; floor < HEIGHT; floor++){
        for (int row = 0; row < 2*WIDTH+1; row++){
            std::cout << map_drawing[row][floor] << std::endl;
        }
        std::cout << std::endl;
    }
}

Maze::~Maze()
{
    // No dynamic memory or pointers as of now
    // Destructor not needed
}
