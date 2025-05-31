#include <iostream>
#include <string>
#include <sys/_types/_size_t.h>

#include "Org.h"
#include "emp/base/vector.hpp"
#include "emp/math/random_utils.hpp"
#include "emp/math/Random.hpp"
#include "emp/config/ArgManager.hpp"

#include "World.h"
#include "KFC.h"
#include "Predator.h"

// This is the main function for the NATIVE version of this project.

EMP_BUILD_CONFIG(MyConfigType,
                 VALUE(SEED, int, 10, "What value should the random seed be?"),
                 VALUE(VISIBILITY, int, 5, "What visibiilyt should the starting organism have?"),
                 VALUE(PREYCOUNT, int, 30, "How many starting prey should there be?"),
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

    // Build the simulations

    for (size_t i = 0; i < 5; i++)
    {
        emp::Random random(5);
        OrgWorld world{random};
        world.Resize(10, 10);

        // Set PreyCount and Visibility
        int PreyCount = config.PREYCOUNT();
        int Visibilty = config.VISIBILITY();
        int VisionHeight = 4;
        int VisionWidth = 1;

        // Inject KFC Organisms (Can I use the same random variable here?)
        for (int x = 0; x < PreyCount; x++)
        {
            KFC *KFC_org = new KFC(&random, 400);
            world.Inject(*KFC_org);
        }

        // Inject Predator Organisms
        Predator *Predator_org = new Predator(&random, 800, VisionHeight, VisionWidth);
        world.Inject(*Predator_org);

        // Do 100 updates
        for (int update = 0; update < 100; update++)
        {
            world.Update();
        }

        // Make the Data File
        world.SetupOrgFile(config.FILE_PATH() + "Org_Vals" + std::to_string(config.SEED()) + config.FILE_NAME());

        // Before the next update, increase vision
        VisionHeight = +1;
        VisionWidth = +2;
    }
}
