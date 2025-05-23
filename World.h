#ifndef WORLD_H
#define WORLD_H

#include "emp/Evolve/World.hpp"
#include "emp/math/random_utils.hpp"
#include "emp/math/Random.hpp"

#include "Org.h"

class OrgWorld : public emp::World<Organism>
{

    emp::Random &random;
    emp::Ptr<emp::Random> random_ptr;

public:
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

        emp::World<Organism>::Update();

        emp::vector<size_t> schedule1 = emp::GetPermutation(random, GetSize());
        emp::vector<size_t> schedule2 = emp::GetPermutation(random, GetSize());

        // Calls the process function for each organism
        CallProcesses(schedule1, pointsPerUpdate);

        // Method for vision check, maybe be in org, but we'll see
        for (int cellSpot : schedule1){

            // Working only with cells that have an organism
            if (IsOccupied(cellSpot)){
                // Call function to check cells in hardcoded areas
                checkVisibleArea(cellSpot);
            }
        }

        // Checks conditions for reproduction and lets organisms move
        for (int i : schedule2)
        {
            if (IsOccupied(i))
            {
                emp::Ptr<Organism> offspring = pop[i]->CheckReproduction();

                // If offspring is made, place into non-empty box
                if (offspring)
                {
                    PlaceOffspring(offspring, i);
                }

                // Move non-grass organisms to random neighboring position, if occupied check if can be eaten
                if (pop[i]->SpeciesName() != "Grass")
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
            DeleteOrganism(pop[diff_org_position]);

            given_org->hasEaten = true;

            AddOrgAt(given_org, diff_org_position);
            return true;
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
        Purpose: Checks an organisms visible area in front of it
    */
    void checkVisibleArea(int cellSpot){
        // We want to check a cell spot
        int heightOfVision = 2;
        int widthOfVision = 3;

        for (int i = 0; i < heightOfVision; i++){
            for (int j = 0; j < widthOfVision; i++){
                
            }
            widthOfVision = widthOfVision--;
        }

        // Direction (front) = North

        // Directly in front (- #width of grid)

        // Directly in front to the right (- #widthOfGrid + 1)

        // Directly in front to the left (- #widthOfGrid - 1)

        // Two spots directly in front (2 * (- #widthOfGrid))

        // Two spots directly in front to the left (2 * (- #widthOfGrid) - 1)

        // Two spots directly in front to the right (2 * (- #widthOfGrid) + 1)
        int visionDepth = 0;
    }

    void getToroidalBound(int cellSpot, int visionDepth){
        if (cellSpot < 0) {
            cellSpot = (visionDepth - 1);
        }
        if (y_neighbor > (num_h_boxes - 1)) {
            y_neighbor = 0;
        }
        if (x_neighbor < 0) {
            x_neighbor = (num_w_boxes - 1);
        }
        if (x_neighbor > (num_w_boxes - 1)){
            x_neighbor = 0;
        }
    }
};
#endif