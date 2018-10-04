#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <list>
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    ~StudentWorld();
    void pushProjectile(int startX, int startY, char type);
    void alienDestroyed(Actor* a, Actor* b);
    bool playerInLineOfFire(const Actor* a);
    NachenBlaster* getCollidingPlayer(Actor* a)    //pass in projectile or alien or goodie
    {
        if (collision(m_nachenblaster, a))
            return m_nachenblaster;
        else
            return nullptr;
    }
    
    Actor* getOneCollidingAlien(Actor* a) //pass in projectile          //PUT THIS IN THE PROJECTILES
    {
        std::list<Actor*>::iterator it;
        it = m_actors.begin();
        while (it != m_actors.end())
        {
            if ((*it)->isAlien())
                if (collision(a, *it))
                    return *it;
            it++;
        }
        return nullptr;
    }
    
    NachenBlaster* getNachenblaster()
    {
        return m_nachenblaster;
    }
    
protected:
    virtual int init();
    virtual int move();
    void removeDeadGameObjects();
    virtual void cleanUp();
    void addActor(Actor *a)
    {
        m_actors.push_back(a);
    }
    bool makeNewAlien();
    bool collision(Actor* a, Actor* b);
    

private:
    StudentWorld* m_studentworld;

    NachenBlaster* m_nachenblaster;
    std::list<Actor*> m_actors;
    
    int m_aliensDestroyed;
    int m_aliensOnScreen;
    

};

#endif // STUDENTWORLD_H_
