
#include"object.hpp"
#include "jsoncommunicate.hpp"


constexpr const char xs[]="size_x";
constexpr const char ys[]="size_y";

Object::Object(unsigned x_size, unsigned y_size) :
    id(0),p(0,0),size(x_size,y_size)
{
    this->initilised=true;
}

Object::Object(ID<> id) : id(id),
    size(
            json_communicate::get_property<unsigned,Object,Checking::size_a(xs),xs>(id.id),
            json_communicate::get_property<unsigned,Object,Checking::size_a(ys),ys>(id.id)
            )
{                   //dummy thin neded to replaced by json
    this->initilised=true;
}



const point<unsigned>& Object::getSize()
{
    return this->size;
}

Object::~Object()
{
    
}

/*void Object::setPosition(float x, float y) {
    sprite.setPosition({x, y});
    p.x = x / (float)pixels_per_tile;
    p.y = y / (float)pixels_per_tile;
}*/

void Object::setPosition(point<long long> pos) {
    p = pos;
//    sprite.setPosition({pos.x / })
}

const point<long long> &Object::getPosition() {
    return p;
}

const ID<>& Object::getId() {
    return id;
}