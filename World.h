#ifndef WORLD_H
#define WORLD_H

#include "emp/Evolve/World.hpp"
#include "emp/math/random_utils.hpp"
#include "emp/math/Random.hpp"
#include "emp/data/DataFile.hpp"
#include <set>

#include "Org.h"
#include "Predator.h"

class OrgWorld : public emp::World<Organism>
{

    emp::Random &random;
    emp::Ptr<emp::Random> random_ptr;

    // Data Variables
    emp::Ptr<emp::DataMonitor<double, emp::data::Histogram>> data_node_visibility;
    emp::Ptr<emp::DataMonitor<int>> data_node_count;

public:
    int grid_w_boxes = 10;
    int grid_h_boxes = 10;

    // Store all visible spots for all predators in the current update
    std::set<size_t> highlighted_cells;

    OrgWorld(emp::Random &_random) : emp::World<Organism>(_random), random(_random)
    {
        random_ptr.New(_random);
    }

    ~OrgWorld()
    {
    }

    void Update()
    {

        double pointsPerUpdate = 0;
        highlighted_cells.clear();

        emp::World<Organism>::Update();

        emp::vector<size_t> schedule1 = emp::GetPermutation(random, GetSize());
        emp::vector<size_t> schedule2 = emp::GetPermutation(random, GetSize());

        // Calls the process function for each organism
        CallProcesses(schedule1, pointsPerUpdate);

        // Method for vision check, maybe be in org, but we'll see
        for (int cellSpot : schedule1)
        {

            // Working only with cells that have an organism
            if (IsOccupied(cellSpot))
            {
                // Call function to check cells in hardcoded areas
                if (pop[cellSpot]->SpeciesName() == "Predator")
                {
                    checkVisibleArea(cellSpot);
                }
            }
        }

        // Checks conditions for reproduction and lets organisms move
        for (int i : schedule2)
        {
            if (IsOccupied(i))
            {
                if (pop[i]->SpeciesName() != "Predator")
                {
                    // std::cout << "Called: " << pop[i]->SpeciesName() << std::endl;
                    emp::Ptr<Organism> offspring = pop[i]->CheckReproduction();

                    // If offspring is made, place into non-empty box
                    if (offspring)
                    {
                        PlaceOffspring(offspring, i);
                    }
                }

                // Move non-grass organisms to random neighboring position, if occupied check if can be eaten
                if (pop[i]->SpeciesName() != "Predator")
                {
                    MoveOrg(i);
                }
            }
        }
    }

    /*
        Input: Int organism position
        Output: emp::Ptr<Organism>
        Purpose: Removes organism from initial position and returns a pointer to the extracted org
    */
    emp::Ptr<Organism> ExtractOrganism(int orgPos)
    {
        emp::Ptr<Organism> extracted_org = pop[orgPos];
        pop[orgPos] = nullptr;
        return extracted_org;
    }

    /*
        Input: Int organism position
        Output: Void
        Purpose: Removes organism from initial position and deletes it
    */
    void DeleteOrganism(int orgPos)
    {
        emp::Ptr<Organism> extracted_org = pop[orgPos];
        pop[orgPos] = nullptr;
        delete (extracted_org);
    }

    /*
        Input: emp::Ptr<Organism> and Integer
        Output: Bool
        Purpose: Calls given organism's Species Eat function to determine if organisms at given position
                 is available for consumption by given organism pointer
    */
    bool EatSpecies(emp::Ptr<Organism> given_org, int diff_org_position)
    {
        // Check if org can eat species at occupied area
        if (given_org->SpeciesEat(pop[diff_org_position]))
        {
            DeleteOrganism(diff_org_position);
            Predator *pred_ptr = dynamic_cast<Predator *>(given_org.Raw());
            pred_ptr->increasePreyConsumed();
            given_org->hasEaten = true; // What is this for?
            // Data monitor: predator succuess

            // AddOrgAt(given_org, diff_org_position);
            return true;
            // Else data 0
            // get sucess rate
        }
        return false;
    }

