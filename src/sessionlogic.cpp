#include "sessionlogic.hpp"

const std::set<Object *> SessionHandler::get_layer(ObjectTypes lr) const
{
    return this->objs[lr];
}

ActionResult SessionHandler::delFromLayerB(Object *obj)
{
    if (!this->objs[ObjectTypes::Buildings].count(obj))
        return ActionResult::BAD;
    tims.active = 1;
    this->objs[ObjectTypes::Buildings].erase(obj);
    this->tims.unregister_timer(obj);
    std::vector<ICarryObj *> stack;
    for (auto builds : ((Building *)obj)->get_Connection(Connections::Standart)->GetConnectionsTo())
        stack.push_back(builds);
    for (auto builds : ((Building *)obj)->get_Connection(Connections::Standart)->GetConnectionsFrom())
        stack.push_back(builds);
    delete obj;
    for (ICarryObj *build : stack)
    {
        std::cout<<build<<" "<<dynamic_cast<Dummy *>(build)<<" "<<dynamic_cast<Object *>(build)<<" "<<obj<<std::endl;
        if (dynamic_cast<Object *>(build) == nullptr)
            throw std::runtime_error("Something went wrong in navigating building tree!");
        if (dynamic_cast<Dummy *>(build) == nullptr)MakeConnections(dynamic_cast<Building *>(build));
        else ClearDummies({dynamic_cast<Dummy *>(build)});
    }
    tims.active = 0;
    return ActionResult::OK;
}

Object *SessionHandler::findObj(point<ll> p, ObjectTypes layer = ObjectTypes::Buildings)
{
    point<ll> pb;
    point<unsigned> sz;
    for (auto obj : objs[layer])
    {
        pb = obj->getPosition();
        sz = obj->getSize();
        if (p.x <= pb.x + sz.x - 1 && p.x >= pb.x)
            if (p.y <= pb.y + sz.y - 1 && p.y >= pb.y)
                return obj;
    }
    return nullptr;
}

std::set<Object *> SessionHandler::findInters(point<ll> p, point<unsigned> sz, ObjectTypes layer)
{
    std::set<Object *> st;
    point<ll> pb;
    point<unsigned> szb;
    for (auto obj : objs[layer])
    {
        pb = obj->getPosition();
        szb = obj->getSize();
        if (max(pb.x, p.x) <= min(pb.x + szb.x - 1, p.x + sz.x - 1) && p.x <= pb.x + szb.x - 1 && pb.x <= p.x + sz.x - 1)
            if (max(pb.y, p.y) <= min(pb.y + szb.y - 1, p.y + sz.y - 1) && p.y <= pb.y + szb.y - 1 && pb.y <= p.y + sz.y - 1)
                st.insert(obj);
    }
    return st;
}

void SessionHandler::MakeConnections(Object *b)
{
    point<ll> p;
    Object *o;
    switch (b->type())
    {
    case Types::Factory:
        break;
    case Types::Chest:
        break;
    case Types::Conveyer:
        p = b->getPosition() + dynamic_cast<Building *>(b)->getDirection().get();
        o = findObj(p);

        if (o == nullptr)
            o = findObj(p, ObjectTypes::SpecialPoints);
        if (o == nullptr)
        {
            o = new Dummy(p);
            objs[ObjectTypes::SpecialPoints].insert(o);
        }
        MakeConnFull(dynamic_cast<ICarryObj *>(b), dynamic_cast<ICarryObj *>(o), Connections::Standart);
        if (dynamic_cast<Conveyer *>(o))
            MakeConnStrait(dynamic_cast<ICarryObj *>(b), dynamic_cast<ICarryObj *>(o), Connections::Chain);
        break;
    case Types::Inserter:
        p = b->getPosition() + dynamic_cast<Building *>(b)->getDirection().get();
        o = findObj(p);
        if (o == nullptr)
            o = findObj(p, ObjectTypes::SpecialPoints);
        if (o == nullptr)
        {
            o = new Dummy(p);
            objs[ObjectTypes::SpecialPoints].insert(o);
        }
        std::cout << (MakeConnFull(dynamic_cast<ICarryObj *>(b), dynamic_cast<ICarryObj *>(o), Connections::Standart) == ActionResult::OK) << " mc\n";
        if (!dynamic_cast<Inserter *>(o) && !dynamic_cast<Dummy *>(o))
            MakeConnStrait(dynamic_cast<ICarryObj *>(b), dynamic_cast<ICarryObj *>(o), Connections::Chain);

        p = b->getPosition() - dynamic_cast<Building *>(b)->getDirection().get();
        o = findObj(p);
        if (o == nullptr)
            o = findObj(p, ObjectTypes::SpecialPoints);
        if (o == nullptr)
        {
            o = new Dummy(p);
            objs[ObjectTypes::SpecialPoints].insert(o);
        }
        std::cout << (MakeConnFull(dynamic_cast<ICarryObj *>(o), dynamic_cast<ICarryObj *>(b), Connections::Standart)== ActionResult::OK) << " mc2\n";
        if (!dynamic_cast<Inserter *>(o) && !dynamic_cast<Dummy *>(o))
            MakeConnForward(dynamic_cast<ICarryObj *>(o), dynamic_cast<ICarryObj *>(b), Connections::Chain);
        break;
    default:
        throw std::invalid_argument("Bad call for making connections");
        break;
    }
}

