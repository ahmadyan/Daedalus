#include "object.h"

int Object::objectCount = 0;

Object::Object(){
    id = generateUniqueID();
}

Object::~Object(){
    
}

int Object::generateUniqueID(){
    return objectCount++ ;
}