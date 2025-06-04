#ifndef ConfigSetup_H
#define ConfigSetup_H

#include "emp/config/ArgManager.hpp"

EMP_BUILD_CONFIG(MyConfigType,
    VALUE(SEED_NUM, int, 0, "What value should the random seed be?"),
    VALUE(GRID_WIDTH, int, 1, "What size should the grid's width be?"),
    VALUE(GRID_HEIGHT, int, 1, "What size should the grid's height be?"),
    VALUE(PREY_SIZE, int, 100, "How many Prey Organsims should we start with?"),
    VALUE(PRED_SIZE, int, 1, "How many Predator Organsims should we start with?"),
    )

#endif