void SessionHandler::ClearDummies(const std::set<Object *> &setobj)
{
    for (auto obj : setobj)
    {
        // std::cout<<obj<<'\n';
        if (dynamic_cast<Dummy *>(obj))
        {
            for (auto obj_c : dynamic_cast<Dummy *>(obj)->get_Connection(Connections::Standart)->GetConnectionsFrom())
            {
                // std::cout<<obj_c<<'\n';
                MakeConnections(dynamic_cast<Object *>(obj_c));
            }
            for (auto obj_c : dynamic_cast<Dummy *>(obj)->get_Connection(Connections::Standart)->GetConnectionsTo())
            {
                // std::cout<<obj_c<<'\n';
                MakeConnections(dynamic_cast<Object *>(obj_c));
            }
        }
        else
            throw std::invalid_argument("There are some not dummies in specialpoints layer!");
        objs[ObjectTypes::SpecialPoints].erase(obj);
        delete obj;
    }
}

Object *SessionHandler::addToLayerB(unsigned id, point<ll> p, Direction dir, unsigned reciepy_id)
{
    Types T = TypesHandler::getTypeById(id);
    Object *o;
    switch (T)
    {
    case Types::Conveyer:
        o = new Conveyer(id, p, dir);
        break;
    case Types::Factory:
        o = new Factory(id, p, dir,reciepy_id); // o = new Factory(id, p, dir);
        break;
    case Types::Inserter:
        o = new Inserter(id, p, dir); // o = new Inserter(id, p, dir);
        break;
    case Types::Chest:
        o = new Chest(id, p, dir);
        break;
    default:
        throw std::invalid_argument("Bad call for type that creates in layer buildings!");
        break;
    }
    tims.active = 1;
    if (findInters(o->getPosition(), o->getSize(), ObjectTypes::Buildings).size() != 0)
    {
        delete o;
        // return nullptr;
        throw std::runtime_error("Intersection with other buildings!");
    }
    objs[ObjectTypes::Buildings].insert(o);
    ClearDummies(findInters(o->getPosition(), o->getSize(), ObjectTypes::SpecialPoints));
    MakeConnections(o);
    tims.register_timer(o);
    tims.active = 0;
    return o;
}

SessionHandler::~SessionHandler()
{
    tims.active = 2;
    for (unsigned i = 0; i < ObjectTypes::Count; i++)
        for (auto obj : objs[i])
        {
            std::cout << "del " << obj << '\n';
            delete obj;
        }
}

const std::vector<Material> SessionHandler::getBuildingInventory(Object *obj) const
{
    ICarryObj* ic=dynamic_cast<ICarryObj*>(obj);
    if (ic) return ic->getInventory();
    throw std::invalid_argument("You can't get inventory not from ICarryObj child!");
}


