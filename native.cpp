#include <iostream>
#include <string>
#include <sys/_types/_size_t.h>

#include "Org.h"
#include "emp/base/vector.hpp"
#include "emp/math/random_utils.hpp"
#include "emp/math/Random.hpp"
#include "emp/config/ArgManager.hpp"

#include "World.h"

// This is the main function for the NATIVE version of this project.

EMP_BUILD_CONFIG(MyConfigType,
                 VALUE(SEED, int, 10, "What value should the random seed be?"),
                 VALUE(START_PROB, double, 0.5, "What cooperation probability value should the starting organism have?"),
                 VALUE(FILE_PATH, std::string, "", "Output file path"),
                 VALUE(FILE_NAME, std::string, "_data.dat", "Root output file name"))

int main(int argc, char *argv[])
{

    MyConfigType config;

    bool success = config.Read("MySettings.cfg");
    if (!success)
        config.Write("MySettings.cfg");

    auto args = emp::cl::ArgManager(argc, argv);
    if (args.ProcessConfigOptions(config, std::cout, "MySettings.cfg") == false)
    {
        std::cerr << "There was a problem in processing the options file." << std::endl;
        exit(1);
    }
    if (args.TestUnknown() == false)
    {
        std::cerr << "Leftover args no good." << std::endl;
        exit(1);
    }

    emp::Random random(config.SEED());
    OrgWorld world(random);

    for (size_t i = 0; i < 10; i++)
    {
    }

    world.Resize(10, 10);

    world.SetupOrgFile(config.FILE_PATH() + "Org_Vals" + std::to_string(config.SEED()) + config.FILE_NAME());

    for (int update = 0; update < 40; update++)
    {
        world.Update();
    }
}
