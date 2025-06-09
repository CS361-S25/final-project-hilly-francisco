#ifndef Predator_H
#define Predator_H

#include "emp/Evolve/World.hpp"
#include "emp/math/random_utils.hpp"
#include "emp/math/Random.hpp"

#include "Org.h"

class Predator : public Organism
{

private:
    bool has_target = false;
    size_t chase_target;

public:
    int totalPreyConsumed = 0;
    // Must always be pos
    int heightOfVision = 3;

    // Must be pos and odd (clean pyramids that way)
    int widthOfVision = 5;

    int last_attack_step = -1000;    // initialized so first attack is always allowed
    static constexpr int HANDLING_TIME = 10;

    Predator(emp::Ptr<emp::Random> _random, double _points = 0.0, int _heightOfVision = 3, int _widthOfVision = 5)
        : Organism(_random, _points),
          heightOfVision(_heightOfVision),
          widthOfVision(_widthOfVision) {}

    bool CanAttack(int current_step) const {
        return (current_step - last_attack_step) >= HANDLING_TIME;
    }
    
    void NoteAttack(int current_step) {
        last_attack_step = current_step;
    }
    /*
        Input: Void
        Output: std::string
        Purpose: Returns name of the species, Predator organism in this case
     */
    std::string SpeciesName() const override
    {
        return "Predator";
    }

    int getHeightOfVision() { return heightOfVision; }

    int getWidthOfVision() { return widthOfVision; }
    int getTotalPreyConsumed() { return totalPreyConsumed; }
    void increasePreyConsumed() { totalPreyConsumed = totalPreyConsumed + 1; }

    void SetChaseTarget(size_t pos)
    {
        chase_target = pos;
        has_target = true;
    }

    void ClearChaseTarget()
    {
        has_target = false;
    }

    bool HasTarget() const
    {
        return has_target;
    }

    size_t GetChaseTarget() const
    {
        return chase_target;
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
            // points -= 15;
            points -= 0;
        }
    }
};

#endif