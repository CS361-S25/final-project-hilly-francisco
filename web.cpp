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
emp::web::Div status_div{"status"};
emp::web::Div fitness_div{"fitness"};
MyConfigType config;

// // Create random generator and world
// emp::Random random_gen_2(5);
// OrgWorld world{random_gen_2};

class AEAnimator : public emp::web::Animate
{

    // arena width and height
    int num_h_boxes = 40;
    int num_w_boxes = 40;
    const double RECT_SIDE = 25;
    const double width{num_w_boxes * RECT_SIDE};
    const double height{num_h_boxes * RECT_SIDE};
    int sim_count = 0; // Counts number of steps we've taken

    emp::Random *random;
    OrgWorld *world;

    // const int num_of_KFC = 50;
    // const int num_of_Predator = 1;

    emp::web::Canvas canvas{width, height, "canvas"};

    bool prev_fit = false;
    double last_fit = 0;

public:
    AEAnimator()
    {
        auto specs = emp::ArgManager::make_builtin_specs(&config);
        emp::ArgManager am(emp::web::GetUrlParams(), specs);
        am.UseCallbacks();
        if (am.HasUnused())
            std::exit(EXIT_FAILURE);

        random = new emp::Random(config.SEED_NUM());
        world = new OrgWorld(*random);
        world->SetPopStruct_Grid(num_w_boxes, num_h_boxes);
        world->Resize(num_h_boxes, num_w_boxes);

        // shove canvas into the div
        // along with a control button
        doc << "<h1> Coevolution of predator and prey morphology and behavior </h1>";

        InsertText();
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

        sim_count++;

        // 1) Always clear first
        fitness_div.Clear();
        status_div.Clear();

        // 2) Update status every frame
        status_div << "<b>Simulation Count: </b>" << sim_count << "<br>";

        // 3) Sample & display fitness once every 500 steps
        if (sim_count > 0 && sim_count % 500 == 0) {
            double fit = world->getPredatorFitness();
            fitness_div << "<b>Predator Fitness of previous predator (for 500 steps): </b>" << fit;
            last_fit = fit;
            prev_fit = true;

            // then do your reset & repopulate
            world->step_counter = 0;
            world->Reset();
            world->SetPopStruct_Grid(num_w_boxes, num_h_boxes);
            world->Resize(num_h_boxes, num_w_boxes);
            CreateandAddKFC(*random, config.PREY_POP_SIZE());
            CreateandAddPredator(*random, 1);
        }
        else {
            // optional placeholder on other frames
            if (!prev_fit){
                fitness_div << " <b>Predator Fitness (last 500): — </b>";
            }

            else{
                fitness_div << "</b>Last Predator Fitness (last 500): </b>" << last_fit;
            }
        }

        // 4) Step & draw
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

                    if (species == "KFC") {
                        KFC *prey_ptr = dynamic_cast<KFC*>( world->GetPopulation()[org_num].Raw() );
                        float camo = prey_ptr->getCamouflageValue();     // [0,1]
                        float opacity = 1.0f - std::clamp(camo, 0.0f, 1.0f);

                        std::ostringstream ss;
                        ss << "rgba(0, 0, 255, " << opacity << ")";

                        canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE,
                                    ss.str(), "black");
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

        for (int spot : world->highlighted_cells)
        {
            // std::cout << "The spot is " << spot << std::endl;
            int x = spot / num_h_boxes; // integer division
            int y = spot % num_h_boxes;
            canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "rgba(0, 0, 0, 0.3)", "black");
        }

        for (int spot : world->attackRange_cells)
        {
            // std::cout << "The spot is " << spot << std::endl;
            int x = spot / num_h_boxes; // integer division
            int y = spot % num_h_boxes;
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
        CreateandAddKFC(*random, config.PREY_POP_SIZE());
        CreateandAddTestLowPredator();
    }

