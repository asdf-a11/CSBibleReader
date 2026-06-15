#pragma once
class MainClass;

class Menu{
public:
    Menu(){}
    virtual void Init(MainClass* ptr){}
    virtual void Update(MainClass* ptr){}
    virtual void Draw(MainClass* ptr){}
    virtual ~Menu() = default;
};