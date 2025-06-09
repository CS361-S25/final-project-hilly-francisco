#ifndef WORLD_H
#define WORLD_H

#include "emp/Evolve/World.hpp"
#include "emp/math/random_utils.hpp"
#include "emp/math/Random.hpp"
#include "emp/data/DataFile.hpp"
#include <set>

#include "Org.h"
#include "Predator.h"
#include "KFC.h"

class OrgWorld : public emp::World<Organism>
{

    emp::Random &random;
    emp::Ptr<emp::Random> random_ptr;

    // Data Variables -- Not sure what to do with these for now
    emp::Ptr<emp::DataMonitor<int>> data_prey_consumed;
    emp::Ptr<emp::DataMonitor<int>> data_prey_seen;
    emp::Ptr<emp::DataMonitor<int>> data_vision_size;
    emp::Ptr<emp::DataMonitor<int>> data_vision_width;
    emp::Ptr<emp::DataMonitor<int>> data_vision_height;
    int total_prey_seen;
    int total_prey_consumed;
    int vision_size;
    int vision_width;
    int vision_height;

public:
    int grid_w_boxes = 40;
    int grid_h_boxes = 40;
    float camouflage_value = 0.1;
    int total_prey_start = 100;
    int step_counter = 0;



    // grab org at population index
    const pop_t &GetPopulation() { return pop; }

    // Store all visible spots for all predators in the current update
    std::set<size_t> highlighted_cells; // The amount of cells the predator
    std::set<size_t> attackRange_cells; // Default attack range for predator

    OrgWorld(emp::Random &_random) : emp::World<Organism>(_random), random(_random)
    {
        random_ptr.New(_random);
    }

    ~OrgWorld() {}

    void Update() 
    {
        step_counter++;

        double pointsPerUpdate = 0;

        // Clear recorded cells for new update
        highlighted_cells.clear();
        attackRange_cells.clear();

        emp::World<Organism>::Update();

        // Grab orgs in randomized ordering
        emp::vector<size_t> schedule1 = emp::GetPermutation(random, GetSize());
        emp::vector<size_t> schedule2 = emp::GetPermutation(random, GetSize());

        // Calls the process function for each organism
        CallProcesses(schedule1, pointsPerUpdate);

        // Handle each organism according to species type
        for (int cellSpot : schedule2)
        {
            if (!IsOccupied(cellSpot))
            {
                continue;
            }

            const auto name = pop[cellSpot]->SpeciesName();

            if (name == "Predator")
            {
                HandlePredator(cellSpot);
            }

            else if (name == "KFC")
            {
                HandlePrey(cellSpot);
            }
        }

        // Calc fitness for each organism
        int alive_prey = getCountAlivePrey();

        for (int cellSpot : schedule1)
        {
            if (!IsOccupied(cellSpot))
            {
                continue;
            }

            const auto name = pop[cellSpot]->SpeciesName();

            if (name == "Predator")
            {
                emp::Ptr<Organism> org_ptr = pop[cellSpot];
                Predator *pred_ptr = dynamic_cast<Predator *>(org_ptr.Raw());

                pred_ptr->AddToFitness(total_prey_start - alive_prey);
            }

            else if (name == "KFC")
            {
                emp::Ptr<Organism> org_ptr = pop[cellSpot];
                KFC *prey_ptr = dynamic_cast<KFC *>(org_ptr.Raw());

                prey_ptr->AddToFitness(alive_prey);
            }
        }
    }

    void HandlePredator(int cellSpot)
    {
        // Attack in visible area
        checkPredVisibleArea(cellSpot);

        // Move predator
        emp::vector<size_t> visibleSpots;

        // Get predator pointer to call predator methods
        Predator *pred_ptr = dynamic_cast<Predator *>(pop[cellSpot].Raw());
        visibleSpots = getVisibleArea(cellSpot, {}, pred_ptr->getHeightOfVision(), pred_ptr->getWidthOfVision());
        vision_size = visibleSpots.size();
        
        // Find prey if any
        int closest = FindClosestPrey(cellSpot, visibleSpots);

        if (closest != -1)
        {
            MovePredatorTowards(cellSpot, closest);
        }

        // If no prey found, just move randomly
        else
        {
            auto org = ExtractOrganism(cellSpot);
            auto dest = GetRandomNeighborPos(cellSpot).GetIndex();
            AddOrgAt(org, dest);
        }
    }

