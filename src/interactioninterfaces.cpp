#include "interactioninterfaces.hpp"
#include <assert.h>

MaterialList::MaterialList()
{
    this->count=0;
}

MaterialList::MaterialList(unsigned count)
{
    this->count=count;
    this->time = 0;
    this->ids=new ID<>[count];
    this->consumes=new int[count];
    this->name="";
}

MaterialList::MaterialList(MaterialList& m)
{
    this->count=m.count;
    this->time=m.time;
    this->ids=new ID<>[this->count];
    this->consumes=new int[this->count];
    this->name=m.name;
    for (unsigned i=0;i<this->count;i++)
    {
        this->ids[i]=m.ids[i];
        this->consumes[i]=m.consumes[i];
    }
}

MaterialList::~MaterialList()
{
    delete [] ids;
    delete [] consumes;
}

void Direction::update()
{
    switch (d)
    {
    case Directions::UP:
        this->direction=Direction::UP;
        break;
    case Directions::DOWN:
        this->direction=Direction::DOWN;
        break;
    case Directions::LEFT:
        this->direction=Direction::LEFT;
        break;
    case Directions::RIGHT:
        this->direction=Direction::RIGHT;
        break;
    default:
        this->direction=UD;
        break;
    }
    assert(this->direction!=Direction::UD);
}

Direction::Direction(Directions d):d(d),direction(Direction::UD)
{
    update();
}

Direction& Direction::operator++()
{
    d=(d+1)&3;
    update();
    return *this; // return new value by reference
}

Direction& Direction::operator--()
{
    d=(d-1)&3;
    update();
    return *this; // return new value by reference
}

void Direction::mirror()
{
    d=(d+2)&3;
    update();
}

Direction Direction::mirrored()
{
    Direction d2((Directions)this->d);
    d2.mirror();
    return d2;
}

const point<ll>& Direction::get()
{
    return this->direction;
}