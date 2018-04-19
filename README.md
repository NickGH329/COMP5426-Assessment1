# Assignment 1
The **Red/Blue computation** simulates two interactive flows: an n by n board is initialized so cells have one of three colors: red, white, and blue, where white is empty, red moves right, and blue moves down. (The board may be initialized with roughly 1/3 cells in read, 1/3 in white and 1/3 in blue and colors should be interleaved and spread across the board. You need to write a separate function board_init to initialize the board.) In the first half step of an iteration, any red color can move right one cell if the cell to the right is unoccupied (white); on the second half step, any blue color can move down one cell if the cell below it is unoccupied (white); the case where red vacates a cell (first half) and blue moves into it (second half) is okay. Note: any red or blue color can only move at most one cell in one iteration. Colors wraparound to the opposite side when reaching the edge. Viewing the board as perfectly overlaid with a t by t grid of tiles (i.e., t divides n, and every tile contains n/t by n/t cells), the computation terminates if any tile’s colored cells are more than c% one color (blue or red).

* Use MPI to write a solution to the Red/Blue computation.
* Assume the processes are organized as a one-dimensional linear array.
* Each process will hold a number of k rows of tiles, or k×n/t rows of cells for k ≥ 0.
* For the purpose of load balancing, processes should have roughly the same number of rows and the difference not greater than one tile row, or n/t cell rows.
* Your program must produce correct results for nprocs being greater than or equal to one.
* Your program needs to ask for 4 user defined parameters (integers) as inputs: cell grid size n, tile grid size t, terminating threshold c, and maximum number of iterations max_iters.
* Your program needs to print out which tile (or tiles if more than one) has the colored squares more than c% one color (blue or red).
* After the parallel computation, you main program must conduct a self-checking, i.e., first perform a sequential computation using the same data set and then compare the two results.

