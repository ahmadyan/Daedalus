#pragma once
#include <string>

enum ObjectType {io, wire, gate};

class Object{
public:
    //Variables:
    int id;
    static int objectCount ;
    std::string name;
    ObjectType type;
    
    //Methods:
    Object();
    ///  I will dynamically cast it to it's childs so Object class must be polymorphic, hence the virtual destructor.
    virtual ~Object();
    static int generateUniqueID();
};