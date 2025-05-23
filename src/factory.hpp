//
// Created by  Владимир Малахов on 14.02.2025.
// Done some reworks by Daniil on same date
//

#ifndef factory_hpp__
#define factory_hpp__

#include "building.hpp"

class Factory: public Building {
private:
    unsigned int recipy_id;
    unsigned int level;
    State state=State::OK;
    unsigned factoryMaterialsStart;
public:
    virtual Types type() const {return Types::Factory;}
    Factory(unsigned id, point<ll> position,  Direction d);
    Factory(unsigned id, point<ll> position,  Direction d, unsigned recipy_id);

    State get_state() final;

    //ActionResult put_material(Material *m);

    // -1 is product
    //Material* get_material(unsigned cell);

    ActionResult action() final;

    //bool canDoAction() final;

    bool isEnoughIngridients() const;

    unsigned getRecipyId() const;

    ActionResult changeRecipy(unsigned id);
    // produce materials
    void produce();
};

class Chest: public Building {
    unsigned int level;
    public:
    virtual Types type() const {return Types::Chest;}
    Chest(unsigned id, point<ll> position,  Direction d);
    ActionResult put_material(Material *m);
};




#endif 
