#ifndef Predator_H
#define Predator_H

#include "emp/Evolve/World.hpp"
#include "emp/math/random_utils.hpp"
#include "emp/math/Random.hpp"

#include "Org.h"

class Predator : public Organism
{

public:
    Predator(emp::Ptr<emp::Random> _random, double _points = 0.0)
        : Organism(_random, _points) {}

    /*
        Input: Void
        Output: std::string
        Purpose: Returns name of the species, Predator organism in this case
     */
    std::string SpeciesName() const override
    {
        return "Predator";
    }

    /*
        Input: Organism class
        Output: Bool
        Purpose: Decides if organism can eat passed in org. Wolves can only eat Deer organisms
    */
    virtual bool SpeciesEat(Organism *other) override
    {
        if (other->SpeciesName() == "Deer")
        {
            points += 700;
            return true;
        }
        else
        {
            return false;
        }
    }

    /*
        Input: a vector of prey organisms to eat, a random
        Output: emp::Ptr<Organism>
        Purpose: Removes a prey from the world once a wolf has attacked it
    */
    emp::Ptr<Organism> Attack(emp::vector<emp::Ptr<Organism>> prey, emp::Random &random)
    {
        // Pick a random prey in the vector to return

        emp::vector<size_t> schedule = emp::GetPermutation(random, prey.size());

        for (int i : schedule)
        {
            emp::Ptr<Organism> return_prey = prey[i];
            return return_prey;
        }
    }

    /*
    Input: Void
    Output: emp::Ptr<Organism>
    Purpose: Checks if organism meets reproduction requirements, returns
             organism type pointer if conditions met. Null pointer otherwise
*/
    emp::Ptr<Organism> CheckReproduction() override
    {
        if (points >= 1000)
        {
            SetPoints(0);
            emp::Ptr<Predator> PredatorOffSpring = new Predator(random, 0.0);
            PredatorOffSpring->SetPoints(0);
            return PredatorOffSpring;
        }
        return nullptr;
    }

    /*
        Input: Void
        Output: Bool
        Purpose: Decides if organism meets death criteria
    */
    bool CheckShouldOrgDie() override
    {
        if (points <= -9000)
        {
            return true;
        }
        return false;
    }

    /*
        Input: double type
        Output: Void
        Purpose: Updates specied amount of points for a deer organism
    */
    void Process(double given_points) override
    {
        points += given_points;
        if (hasEaten == false)
        {
            points -= 15;
        }
    }
};

#endif