    /*
        Input: emp::vector<size_t> and Integer
        Output: Void
        Purpose: Calls process function for each organism in the given vector, and assigns them
                 the issued number of points passed into function
    */
    void CallProcesses(emp::vector<size_t> schedule, int points)
    {
        for (int i : schedule)
        {
            if (IsOccupied(i))
            {
                pop[i]->Process(points);
                pop[i]->hasEaten = false;

                if (pop[i]->CheckShouldOrgDie())
                {
                    DeleteOrganism(i);
                }
            }
        }
    }

    /*
        Input: emp::Ptr<Organism> and Integer
        Output: Void
        Purpose: Places given offspring organism into random neighbouring spot
                 from parent's position
    */
    void PlaceOffspring(emp::Ptr<Organism> offspring, int parent_pos)
    {
        emp::WorldPosition birth_pos = GetRandomNeighborPos(parent_pos);
        if (!IsOccupied(birth_pos))
        {
            AddOrgAt(offspring, birth_pos.GetIndex());
        }
        else
        {
            delete offspring;
        }
    }

    /*
        Input: Integer
        Output: Void
        Purpose: Moves organism at specified position into available neighbouring location
                 Checks if new position is empty, remains in place if not.
    */
    void MoveOrg(int pos)
    {
        emp::WorldPosition newPosition = GetRandomNeighborPos(pos);
        emp::Ptr<Organism> extracted_org = ExtractOrganism(pos);

        if (!IsOccupied(newPosition))
        {
            AddOrgAt(extracted_org, newPosition);
        }

        else
        {
            bool wasEaten = EatSpecies(extracted_org, newPosition.GetIndex());

            if (!wasEaten)
            {
                AddOrgAt(extracted_org, pos);
            }
        }
    }

    /*
        Input: Integer
        Output: Void
        Purpose: Checks an organisms visible area in front of it (Only N rn)
    */
    void checkVisibleArea(int cellSpot)
    {
        emp::Ptr<Organism> org_ptr = pop[cellSpot];

        // Try casting to Predator
        Predator *pred_ptr = dynamic_cast<Predator *>(org_ptr.Raw());

        int heightOfVision = pred_ptr->getHeightOfVision();
        int widthOfVision = pred_ptr->getWidthOfVision();

        emp::vector<size_t> visibleSpots;

        std::cout << "Height is: " << heightOfVision << " Width is: " << widthOfVision << std::endl;

        // Dumb Math
        int numberOfSpots = (heightOfVision * widthOfVision) - heightOfVision * (heightOfVision - 1);

        // std::cout << "cell spot " << ": " << cellSpot << std::endl;

        for (int h = 0; h < heightOfVision; h++)
        {
            // How many cells to left/right at this cur depth
            int curWidth = std::max(1, widthOfVision - 2 * h);
            int half = curWidth / 2;

            // go north by h rows
            int basePos = cellSpot - ((h + 1) * grid_w_boxes);

            for (int leftAndRightCount = -half; leftAndRightCount <= half; leftAndRightCount++)
            {
                int target = basePos + leftAndRightCount;

                // Toroidal wrap function call to ensure...well toroidal wrapping
                target = getToroidalBound(target);

                visibleSpots.push_back(target);
                highlighted_cells.insert(target); // <-- Add to the set for GUI

                // std::cout << "Visible spot at depth " << h << ": " << target << std::endl;
            }
        }
        int visibility = getVisibleOrganismCount(visibleSpots);
        // Save visibility to a data node
        // data_node_visibility->AddDatum(visibility);
        Attack(visibleSpots, pop[cellSpot]);
        return;
    }

