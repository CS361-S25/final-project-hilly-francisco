#define UIT_VENDORIZE_EMP
#define UIT_SUPPRESS_MACRO_INSEEP_WARNINGS

// Your web code goes here
#include "emp/math/Random.hpp"
#include "emp/web/Animate.hpp"
#include "emp/web/web.hpp"
#include "World.h"
#include "Org.h"
#include "KFC.h"
#include "Predator.h"
#include "ConfigSetup.h"

#include "emp/config/ArgManager.hpp"
#include "emp/prefab/ConfigPanel.hpp"
#include "emp/web/UrlParams.hpp"

emp::web::Document doc{"target"};
emp::web::Document settings("settings");
MyConfigType config;

// Create random generator and world
emp::Random random_gen_2(5);
OrgWorld world{random_gen_2};

class AEAnimator : public emp::web::Animate
{

    // arena width and height
    int num_h_boxes = 20;
    int num_w_boxes = 20;
    const double RECT_SIDE = 25;
    const double width{num_w_boxes * RECT_SIDE};
    const double height{num_h_boxes * RECT_SIDE};

    emp::Random  *random;
    OrgWorld *world;

    // const int num_of_KFC = 50;
    // const int num_of_Predator = 1;

    emp::web::Canvas canvas{width, height, "canvas"};

public:
    AEAnimator()
    {
        auto specs = emp::ArgManager::make_builtin_specs(&config);
        emp::ArgManager am(emp::web::GetUrlParams(), specs);
        am.UseCallbacks();
        if (am.HasUnused()) std::exit(EXIT_FAILURE);

        random = new emp::Random(config.SEED_NUM());
        world = new OrgWorld(*random);
        world->SetPopStruct_Grid(num_w_boxes, num_h_boxes);
        world->Resize(num_h_boxes, num_w_boxes);


        // shove canvas into the div
        // along with a control button
        doc << "<h1> Coevolution of predator and prey morphology and behavior </h1>";
        doc << canvas;
        doc << GetToggleButton("Toggle");
        doc << GetStepButton("Step");
        SetConfigPanel();
        AddOrgs();

        DrawSquares();
    }

    /*
        Input: Void
        Output: Void
        Purpose: Initiates a step through the simulation
    */
    void DoFrame() override
    {
        canvas.Clear();
        world->Update();
        DrawSquares();
    }

    /*
        Input: Void
        Output: Void
        Purpose: Iterates through grid and draws squares for KFC, Predator, and NULL organisms
    */
    void DrawSquares()
    {
        int org_num = 0;
        for (int x = 0; x < num_w_boxes; x++)
        {
            for (int y = 0; y < num_h_boxes; y++)
            {
                if (world->IsOccupied(org_num))
                {
                    std::string species = world->GetPopulation()[org_num]->SpeciesName();
                    if (species == "KFC")
                    {
                        canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "blue", "black");
                    }
                    if (species == "Predator")
                    {
                        canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "red", "black");
                    }
                }
                else
                {
                    canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "white", "black");
                }
                org_num++;
            }
        }


        for (int spot : world->highlighted_cells){
            //std::cout << "The spot is " << spot << std::endl;
            int x = spot / num_h_boxes;   // integer division
            int y = spot % num_h_boxes;
            //canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "black", "black");
            canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "rgba(0, 0, 0, 0.3)", "black");
        }

        for (int spot : world->attackRange_cells){
            //std::cout << "The spot is " << spot << std::endl;
            int x = spot / num_h_boxes;   // integer division
            int y = spot % num_h_boxes;
            //canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "yellow", "black");
            canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "rgba(255, 255, 0, 0.3)", "black");
        }

    }

    /*
        Input: Void
        Output: Void
        Purpose: Adds previously defined number of organisms for each type
    */
    void AddOrgs()
    {
        CreateandAddKFC(random_gen_2, config.PREY_SIZE());
        //CreateandAddPredator(random_gen_2, num_of_Predator);
        //CreateandAddTestPredator();
        CreateandAddTestLowPredator();
    }

        /*
        Inputs: None
        Outputs: None
        Purpose: Utility function creating configuration panel for GUI
    */
    void SetConfigPanel(){
        emp::prefab::ConfigPanel config_panel(config);
        settings << config_panel;
        config_panel.SetRange("PRED_SIZE", "1", "10");
        config_panel.SetRange("SEED_NUM", "1", "10");
        config_panel.SetRange("PREY_SIZE", "1", "50");
        config_panel.SetRange("GRID_WIDTH", "1", "200");
        config_panel.SetRange("GRID_HEIGHT", "1", "200");

        settings.SetCSS("max-width", "500px");
    }

    /*
        Input: Void
        Output: Void
        Purpose: Creates and adds a KFC type organism to world
    */
    void CreateandAddKFC(emp::Random &ran, int num)
    {
        for (int i = 0; i < num; i++)
        {
            KFC *KFC_org = new KFC(&random_gen_2, 400);
            world->AddOrgAt(KFC_org, ran.GetInt(0, world->GetSize()));
        }
    }

    /*
        Input: Void
        Output: Void
        Purpose: Creates and adds a Predator type organism to world with random vision values
    */
    void CreateandAddPredator(emp::Random &ran, int num)
    {
        for (int i = 0; i < num; i++)
        {
            int randomWidthVision = 1 + 2 * ran.GetInt(1, 5);
            int randomHeightVision = ran.GetInt(1, 5);
            Predator *Predator_org = new Predator(&random_gen_2, 800, randomHeightVision, randomWidthVision);
            world->AddOrgAt(Predator_org, ran.GetInt(0, world->GetSize()));
        }
    }

    void CreateandAddTestPredator() {
            int randomWidthVision = 7;
            int randomHeightVision = 4;
            Predator *Predator_org = new Predator(&random_gen_2, 800, randomHeightVision, randomWidthVision);
            world->AddOrgAt(Predator_org, 100);
    };

    void CreateandAddTestLowPredator() {
            
            int randomWidthVision = 9;
            int randomHeightVision = 5;
            Predator *Predator_org = new Predator(&random_gen_2, 800, randomHeightVision, randomWidthVision);
            world->AddOrgAt(Predator_org, 152);
    };
};

AEAnimator animator;

int main()
{
    animator.Step();
}