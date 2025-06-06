//
// Created by  Владимир Малахов on 14.02.2025.
// Done some reworks by Daniil on same date
//

#include "factory.hpp"
#include "material.hpp"
#include "object.hpp"
#include "jsoncommunicate.hpp"
//#include "new" ????????


constexpr const char lvl[]="level";
constexpr const char cldn[]="cooldown";

Factory::Factory(unsigned id, point<ll> position, Direction d):Building(id, position,d),recipy_id(0) {
    this->level =json_communicate::get_property<unsigned,Factory,Checking::size_a(lvl),lvl>(id);//get from json

    this->requirments = RecipyHandler::getRequirementsById(this->id);
    this->BuildingInventory = new Material[this->requirments->count];
    
    for(unsigned i=0;i<this->requirments->count;i++)
        (BuildingInventory+i)->ChangeId(this->requirments->ids[i]);

    this->cooldpown=json_communicate::get_property<float,Factory,Checking::size_a(cldn),cldn>(id)+requirments->time;
    this->factoryMaterialsStart = 2; //Needed to be grabbed from json by id of factory type
}

Factory::Factory(unsigned id, point<ll> position, Direction d, unsigned recipy_id):Building(id, position,d),recipy_id(recipy_id) {
    this->level =json_communicate::get_property<unsigned,Factory,Checking::size_a(lvl),lvl>(id);//get from json

    this->requirments = RecipyHandler::getRequirementsById(this->id, recipy_id);
    this->BuildingInventory = new Material[this->requirments->count];
    
    for(unsigned i=0;i<this->requirments->count;i++)
        (BuildingInventory+i)->ChangeId(this->requirments->ids[i]);

    this->cooldpown=json_communicate::get_property<float,Factory,Checking::size_a(cldn),cldn>(id)+requirments->time;
    this->factoryMaterialsStart = 2; //Needed to be grabbed from json by id of factory type
}

ActionResult Factory::changeRecipy(unsigned id)
{
    MaterialList* ml=RecipyHandler::getRequirementsById(this->id);
    delete [] BuildingInventory;
    delete requirments;
    this->recipy_id=id;
    this->requirments =ml;
    this->BuildingInventory = new Material[this->requirments->count];
    
    for(unsigned i=0;i<this->requirments->count;i++)
        (BuildingInventory+i)->ChangeId(this->requirments->ids[i]);
    return ActionResult::OK;
}

unsigned Factory::getRecipyId() const
{
    return this->recipy_id;
};

State Factory::get_state() {
    if (!isEnoughIngridients()) {
        state = State::NotEnoughMaterial;
        return this->state;
    }
    for (unsigned cell=0;cell<this->requirments->count;cell++)
        if ((long int)(BuildingInventory[cell].get_maxquantity()-BuildingInventory[cell].get_quantity())<(long int)requirments->consumes[cell])
        {
            state=State::Full;
            return this->state;
        }
    state = State::OK;
    return this->state;
}

bool fbf(int f)
{
    return !(f&(1<<(sizeof(unsigned)*8-1)));
}

bool Factory::isEnoughIngridients() const{
    for (unsigned i = 0; i < requirments->count; i++) {
        if ((unsigned)(requirments->consumes[i])*fbf(requirments->consumes[i]) > BuildingInventory[i].get_quantity()) {
            return false;
        }
    }
    return true;
}


/*bool Factory::canDoAction()
{
    return get_state()==State::OK;
}*/

ActionResult Factory::action() {
    get_state();
    if (isEnoughIngridients() && state == State::OK) produce();
    else return ActionResult::BAD;
    return ActionResult::OK;
}


void Factory::produce() {
    if (state == State::OK) {
        //std::cout<<"p!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<'\n';
        for (unsigned i = 0; i < requirments->count; i++)
            BuildingInventory[i] -= requirments->consumes[i];
    }
}


Chest::Chest(unsigned id, point<ll> position, Direction d):Building(id, position,d) {
    this->level =json_communicate::get_property<unsigned,Chest,Checking::size_a(lvl),lvl>(id);//get from json

    this->requirments = RecipyHandler::getRequirementsById(id);
    this->BuildingInventory = new Material[this->requirments->count];
    
    for(unsigned i=0;i<this->requirments->count;i++)
        (BuildingInventory+i)->ChangeId(this->requirments->ids[i]);
}


ActionResult Chest::put_material(Material *m) {
    //std::cout<<"st added\n";
    for (unsigned i = 0; i < requirments->count; i++) {
        //std::cout<<requirments->ids[i].id<<' '<< m->getId().id<< ' '<< (requirments->ids[i] == m->getId()) <<"\n";
        if (BuildingInventory[i].getId() == m->getId() && BuildingInventory[i].get_quantity()<BuildingInventory[i].get_maxquantity()) {
            //std::cout<<"added\n";
            return BuildingInventory[i] + *m;
        }
    }
    return ActionResult::BAD;
}