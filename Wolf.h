#ifndef WOLF_H
#define WOLF_H

#include "emp/Evolve/World.hpp"
#include "emp/math/random_utils.hpp"
#include "emp/math/Random.hpp"

#include "Org.h"

class Wolf : public Organism {

    public:
        Wolf(emp::Ptr<emp::Random> _random, double _points = 0.0)
            : Organism(_random, _points) {}

        /*
            Input: Void
            Output: std::string
            Purpose: Returns name of the species, wolf organism in this case
         */
        std::string SpeciesName() const override {
            return "Wolf";
        }

        /*
            Input: Organism class
            Output: Bool
            Purpose: Decides if organism can eat passed in org. Wolves can only eat Deer organisms
        */
        virtual bool SpeciesEat(Organism* other) override {
            if (other->SpeciesName() == "Deer") {
                points += 700;
                return true;
            }
            else {
                return false;
            }
        }

        /*
            Input: Void
            Output: emp::Ptr<Organism>
            Purpose: Checks if organism meets reproduction requirements, returns 
                     organism type pointer if conditions met. Null pointer otherwise
        */
        emp::Ptr<Organism> CheckReproduction() override {
            if (points >= 1000){
                SetPoints(0);
                emp::Ptr<Wolf> wolfOffSpring = new Wolf(random, 0.0);
                wolfOffSpring->SetPoints(0);
                return wolfOffSpring;
            }
            return nullptr;
        }

        /*
            Input: Void
            Output: Bool
            Purpose: Decides if organism meets death criteria
        */
        bool CheckShouldOrgDie() override {
            if (points <= -9000){
                return true;
            }
            return false;
        }

        /*
            Input: double type
            Output: Void
            Purpose: Updates specied amount of points for a deer organism
        */
        void Process(double given_points) override {
            points += given_points;
            if (hasEaten == false){
                points -= 15;
            }
        }
};

#endif