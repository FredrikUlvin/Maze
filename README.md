
                  3D maze 
                 
This is a class describing a 3D maze
Usage:

//Create a maze by 
Maze some_maze(columns, rows, floors, horizontal_bias, vertical_bias);
  
// Build the maze
some_maze.build();

// Print the maze
some_maze.print();

Example print() output for a 3x3x3 maze:
+-+-+-+
| |U|U|
+ +-+ +
|U| | |
+ + +-+
|   |U|
+-+-+-+

+-+-+-+
| |D|B|
+ + + +
|B  | |
+-+-+ +
|U  |D|
+-+-+-+

+-+-+-+
|   |D|
+ +-+ +
|D    |
+ + +-+
|D|   |
+-+-+-+
