#ifndef KFC_H
#define KFC_H

#include "emp/Evolve/World.hpp"
#include "emp/math/random_utils.hpp"
#include "emp/math/Random.hpp"

#include "Org.h"

class KFC : public Organism
{

public:
    KFC(emp::Ptr<emp::Random> _random, double _points = 0.0)
        : Organism(_random, _points) {}

    /*
        Input: Void
        Output: std::string
        Purpose: Returns name of the species, KFC in this case
    */
    std::string SpeciesName() const override
    {
        return "KFC";
    }

    /*
        Input: double type
        Output: Void
        Purpose: Updates specied amount of points for a KFC organism
    */
    void Process(double given_points) override
    {
        // used to be -30 for metabolism
        AddPoints(30);// points for surviving
    }

    /*
        Input: Void
        Output: emp::Ptr<Organism>
        Purpose: Checks if organism meets reproduction requirements, returns
                 organism type pointer if conditions met. Null pointer otherwise
    */
    emp::Ptr<Organism> CheckReproduction() override
    {
        if (points > 900)
        {
            points = 200;
            emp::Ptr<KFC> KFCOffSpring = new KFC(random, 0.0);
            KFCOffSpring->SetPoints(0);
            return KFCOffSpring;
        }
        return nullptr;
    }

    /*
        Input: Void
        Output: Bool
        Purpose: Checks if organism meets death requirements. If yes, returns true
    */
    bool CheckShouldOrgDie() override
    {
        if (points <= -400)
        {
            return true;
        }
        return false;
    }
};

#endif