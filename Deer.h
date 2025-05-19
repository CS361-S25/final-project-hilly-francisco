#ifndef DEER_H
#define DEER_H

#include "emp/Evolve/World.hpp"
#include "emp/math/random_utils.hpp"
#include "emp/math/Random.hpp"

#include "Org.h"

class Deer : public Organism {

    public:
        Deer(emp::Ptr<emp::Random> _random, double _points = 0.0)
            : Organism(_random, _points) {}

        /*
            Input: Void
            Output: std::string
            Purpose: Returns name of the species, deer in this case
        */
        std::string SpeciesName() const override {
            return "Deer";
        }

        /*
            Input: double type
            Output: Void
            Purpose: Updates specied amount of points for a deer organism
        */
        void Process(double given_points) override {
            AddPoints(-30);  // cost of metabolism
        }

        /*
            Input: Void
            Output: emp::Ptr<Organism>
            Purpose: Checks if organism meets reproduction requirements, returns 
                     organism type pointer if conditions met. Null pointer otherwise
        */
        emp::Ptr<Organism> CheckReproduction() override {
            if (points > 900){
                points = 200;
                emp::Ptr<Deer> deerOffSpring = new Deer(random, 0.0);
                deerOffSpring->SetPoints(0);
                return deerOffSpring;
            }
            return nullptr;
        }

        /*
            Input: Void
            Output: Bool
            Purpose: Checks if organism meets death requirements. If yes, returns true
        */
        bool CheckShouldOrgDie() override {
            if (points <= -400){
                return true;
            }
            return false;
        }
};

#endif