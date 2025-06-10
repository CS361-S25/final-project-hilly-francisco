#ifndef ConfigSetup_H
#define ConfigSetup_H

#include "emp/config/ArgManager.hpp"

EMP_BUILD_CONFIG(MyConfigType,
                 VALUE(SEED_NUM, int, 0, "What value should the random seed be? Default is 0."),\
                 VALUE(PREY_POP_SIZE, int, 100, "How many Prey Organsims should we start with?, Default is 100."),
                 VALUE(PREY_BEHAVIOR, int, 0, "What should the Prey behavior be? 0 = Disperal, 1 = Swarming."))

#endif
