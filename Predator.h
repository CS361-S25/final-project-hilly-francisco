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
        if (other->SpeciesName() == "KFC")
        {
            points += 0;
            return true;
        }
        else
        {
            return false;
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
            return nullptr;
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
            //points -= 15;
            points -= 0;
        }
    }
};

#endif