#include <iostream>
#include <string>
#include <sys/_types/_size_t.h>

#include "Org.h"
#include "emp/base/vector.hpp"
#include "emp/math/random_utils.hpp"
#include "emp/math/Random.hpp"
#include "emp/config/ArgManager.hpp"

#include "ConfigSetup.h"
#include "World.h"
#include "KFC.h"
#include "Predator.h"

// Instance variables for OrgWorld
emp::Random *random_ptr;
OrgWorld *world;
MyConfigType config;

// This is the main function for the NATIVE version of this project.
void CreateandAddKFC(emp::Random &ran, int num)
{
    for (int i = 0; i < num; i++)
    {
        KFC *KFC_org = new KFC(&ran, 400);
        KFC_org->setBehavior(0);
        world->AddOrgAt(KFC_org, ran.GetInt(0, world->GetSize()));
    }
}

void CreateandAddPredator(emp::Random &ran, int num)
{
    for (int i = 0; i < num; i++)
    {
        int randomWidthVision = 1 + 2 * ran.GetInt(1, 13);
        int randomHeightVision = ran.GetInt(1, 10);
        Predator *Predator_org = new Predator(&ran, 800, randomHeightVision, randomWidthVision);
        world->AddOrgAt(Predator_org, 410);
        // 152 for grid size of 20 x 20
        int vision_size = randomWidthVision * randomHeightVision;
        std::cout << "Vision Size: " << vision_size << std::endl; // This isn't the actual vision because of triagnles
    }
}

int CalculateFitness()
{
    // Steps/number of prey eaten
}

int main(int argc, char *argv[])
{
    // Should I add config here?

    int num_h_boxes = 40;
    int num_w_boxes = 40;
    const double RECT_SIDE = 25; // I think I dont need this
    int sim_count = 0;           // Counts number of steps we've taken

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

    // Give values to your Orgworld
    random_ptr = new emp::Random(config.SEED_NUM());
    world = new OrgWorld(*random_ptr);
    world->Resize(num_h_boxes, num_w_boxes);

    world->SetupOrgFile(config.FILE_PATH() + "Org_Vals" + std::to_string(config.SEED_NUM()) + config.FILE_NAME());

    // Run the simulation
    world->SetPopStruct_Grid(num_w_boxes, num_h_boxes);
    world->Resize(num_h_boxes, num_w_boxes);

    CreateandAddKFC(*random_ptr, config.PREY_POP_SIZE());
    CreateandAddPredator(*random_ptr, config.PRED_POP_SIZE());
    for (int update = 0; update < 155; update++)
    {
        world->Update();
    }

    world->Reset();
}