    void HandlePrey(int cellSpot)
    {
        // Get prey pointer to call prey methods
        emp::Ptr<Organism> org_ptr = pop[cellSpot];
        KFC *prey_ptr = dynamic_cast<KFC *>(org_ptr.Raw());

        // Collect neighbors
        auto nbrs = GetNeighborIndices(cellSpot);
        emp::vector<size_t> open_spots;
        for (size_t n : nbrs)
            if (!IsOccupied(n))
                open_spots.push_back(n);

        if (open_spots.empty())
        {
            // no room to move: stay
            return;
        }

        // pick best spot
        size_t best_spot = cellSpot;
        if (prey_ptr->prey_swarm_mode)
        {
            best_spot = MoveTowardPrey(cellSpot, open_spots);
        }
        else
        {
            best_spot = MoveAwayFromPrey(cellSpot, open_spots);
        }

        // if we found a different spot, move; else, random move
        if (best_spot != cellSpot)
        {
            auto org = ExtractOrganism(cellSpot);
            AddOrgAt(org, best_spot);
        } 
        
        else
        {
            MoveOrg(cellSpot);
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
            pred_ptr->increasePreyConsumed(); // Increases totalPreyConsumed variable in Predator organism

            // int vision_width = pred_ptr->getWidthOfVision();
            // int vision_height = pred_ptr->getHeightOfVision();
            int eaten = pred_ptr->getTotalPreyConsumed();
            total_prey_consumed = eaten;
            // std::cout << "Count of total prey eaten:" << eaten << std::endl;
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

        if (!IsOccupied(newPosition)) {
        AddOrgAt(extracted_org, newPosition);
        } else {
        // collisions don’t trigger eating any more—just stay
        AddOrgAt(extracted_org, pos);
        }
    }

    /*
        Input: Integer
        Output: Void
        Purpose: Checks an organisms visible area in front of it (Only N rn) and attacks visible orgs
    */
    void checkPredVisibleArea(int cellSpot)
    {
        // std::cout << "Predator spot: " << cellSpot << std::endl;

        // Grab Predator Pointer to call predator functions
        emp::Ptr<Organism> org_ptr = pop[cellSpot];
        Predator *pred_ptr = dynamic_cast<Predator *>(org_ptr.Raw());

        // Grab height and width for vision
        int heightOfVision = pred_ptr->getHeightOfVision();
        vision_height = heightOfVision;
        int widthOfVision = pred_ptr->getWidthOfVision();
        vision_width = widthOfVision;

        // Create and reserve space for visible spots array
        emp::vector<size_t> visibleSpots;
        visibleSpots.reserve(heightOfVision * widthOfVision);

        // Fill in visiblespots with function call
        visibleSpots = getVisibleArea(cellSpot, visibleSpots, heightOfVision, widthOfVision, &highlighted_cells);

        // Get all prey in sight
        emp::vector<size_t> preyinVision = getPreyInVsion(visibleSpots);

        // Build a new list of _actually_ spotted prey
        emp::vector<size_t> spottedPrey;
        spottedPrey.reserve(preyinVision.size());

        for (size_t spot : preyinVision) {

            emp::Ptr<Organism> org_ptr = pop[spot];
            KFC *prey_ptr = dynamic_cast<KFC *>(org_ptr.Raw());

            // Camouflage roll:
            if (random.GetDouble() < prey_ptr->getCamouflageVlaue()) 
            {
                // successful camouflage → reward
                //std::cout << "hid succesfully adding pts to camo value" << std::endl;
                prey_ptr->addCamouflage(camouflage_value);
            } 
            else 
            {
                // failed camouflage → mark and queue for attack
                prey_ptr->setSpotted(true);
                spottedPrey.push_back(spot);
            }
        }

        // Now attack only those prey that actually failed their camouflage check
        float attackChance = getAttackChance(spottedPrey.size());
        emp::vector<size_t> attackSpots = checkAttackRange(cellSpot);
        Attack(spottedPrey, pop[cellSpot], attackSpots);
    }

    emp::vector<size_t> checkAttackRange(int cellSpot)
    {
        // All predator have same attack range, only vision changes
        int heightOfVision = 2;
        int widthOfVision = 3;

        emp::vector<size_t> attackSpots;
        attackSpots.reserve(heightOfVision * widthOfVision);
        attackSpots = getVisibleArea(cellSpot, attackSpots, heightOfVision, widthOfVision, &attackRange_cells);

        return attackSpots;
    }

    emp::vector<size_t> getVisibleArea(int cellSpot, emp::vector<size_t> givenVector, int Height, int Width, std::set<size_t> *optionalVector = NULL)
    {
        givenVector.reserve(Height * Width);

        // We need to figure out (col, row). GRID IS IN COLUMN-MAJOR ORDER
        int cols = grid_w_boxes;
        int rows = grid_h_boxes;

        // column index = cellSpot / #rows
        // row index    = cellSpot % #rows
        int col0 = cellSpot / rows;
        int row0 = cellSpot % rows;

        // For each "layer" h = 0..heightOfVision-1, we look (h+1) cells north:
        for (int h = 0; h < Height; h++)
        {
            // how many columns to include at this "depth"
            int curWidth = std::max(1, Width - 2 * h);
            int half = curWidth / 2;

            // move (h+1) steps *up* (north) = subtract from row0
            int newRow = row0 - (h + 1);
            // toroidal wrap in [0 .. rows-1]
            newRow = ((newRow % rows) + rows) % rows;

            // scan left TO right
            for (int dcol = -half; dcol <= half; dcol++)
            {
                int newCol = col0 + dcol;
                // wrap horizontally in [0 .. cols-1]
                newCol = ((newCol % cols) + cols) % cols;

                // convert (newCol, newRow) back to a column‐major index:
                size_t target = (size_t)newCol * rows + newRow;

                givenVector.push_back(target);

                if (optionalVector != NULL)
                {
                    optionalVector->insert(target);
                }
            }
        }

        return givenVector;
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

        total_prey_seen += countOfOrgs;
        // std::cout << "Count of total prey seen: " << total_prey_seen << std::endl;
        return countOfOrgs;
    }

    // Predator-only attack function
    void Attack(const emp::vector<size_t> &visibleSpots,
        emp::Ptr<Organism> org_ptr,
        const emp::vector<size_t> & /*attackSpots—no longer needed*/)
        {
            // 1) Only predators can attack
            Predator *pred = dynamic_cast<Predator*>(org_ptr.Raw());
            if (!pred) return;

            // 2) Handling‐time cooldown
            if (!pred->CanAttack(step_counter)) return;

            // 3) Build list of attackable prey
            emp::vector<size_t> targets = getSpottedPrey(getPreyInVsion(visibleSpots));
            if (targets.empty()) {
                // No prey to hit, but we still incur the attempt delay
                pred->NoteAttack(step_counter);
                return;
            }

            // 4) Do the probability roll exactly as before
            int idx = random.GetInt((int)targets.size());
            float attackChance = getAttackChance((int)targets.size());
            if (random.GetDouble() < attackChance) {
                EatSpecies(org_ptr, targets[idx]);
            } else {
                std::cout << "Attack failed due to probability.\n";
            }

            // 5) Start the handling‐time clock now
            pred->NoteAttack(step_counter);
        }

    // Get prey in predator vision
    emp::vector<size_t> getPreyInVsion(emp::vector<size_t> visibleArea)
    {
        emp::vector<size_t> targets;

        // Get spots with prey in them
        for (size_t spot : visibleArea)
        {
            if (IsOccupied(spot))
            {
                targets.push_back(spot);
            }
        }

        return targets;
    }

    // Get spotted prey (that failed camo check)
    emp::vector<size_t> getSpottedPrey(emp::vector<size_t> preyInVision)
    {
        emp::vector<size_t> targets;

        // Get spots with prey in them
        for (size_t spot : preyInVision)
        {
            if (IsOccupied(spot))
            {
                emp::Ptr<Organism> org_ptr = pop[spot];
                KFC *prey_ptr = dynamic_cast<KFC *>(org_ptr.Raw());

                if (!prey_ptr->checkCamo())
                {
                    //std::cout << "prey failed camo test" << std::endl;
                    targets.push_back(spot);
                }
                // else nothing
            }
        }

        return targets;
    }
    
    // Predator Confusion Mechanism
    float getAttackChance(int visibleTargets)
    {
        if (visibleTargets <= 0)
            return 0.0f;
        float chanceOfAttack = 1.0f / visibleTargets;

        return chanceOfAttack;
    }

    // Find closest prey for swarming behavior
    int FindClosestPrey(int predator_index, const emp::vector<size_t> &visibleSpots)
    {
        int min_dist = std::numeric_limits<int>::max();
        int closest_prey_index = -1;

        for (int spot : visibleSpots)
        {
            if (IsOccupied(spot) && pop[spot]->SpeciesName() == "KFC")
            {
                int dist = GetManhattanDistance(predator_index, spot);
                if (dist < min_dist)
                {
                    min_dist = dist;
                    closest_prey_index = spot;
                }
            }
        }

        return closest_prey_index;
    }

    // Grab distance between two organisms
    int GetManhattanDistance(int index1, int index2)
    {
        // Below converts Colum-Major grid into x,y thing
        int x1 = index1 / grid_h_boxes;
        int y1 = index1 % grid_h_boxes;

        int x2 = index2 / grid_h_boxes;
        int y2 = index2 % grid_h_boxes;

        int distance = std::abs(x1 - x2) + std::abs(y1 - y2);

        return distance;
    }

    // Moves Predators toward given prey spot
    void MovePredatorTowards(int predator_index, int prey_index)
    {
        int col_pred = predator_index / grid_h_boxes;
        int row_pred = predator_index % grid_h_boxes;

        int col_prey = prey_index / grid_h_boxes;
        int row_prey = prey_index % grid_h_boxes;

        emp::vector<std::pair<int, int>> moves;

        // Check if prey is "north" considering toroidal wrap
        int vertical_dist = (row_pred - row_prey + grid_h_boxes) % grid_h_boxes;
        
        if (vertical_dist > 0 && vertical_dist <= grid_h_boxes / 2)
        {
            int target_row = (row_pred - 1 + grid_h_boxes) % grid_h_boxes;

            if ((col_prey - col_pred + grid_w_boxes) % grid_w_boxes <= grid_w_boxes / 2)
            {

                // Prefer moving toward the prey column-wise as well
                if (col_prey < col_pred || (col_prey > col_pred && (col_prey - col_pred) > grid_w_boxes / 2))
                {
                    moves.emplace_back((col_pred - 1 + grid_w_boxes) % grid_w_boxes, target_row); // top-left
                }
                if (col_prey > col_pred || (col_pred > col_prey && (col_pred - col_prey) > grid_w_boxes / 2))
                {
                    moves.emplace_back((col_pred + 1) % grid_w_boxes, target_row); // top-right
                }
            }

            moves.emplace_back(col_pred, target_row); // directly up
        }

        // Attempt moves in preferred order
        for (auto [new_col, new_row] : moves)
        {
            int new_index = new_col * grid_h_boxes + new_row;

            if (!IsOccupied(new_index))
            {
                auto org = ExtractOrganism(predator_index);
                AddOrgAt(org, new_index);
                return;
            }
        }

        // fallback: no move
    }

    // Grab spots of neighbors
    emp::vector<size_t> GetNeighborIndices(int cell_index)
    {
        emp::vector<size_t> neighbor_indices;
        // Convert linear index to (x, y) coordinates
        int x = cell_index / grid_w_boxes;
        int y = cell_index % grid_h_boxes;

        for (int d_col = -1; d_col <= 1; ++d_col)
        {
            for (int d_row = -1; d_row <= 1; ++d_row)
            {
                // Skip the center cell itself
                if (d_col == 0 && d_row == 0)
                    continue;

                // Wrap around toroidally
                int nx = (x + d_col + grid_w_boxes) % grid_w_boxes;
                int ny = (y + d_row + grid_h_boxes) % grid_h_boxes;

                size_t neighbor_idx = nx * grid_h_boxes + ny;
                neighbor_indices.push_back(neighbor_idx);
            }
        }

        return neighbor_indices;
    }

    // pick the open spot with the most adjacent prey
    size_t MoveTowardPrey(size_t current_pos, const emp::vector<size_t> &candidates)
    {
        // We want the maximum number of nearby prey; start below any possible neighbor count.
        int max_adjacent_prey = -1;
        size_t best_spot = current_pos;

        for (size_t spot : candidates)
        {
            // Count how many prey are adjacent to this candidate spot
            int adjacent_prey_count = 0;
            for (size_t neighbor : GetNeighborIndices(spot))
            {
                if (IsOccupied(neighbor) && pop[neighbor]->SpeciesName() == "KFC")
                {
                    ++adjacent_prey_count;
                }
            }

            // If this spot has more adjacent prey, it’s a better swarm location
            if (adjacent_prey_count > max_adjacent_prey)
            {
                max_adjacent_prey = adjacent_prey_count;
                best_spot = spot;
            }
        }

        return best_spot;
    }

    // Prey disperse behavior function
    size_t MoveAwayFromPrey(size_t current_pos, const emp::vector<size_t> &candidates)
    {
        // We want the minimum number of nearby prey; start higher than any possible neighbor count.
        int min_adjacent_prey = std::numeric_limits<int>::max();
        size_t best_spot = current_pos;

            for (size_t spot : candidates)
            {
            // Count how many prey are adjacent to this candidate spot
            int adjacent_prey_count = 0;
                for (size_t neighbor : GetNeighborIndices(spot))
                {
                    if (IsOccupied(neighbor) && pop[neighbor]->SpeciesName() == "KFC")
                    {
                    ++adjacent_prey_count;
                }
            }

            // If this spot has fewer adjacent prey, it’s a better disperse location
                if (adjacent_prey_count < min_adjacent_prey)
                {
                min_adjacent_prey = adjacent_prey_count;
                best_spot = spot;
            }
        }

        return best_spot;
    }

    // Count how many prey are alive
    int getCountAlivePrey()
    {
        int count = 0;
        for (size_t i = 0; i < pop.size(); ++i)
        {
            if (IsOccupied(i) && pop[i]->SpeciesName() == "KFC")
            {
                ++count;
            }
        }
        return count;
    }

    int getPredatorFitness()
    {
        float predFitness = 0;
        for (size_t i = 0; i < pop.size(); ++i)
        {
            if (IsOccupied(i) && pop[i]->SpeciesName() == "Predator")
            {
                emp::Ptr<Organism> org_ptr = pop[i];
                Predator *pred_ptr = dynamic_cast<Predator *>(org_ptr.Raw());

                predFitness = pred_ptr->GetFitness();
            }
        }
        return predFitness;
    }
    emp::DataMonitor<int> &GetOrgPreyConsumedDataNode()
    {
        if (!data_prey_consumed)
        {
            data_prey_consumed.New();
            OnUpdate([this](size_t update)
                     {
                         data_prey_consumed->Reset();
                         data_prey_consumed->AddDatum(total_prey_consumed); });
        }
        return *data_prey_consumed;
    }

    emp::DataMonitor<int> &GetOrgPreySeenDataNode()
    {
        if (!data_prey_seen)
        {
            data_prey_seen.New();
            OnUpdate([this](size_t update)
                     {
            data_prey_seen->Reset();
            data_prey_seen->AddDatum(total_prey_seen); });
        }
        return *data_prey_seen;
    }

    emp::DataFile &SetupOrgFile(const std::string &filename)
    {
        auto &file = SetupFile(filename);

        auto &node1 = GetOrgPreySeenDataNode();
        auto &node2 = GetOrgPreyConsumedDataNode();

        file.AddVar(update, "update", "Update");
        file.AddTotal(node1, "prey_seen", "Total Prey Seen by Predators");
        file.AddTotal(node2, "prey_consumed", "Total Prey Consumed");

        file.AddVar(vision_size, "predator_vision", "Vision size");
        file.AddVar(vision_height, "predator_vision_height", "Vision height");
        file.AddVar(vision_width, "predator_vision_width", "Vision width");

        file.PrintHeaderKeys();

        return file;
    }
};

#endif