#ifndef ORG_H
#define ORG_H

#include "emp/math/Random.hpp"
#include "emp/tools/string_utils.hpp"

class Organism {
    public:
    double points;
    bool hasEaten = false;

    emp::Ptr<emp::Random> random;

    Organism(emp::Ptr<emp::Random> _random, double _points=0.0) :
        points(_points), random(_random) {;}

    void SetPoints(double _in) {points = _in;}
    void AddPoints(double _in) {points += _in;}


    /*
        Input: double type
        Output: Void
        Purpose: Updates specied amount of points for a deer organism
    */
    virtual void Process(double given_points) {
        points += given_points;
        if (hasEaten == false){
            points -= 20;
        }
    }

    /*
        Input: Void
        Output: emp::Ptr<Organism>
        Purpose: Checks if organism meets reproduction requirements, returns 
                 organism type pointer if conditions met. Null pointer otherwise
    */
    virtual emp::Ptr<Organism> CheckReproduction() {
        if (points >= 1000) {
            emp::Ptr<Organism> offspring = new Organism(*this);
            offspring->SetPoints(0);
            std::cout << "Org points set to zero" << std::endl;
            SetPoints(0);
            return offspring;
        }
        return nullptr;
    }

    /*
        Input: Void
        Output: std::string
        Purpose: Returns name of the species, generic organism in this case
    */
    virtual std::string SpeciesName() const{
        return "Generic";
    }

    /*
        Input: Organism class
        Output: Bool
        Purpose: Decides if organism can eat passed in org. Defaults to only eating grass
    */
    virtual bool SpeciesEat(Organism* other) {
        if (other->SpeciesName() == "Grass") {
            points += 120;
            return true;
        }
        else {
            return false;
        }
    }

    /*
        Input: Void
        Output: Bool
        Purpose: Decides if organism meets death criteria
    */
    virtual bool CheckShouldOrgDie() {
        if (points <= -1000){
            return true;
        }
        return false;
    }
};

#endif