ActionResult SessionHandler::SaveFactory(std::string path)
{
    try {
        this->tims.active=1;
        std::fstream save(path,std::ios::out | std::ios::trunc | std::ios::out | std::ios::binary);
        for (auto obj: this->objs[ObjectTypes::Buildings])
            switch (obj->type())
            {
            case Types::Inserter:
                save<<"Inserter"<<"|"<<obj->getId().id<<"|"<<obj->getPosition().x<<"|"<<obj->getPosition().y<<"|"<<
                dynamic_cast<Building*>(obj)->getDirection().dir()<<"|\n";
                break;
            case Types::Chest:
                save<<"Chest___"<<"|"<<obj->getId().id<<"|"<<obj->getPosition().x<<"|"<<obj->getPosition().y<<"|\n";
                break;
            case Types::Conveyer:
                save<<"Conveyer"<<"|"<<obj->getId().id<<"|"<<obj->getPosition().x<<"|"<<obj->getPosition().y<<"|"<<
                dynamic_cast<Building*>(obj)->getDirection().dir()<<"|\n";
                break;
            case Types::Factory:
            save<<"Factory_"<<"|"<<obj->getId().id<<"|"<<obj->getPosition().x<<"|"<<obj->getPosition().y<<"|"<<
            dynamic_cast<Building*>(obj)->getDirection().dir()<<"|"<<dynamic_cast<Factory*>(obj)->getRecipyId()<<"|\n";
            break;
            default:
                break;
            }
    }
    catch (const std::exception &e)
    {
        std::cerr<<"Error occured when saving: "<<e.what();
        return ActionResult::BAD;
    }
    return ActionResult::OK;
}

ActionResult SessionHandler::LoadFactory(std::string path)
{
    std::map<std::string, Types> String2Type = {{"Factory_",Types::Factory},
            {"Inserter",Types::Inserter}, {"Conveyer",Types::Conveyer}, {"Chest___",Types::Chest}};
    if (objs[ObjectTypes::Buildings].size()!=0) std::cerr<<"Can not load save in not clear map\n";
    try 
    {
        this->tims.active=1;
        std::fstream save(path,std::ios::in | std::ios::binary);
        std::string s;
        std::vector<std::string> v;
        std::vector<ll> vl;
        while (std::getline(save,s))
        {
            std::cout<<"read line: "<<s<<" "<<s.substr(0,8)<<" "<<String2Type.count(s.substr(0,8))<<'\n';
            if (String2Type.count(s.substr(0,8))!=0)
            {
                v.clear();
                v.push_back("");
                for (auto c: s)
                {
                    if (c=='|') v.push_back("");
                    else v[v.size()-1]+=c;
                }
                vl.resize(v.size()-1);
                for (unsigned long i=1;i<v.size()-1;i++) 
                {
                    std::cout<<v[i]<<'\n';
                    vl[i-1]=std::stoll(v[i]);
                }
                
                if (String2Type[v[0]]==TypesHandler::getTypeById(vl[0]))
                switch (String2Type[v[0]])
                {
                case Types::Chest:
                    addToLayerB((unsigned)vl[0],point<ll>(vl[1],vl[2]),Directions::UP);
                    break;
                case Types::Conveyer:
                case Types::Inserter:
                    addToLayerB((unsigned)vl[0],point<ll>(vl[1],vl[2]),(Directions)vl[3]);
                    break;
                case Types::Factory:
                    addToLayerB((unsigned)vl[0],point<ll>(vl[1],vl[2]),(Directions)vl[3], (unsigned)vl[4]);
                    break;
                default:
                    break;
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr<<"Error occured when loading: "<<e.what();
        return ActionResult::BAD;
    }
    return ActionResult::OK;
}


//------------------------------------------------------------------------------------------------------

RunMachine::RunMachine(Building *obj) : _curr(Wait)
{
    func_call[Action] = std::function<ActionResult()>(std::bind([](Building *o) -> ActionResult
                                                                { return o->action(); }, obj));
    func_call[ActionMove] = std::function<ActionResult()>(std::bind([](Building *o) -> ActionResult
                                                                    { return o->actionMove(); }, obj));
    func_call[Wait] = std::function<ActionResult()>([]() -> ActionResult
                                                    { return ActionResult::OK; });
}

void RunMachine::Do()
{
    if (func_call[_curr]() == ActionResult::OK)
        _curr = (_curr + 1) % Count;
}

//-----------------------------------------------------------------------------------------------------

void TimersHandler::startHandling(bool onetime)
{

    RunMachine *r;
    while (!(active & 0x2))
    {
        while (busy && active)
        {
            if (active & 0x2) return;
        }
        doing = true;
        for (auto it = Timers.begin(); it != Timers.end(); it++)
        {
            r = Runner[it->first];
            if (r->getCurr() == RunMachine::Wait)
            {
                if (get_delta_s(it->first) < it->first->get_cooldown())
                    continue;
                else
                {
                    r->Do();
                    if (r->getCurr() != RunMachine::Wait)
                        restart_timer(it->first);
                }
                continue;
            }
            r->Do();
        }
        if (onetime)
        {
            break;
        }
        doing = false;
    }
    doing = false;
}

TimersHandler::~TimersHandler()
{
    for (auto rm: Runner)
        delete rm.second;
}