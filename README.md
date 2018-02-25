
                  3D maze   
                 
This class generates a 3D maze and contain descriptions of it.
The algorithm is flawed, in that it partitions connected rooms into sets, then chooses one stair up to the next floor pr. set.
This means that if you want to go somewhere on the same floor, but can't find a way, the answer is always to be found on the floors above, but never below.

This can be avoided by properly mapping sets all the way through, and will be done when time allows.

Usage:  

//Create a maze by   
Maze some_maze(columns, rows, floors, horizontal_bias, vertical_bias);  
  
// Build the maze  
some_maze.build();     

// Print the maze  
some_maze.print();  

Example print() output for a 3x3x3 maze:

```
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
```