    /*
        Input: Vector of integers/size_t
        Output: Int
        Purpose: Returns the number of visible organism in specified visible area
    */
    int getVisibleOrganismCount(emp::vector<size_t> visibleSpots)
    {
        int countOfOrgs = 0;
        for (int spot : visibleSpots)
        {
            // std::cout << "spot: " << spot << std::endl;
            if (IsOccupied(spot))
            {
                countOfOrgs++;
            }
        }
        std::cout << "Count of organism visible: " << countOfOrgs << std::endl;
        return countOfOrgs;
    }

    void Attack(const emp::vector<size_t> &visibleSpots, emp::Ptr<Organism> organism)
    {
        emp::vector<size_t> targets;
        for (size_t spot : visibleSpots)
        {
            if (IsOccupied(spot))
            {
                targets.push_back(spot);
            }
        }

        if (!targets.empty())
        {
            float attackChance = getAttackChance(targets.size());
            if (random.GetDouble() < attackChance)
            {
                int chosen = random.GetInt(targets.size());
                std::cout << "Deleting organism at: " << targets[chosen] << std::endl;
                EatSpecies(organism, targets[chosen]);
            }
            else
            {
                // Attack failed due to probability check
                std::cout << "Attack failed due to probability." << std::endl;
            }
        }
        else
        {
            // No target available to attack
        }
    }

    float getAttackChance(int visibleTargets)
    {
        if (visibleTargets <= 0)
            return 0.0f;
        int chanceOfAttack = 1.0f / visibleTargets;

        return chanceOfAttack;
    }

    int getToroidalBound(int cellCheck)
    {
        int x = cellCheck % grid_w_boxes;
        int y = cellCheck / grid_w_boxes;

        // wrap x
        if (x < 0)
            x += grid_w_boxes;
        else if (x >= grid_w_boxes)
            x -= grid_w_boxes;
        // wrap y
        if (y < 0)
            y += grid_h_boxes;
        else if (y >= grid_h_boxes)
            y -= grid_h_boxes;

        return y * grid_w_boxes + x;
    }

    int GetToroidalDistance(int idx1, int idx2)
    {
        int x1 = idx1 % grid_w_boxes;
        int y1 = idx1 / grid_w_boxes;
        int x2 = idx2 % grid_w_boxes;
        int y2 = idx2 / grid_w_boxes;

        int dx = std::abs(x1 - x2);
        int dy = std::abs(y1 - y2);

        // Wrap distances
        dx = std::min(dx, grid_w_boxes - dx);
        dy = std::min(dy, grid_h_boxes - dy);

        return dx + dy; // Manhattan distance
    }

    // Everything for gathering data
    emp::DataMonitor<int> &GetOrgCountDataNode()
    {
        if (!data_node_visibility)
        {
            data_node_visibility.New();
            OnUpdate([this](size_t)
                     {
    data_node_visibility -> Reset();
    for (size_t i = 0; i < pop.size(); i++)
        if(IsOccupied(i))
        data_node_count->AddDatum((1)); });
        }
        return *data_node_count;
    }

    emp::DataMonitor<double, emp::data::Histogram> &GetOrgVisibilityDataNode()
    {
        if (!data_node_visibility)
        {
            data_node_visibility.New();
            OnUpdate([this](size_t)
                     {
          data_node_visibility->Reset();
          for (size_t i = 0; i< pop.size(); i++)
          if (IsOccupied(i))
              data_node_visibility->AddDatum(0); });
        }
        data_node_visibility->SetupBins(0.0, 1.1, 11);
        return *data_node_visibility;
    }

    emp::DataFile &SetupOrgFile(const std::string &filename)
    {
        auto &file = SetupFile(filename);
        auto &node1 = GetOrgCountDataNode();
        auto &node = GetOrgVisibilityDataNode();
        file.AddVar(update, "update", "Update");
        file.AddTotal(node1, "count", "Total number of organisms");
        file.AddHistBin(node, 0, "visibiliyt", "Count for histogram bin visibility");

        file.PrintHeaderKeys();

        return file;
    }
};
#endif