#ifndef GRASS_H
#define GRASS_H

#include "emp/Evolve/World.hpp"
#include "emp/math/random_utils.hpp"
#include "emp/math/Random.hpp"

#include "Org.h"

class Grass : public Organism {

    public:
        Grass(emp::Ptr<emp::Random> _random, double _points = 0.0)
            : Organism(_random, _points) {}

        /*
            Input: double type
            Output: Void
            Purpose: Updates specied amount of points for a deer organism
        */
        void Process(double given_points) override {
            AddPoints(20);
        }

        /*
            Input: Void
            Output: emp::Ptr<Organism>
            Purpose: Checks if organism meets reproduction requirements, returns 
                     organism type pointer if conditions met. Null pointer otherwise
        */
        emp::Ptr<Organism> CheckReproduction() override {
            if (points >= 250){
                points = 0;
                emp::Ptr<Grass> grassOffSpring = new Grass(random, 0.0);
                grassOffSpring->SetPoints(0);
                return grassOffSpring;
            }
            return nullptr;
        }

        /*
            Input: Void
            Output: std::string
            Purpose: Returns name of the species, grass in this case
        */
        std::string SpeciesName() const override {
            return "Grass";
        }

        /*
            Input: Organism class
            Output: Bool
            Purpose: Decides if organism can eat passed in org. Grass returns false as it does not eat.
        */
        bool SpeciesEat(Organism *other) override {
            return false;
        } 

        /*
            Input: Void
            Output: Bool
            Purpose: Decides if organism meets death criteria. Grass does not die from point threshold, so returns false
        */
        virtual bool CheckShouldOrgDie() override {
            return false;
        }
};

#endif