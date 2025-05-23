#include"conveyer.hpp"
#include "jsoncommunicate.hpp"

constexpr const char lvl[]="level";
constexpr const char cldn[]="cooldown";

Conveyer::Conveyer(unsigned id, point<ll> position, Direction d):Building(id,position,d)
{
    requirments=new MaterialList(4);
    BuildingInventory=new Material[4];
    for (unsigned cell=0; cell<4;cell++) BuildingInventory[cell].setCapacity(1);
    this->cooldpown=json_communicate::get_property<float,Factory,Checking::size_a(cldn),cldn>(id);
}

ActionResult Conveyer::put_material(Material *m) {
    for (unsigned i = 0; i < 4; i++) {
        {
            if (BuildingInventory[i].get_quantity()==0) BuildingInventory[i].ChangeId(m->getId());
        if ((BuildingInventory[i].getId() == m->getId() || BuildingInventory[i].getId()==0) &&
    BuildingInventory[i].get_quantity()<BuildingInventory->get_maxquantity()) {
            return BuildingInventory[i] + *m;
            }
        }
    }
    return ActionResult::BAD;
}

Material* Conveyer::get_material(ID<> id) {
    if (id==0) return get_material();
    for (unsigned cell=0;cell<4;cell++)
        if (this->BuildingInventory[cell].getId()==id) return this->BuildingInventory+cell;
    return nullptr;
}

Material* Conveyer::get_material() {
    for (unsigned cell=0;cell<4;cell++)
        if (this->BuildingInventory[cell].get_quantity()>0) return this->BuildingInventory+cell;
    return nullptr;
}

/*bool Conveyer::canDoAction()
{
    if (con[Connections::Chain]->GetConnectionsTo().size()==0 &&
    get_material()==nullptr) return false;
    return true;
}*/


Directions rotate(Direction main, Direction d)
{
    while (main.dir()!=Directions::UP)
    {++d; ++main;}
    return d.dir();
}

ActionResult Conveyer::put_material(short row, Material *m, Conveyer* prev)
{
    if (row>1) throw std::invalid_argument("Conveyer cell error!");
    if (get_Connection(Connections::Chain)->GetConnectionsFrom().count(dynamic_cast<ICarryObj*>(prev))) throw std::invalid_argument("Not connected!");
    switch (rotate(this->direction, prev->direction))
    {
        case Directions::UP:
            if (BuildingInventory[row].get_quantity()==0) BuildingInventory[row].ChangeId(m->getId());
            return BuildingInventory[row]+*m;
        case Directions::LEFT:
            if (BuildingInventory[row*2].get_quantity()==0) BuildingInventory[row*2].ChangeId(m->getId());
            return BuildingInventory[row*2]+*m;
        case Directions::RIGHT:
            if (BuildingInventory[row*2+1].get_quantity()==0) BuildingInventory[row*2+1].ChangeId(m->getId());
            return BuildingInventory[row*2+1]+*m;
        default:
            break;
            throw std::runtime_error("Something went wrong with conveyer!");
    }
    return ActionResult::BAD;
}

ActionResult Conveyer::action()
{
    for (int i=0;i<2;i++) 
    if (BuildingInventory[i].getId().id!=0)
    {
        if (BuildingInventory[i+2].get_quantity()==0) BuildingInventory[i+2].ChangeId(0);
        BuildingInventory[2+i]+BuildingInventory[i];
    }
    return ActionResult::OK;
}

ActionResult Conveyer::actionMove()
{
    if (get_Connection(Connections::Chain)->GetConnectionsTo().size()==1)
    {
        for (int i=0;i<2;i++)
            if (BuildingInventory[i+2].getId().id!=0)
            if (dynamic_cast<Conveyer*>(*get_Connection(Connections::Chain)->GetConnectionsTo().begin()))
                dynamic_cast<Conveyer*>(*get_Connection(Connections::Chain)->GetConnectionsTo().begin())->put_material(i,BuildingInventory+i+2,this);
    }
    else if (get_Connection(Connections::Chain)->GetConnectionsTo().size()>1)
        throw std::runtime_error("Erorr in chaining conveyer to");
    return ActionResult::OK;
}