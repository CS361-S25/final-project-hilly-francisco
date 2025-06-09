#ifndef ConfigSetup_H
#define ConfigSetup_H

#include "emp/config/ArgManager.hpp"

EMP_BUILD_CONFIG(MyConfigType,
                 VALUE(SEED_NUM, int, 0, "What value should the random seed be? Default is 0."),
                 VALUE(GRID_WIDTH, int, 1, "What size should the grid's width be? Default is 300."),
                 VALUE(GRID_HEIGHT, int, 1, "What size should the grid's height be? Default is 300."),
                 VALUE(PREY_POP_SIZE, int, 100, "How many Prey Organsims should we start with?, Default is 100."),
                 VALUE(PRED_POP_SIZE, int, 1, "How many Predator Organsims should we start with? Default is 1."),
                 VALUE(FILE_PATH, std::string, "./data/", "Path to output files"),
                 VALUE(FILE_NAME, std::string, ".dat", "Output filename suffix"),
                 VALUE(PREY_BEHAVIOR, int, 0, "What should the Prey behavior be? 0 = False, 1 = True."))

#endif
