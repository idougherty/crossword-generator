// board.cpp
#define OPEN_TILE '_'
#define CLOSED_TILE '#'

#define GETROW(b, idx) (idx) / b->size
#define GETCOL(b, idx) (idx) % b->size
#define GETIDX(b, row, col) ((row) * b->size) + (col)
#define GETVAL(b, row, col) b->data[GETIDX(b, row, col)]

// board_generator.cpp
#define MIN_LENGTH 3
#define MAX_CHAINED 6
// Most crosswords have at least 1/6 squares blocked
#define CLOSED_TILE_RATIO 1 / 6.0

// solver.cpp
#define LOOKAHEAD 20