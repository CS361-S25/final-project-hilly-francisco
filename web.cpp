#define UIT_VENDORIZE_EMP
#define UIT_SUPPRESS_MACRO_INSEEP_WARNINGS

//Your web code goes here
#include "emp/math/Random.hpp"
#include "emp/web/Animate.hpp"
#include "emp/web/web.hpp"
#include "World.h"
#include "Org.h"
#include "Grass.h"
#include "Deer.h"
#include "Wolf.h"

emp::web::Document doc{"target"};

// Create random generator and world
emp::Random random_gen_2(5);
OrgWorld world{random_gen_2};

class AEAnimator : public emp::web::Animate {

    // arena width and height
    const int num_h_boxes = 25;
    const int num_w_boxes = 25;
    const double RECT_SIDE = 20;
    const double width{num_w_boxes * RECT_SIDE};
    const double height{num_h_boxes * RECT_SIDE};

    const int num_of_grass = 80;
    const int num_of_deer = 12;
    const int num_of_wolf = 4;

    emp::web::Canvas canvas{width, height, "canvas"};

    public:

    AEAnimator() {
        // shove canvas into the div
        // along with a control button
        doc << "<h1> Artifical Ecology </h1>";
        doc << canvas;
        doc << GetToggleButton("Toggle");
        doc << GetStepButton("Step");
        doc << "<h2> Species: </h2>";
        doc << "<strong>Grass (Green) </strong>: Represented by the green boxes, the Grass serve as the primary producers in this simulation. <br>" 
                "<b> Deer (Blue) </b>: Represented by the blue boxes, the deer serve as the primary consumers.<br>"
                "<strong> Wolf (Red) </strong>: Represented by the red boxes, the wolves keep the deer in check and are this simulation's apex predator."
                
                "<h2> Interactions </h2>"
                "This Simulation displays varying relationships and interactions between the three species. For one, there is a basic predation interaction"
                "between deer and grass, and wolves and deers. The deer graze on the grass, while attempting to escape the hungry wolves."
                "Between the wolves and grass, there is sort of a mutualistic realtionship. When the wolves eat deer, the grass population flourishes."
                "In return, more grass draws in a bigger deer population benefitting the wolves.";


        world.Resize(num_w_boxes, num_h_boxes);
        world.SetPopStruct_Grid(num_w_boxes, num_h_boxes);
        AddOrgs();

        DrawSquares();

    }

    /*
        Input: Void
        Output: Void
        Purpose: Initiates a step through the simulation
    */
    void DoFrame() override {
        canvas.Clear();
        world.Update();
        DrawSquares();
    }

    /*
        Input: Void
        Output: Void
        Purpose: Iterates through grid and draws squares for Grass, Deer, Wolf, and NULL organisms
    */
    void DrawSquares() {
        int org_num = 0;
        for (int x = 0; x < num_w_boxes; x++){
            for (int y = 0; y < num_h_boxes; y++) {
                if (world.IsOccupied(org_num)) {
                    std::string species = world[org_num].SpeciesName();
                    if (species == "Grass"){
                        canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "green", "black");
                    }
                    if (species == "Deer"){
                        canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "blue", "black");
                    }
                    if (species == "Wolf"){
                        canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "red", "black");
                    }
                } else {
                    canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "white", "black");
                }
                org_num++;
            }
        }
    }

    /*
        Input: Void
        Output: Void
        Purpose: Adds previously defined number of organisms for each type
    */
    void AddOrgs() {
        CreateandAddGrass(random_gen_2, num_of_grass);
        CreateandAddDeer(random_gen_2, num_of_deer);
        CreateandAddWolf(random_gen_2, num_of_wolf);
    }

    /*
        Input: Void
        Output: Void
        Purpose: Creates and adds a grass type organism to world
    */
    void CreateandAddGrass(emp::Random &ran, int num) {
        for (int i = 0; i < num; i++){
            Grass* grass_org = new Grass(&random_gen_2);
            world.AddOrgAt(grass_org, ran.GetInt(0, world.GetSize()));
        }
    }

    /*
        Input: Void
        Output: Void
        Purpose: Creates and adds a deer type organism to world
    */
    void CreateandAddDeer(emp::Random &ran, int num) {
        for (int i = 0; i < num; i++){
            Deer* deer_org = new Deer(&random_gen_2, 400);
            world.AddOrgAt(deer_org, ran.GetInt(0, world.GetSize()));
        }
    }

    /*
        Input: Void
        Output: Void
        Purpose: Creates and adds a wolf type organism to world
    */
    void CreateandAddWolf(emp::Random &ran, int num) {
        for (int i = 0; i < num; i++){
            Wolf* wolf_org = new Wolf(&random_gen_2, 800);
            world.AddOrgAt(wolf_org, ran.GetInt(0, world.GetSize()));
        }
    }

};



AEAnimator animator;

int main() {
    animator.Step();
}