    /*
    Inputs: None
    Outputs: None
    Purpose: Utility function creating configuration panel for GUI
*/
    void SetConfigPanel()
    {
        emp::prefab::ConfigPanel config_panel(config);
        settings << config_panel;
        config_panel.SetRange("PRED_SIZE", "1", "10");
        config_panel.SetRange("SEED_NUM", "1", "10");
        config_panel.SetRange("PREY_POP_SIZE", "1", "200");
        config_panel.SetRange("GRID_WIDTH", "1", "200");
        config_panel.SetRange("PREY_BEHAVIOR", "0", "1");

        config_panel.SetRange("GRID_HEIGHT", "1", "200");

        settings.SetCSS("max-width", "700px");
        settings.SetCSS("padding", "10px");
        settings.SetCSS("margin", "10px");

        canvas.SetCSS("max-width", "100%");
        canvas.SetCSS("height", "auto");
        canvas.SetCSS("display", "block");
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
            KFC *KFC_org = new KFC(random, 400);
            KFC_org->setBehavior(config.PREY_BEHAVIOR());
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
            int randomWidthVision = 1 + 2 * ran.GetInt(1, 13);
            int randomHeightVision = ran.GetInt(1, 10);
            Predator *Predator_org = new Predator(random, 800, randomHeightVision, randomWidthVision);
            world->AddOrgAt(Predator_org, 410);
            // 152 for grid size of 20 x 20
        }
    }

    void CreateandAddTestPredator()
    {
        int randomWidthVision = 9;
        int randomHeightVision = 5;
        Predator *Predator_org = new Predator(random, 800, randomHeightVision, randomWidthVision);
        world->AddOrgAt(Predator_org, 410);
    };

    void CreateandAddTestLowPredator()
    {

        int randomWidthVision = 3;
        int randomHeightVision = 6;
        Predator *Predator_org = new Predator(random, 800, randomHeightVision, randomWidthVision);
        world->AddOrgAt(Predator_org, 410);
    };

    void InsertText()
    {
        doc << canvas
        << status_div;
        doc << fitness_div;
        doc << GetToggleButton("Toggle");
        doc << GetStepButton("Step");

        doc << "<h2> Simulation </h2>"
               "This Simulation attempts to replicate a research paper exploring the co-evolution of predator and prey organisms' behavior and morphology. "
               "Predators are given a set range of visible area they can see determined by given vision width and height. "
               "Prey in return move around the grid in two distinct styles, swarming and dispersal."
               " <br>";

        doc << "<h2> Results: </h2>";
        doc << "<strong> Predator Confusion Mechanism </strong>: A predator's liklihood of successfully hunting prey is determined by the number of prey they can see. The more prey in it's field of view, results in a lower successful attack chance. <br>"
               "<b> Broader vision suited for dispersal </b>: With the PCM in mind, broader predator vision results in dispersal prey behavior being best for prey survival.<br>"
               "<strong> Narrow vision suited for swarming </strong>: In turn, narrow predator vision gives leeway to swarming behavior among prey for better survivability. <br>"
               "<b> Fitness: </b> Fitness here, as in the original research paper, is the sum, over each of the 500 simulation steps, of the number of prey killed at that step. So we’re effectively adding up a handful of prey-per-step counts 500 times—hence totals in the tens of thousands."
               "<b> the higher the number, the better the hunting.</b>";

        doc << "<h2> Expansion (Camouflage): </h2>";
        doc << "<strong> Prey Camouflage </strong>: As an expansion to the paper, prey are given an extra mechanism. They can camouflage!<br>"
               "<strong> Explanation </strong>If a Prey is inside the predator's field of range, it has an inherent capability to possibly hide from the predator. A succesful hide results in it not being a candidate for hunting."
               "The more a prey hides from a predator, the better it's camouflaging ability gets! <br>"
               "<b> GUI Representation: </b> As prey get better at hiding, they get more transparent. It is easier to see if you step through! <br>";
    };
};

AEAnimator animator;

int main()
{
    animator.Step();
}