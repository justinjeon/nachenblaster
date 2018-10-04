#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameWorld.h"

const int HIT_BY_SHIP = 0;
const int HIT_BY_PROJECTILE = 1;

class StudentWorld;
class Alien;
class NachenBlaster;
class LivingActor;

///ACTOR

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* studentworldptr, int imageID, double startX, double startY, int dir, double size, int depth);
    virtual void doSomething() = 0;
    virtual bool isAlien() const;
    void setDead();
    bool isDead();
    char getType()
    {
        return m_type;
    }
    
    int getDamage()
    {
        return m_damage;
    }
    
    int getPoints()
    {
        return m_points;
    }
    
protected:
    bool isOffScreen();

    
    void setPoints(int points)
    {
        m_points = points;
    }
    
    
    void setDamage(int damage)
    {
        m_damage = damage;
    }

    
    void setType(char type)
    {
        m_type = type;
    }
    
    StudentWorld* getWorld();

    bool initialConditions()    //is dead or is off screen
    {
        if (isOffScreen() || isDead())
        {
            setDead();
            return true;
        }
        return false;
    }

    virtual void checkCollide(Actor* a, Actor* b);  //handles what happens to each object when they collide
    
private:
    bool m_dead;
    int m_points;
    int m_damage;
    char m_type;
    StudentWorld* m_studentworld;
};


class Star : public Actor
{
public:
    Star(StudentWorld* studentworldptr, double startX, double startY);
private:
    virtual void doSomething();
};


class Explosion : public Actor
{
public:
    Explosion(StudentWorld* studentworldptr, double startX, double startY);
private:
    virtual void doSomething();
    double m_ticks;
};


/////////////////////////////////////////PROJECTILES

class Projectile : public Actor
{
public:
    Projectile(StudentWorld* studentworldptr, int damage, int imageID, double startX, double startY, int dir, double size, int depth);
    int getDamage();
private:
    virtual void doSomething();
    virtual void moveProj() = 0;
    virtual void checkProjCollision();
    int m_damage;
    
};


class Cabbage : public Projectile
{
public:
    Cabbage(StudentWorld* studentworldptr, double startX, double startY);
private:
    virtual void moveProj();
    
};


class Turnip : public Projectile
{
public:
    Turnip(StudentWorld* studentworldptr, double startX, double startY);
private:
    virtual void checkProjCollision();
    virtual void moveProj();
};


class Torpedo : public Projectile
{
public:
    Torpedo(StudentWorld* studentworldptr, double startX, double startY, int dir);
private:
    virtual void checkProjCollision();
    virtual void moveProj();
};


///////////////////////////////////////////////LIVING ACTOR


class LivingActor : public Actor
{
public:
    LivingActor(StudentWorld* studentworldptr, int health, int imageID, double startX, double startY, int dir, double size, int depth);
    void setHealth(int health);
    int getHealth();
    void minusHealth(double amt);
protected:
    void addHealth(int health);
    virtual bool initialConditions()
    {
        if (isOffScreen() || isDead() || m_health <= 0)
        {
            setDead();
            return true;
        }
        return false;
    }
    
private:
    int m_health;

};



class NachenBlaster: public LivingActor
{
public:
    NachenBlaster(StudentWorld* studentworldptr);
    virtual void doSomething();
    int cabbagePct()
    {
        return ((double)m_cabbageEnergy / 30) * 100;
    }
    int numTorpedos()
    {
        return m_torpedos;
    }
    void increaseTorpedos()
    {
        m_torpedos += 5;
    }
private:
    int m_cabbageEnergy;
    int m_torpedos;
};



/////ALIENS

class Alien : public LivingActor
{
public:
    Alien(StudentWorld* studentworldptr, int health, int imageID, int dir, double size, int depth, int flightplan, double m_speed);
    virtual bool isAlien() const;
protected:
    virtual void doSomething();
    virtual void setNewFlightPath()
    {
        if (isFlightPlanZero() || getY() >= VIEW_HEIGHT - 2 || getY() <= 1)
        {
            if (getY() >= VIEW_HEIGHT - 2)
                setFlightPlanDir(-1.0);
            else if (getY() <= 1)
                setFlightPlanDir(1.0);
            else if (isFlightPlanZero())
            {
                setFlightPlanDir(randInt(-1, 1));
                setFlightPlanSize(randInt(1, 32));
            }
        }
    }
    void setFlightPlanSize(int flightplan)
    {
        m_flightPlanSize = flightplan;
    }
    void reduceFlightPlanSize()
    {
        if (m_flightPlanSize > 0)
            m_flightPlanSize--;
    }
    bool isFlightPlanZero()
    {
        if (m_flightPlanSize <= 0)
            return true;
        return false;
    }
    
    void setFlightPlanDir(int i)
    {
        if (i >= -1 && i <= 1)
            m_flightPlanDir = i;
    }
    
    double getFlightPlanDir()
    {
        return m_flightPlanDir;
    }
    
    void setSpeed(double speed)
    {
        m_speed = speed;
    }

    
private:
    double m_flightPlanDir;
    int m_flightPlanSize;
    double m_speed;
    virtual void possiblyShoot() = 0;
};



class Smallgon : public Alien
{
public:
    Smallgon(StudentWorld* studentworldptr);
private:
    void possiblyShoot();
};


class Smoregon : public Alien
{
public:
    Smoregon(StudentWorld* studentworldptr);
private:
    void possiblyShoot();
};


class Snagglegon : public Alien
{
public:
    Snagglegon(StudentWorld* studentworldptr);
protected:
    virtual void setNewFlightPath()
    {
        if (isFlightPlanZero() || getY() >= VIEW_HEIGHT - 3 || getY() <= 2)
        {
            if (getY() >= VIEW_HEIGHT - 3)
                setFlightPlanDir(-1.0);
            else if (getY() <= 2)
                setFlightPlanDir(1.0);
            else if (isFlightPlanZero())
            {
                setFlightPlanDir(randInt(-1, 1));
                setFlightPlanSize(randInt(1, 32));
            }
        }
    }
private:
    void possiblyShoot();
};



///GOODIES

class Goodie : public Actor
{
public:
    Goodie(StudentWorld* w, double startX, double startY, int imageID);
    virtual void doGoodieThing() = 0;
protected:
    virtual void doSomething();
};

class ExtraLifeGoodie : public Goodie
{
public:
    ExtraLifeGoodie(StudentWorld* w, double startX, double startY);
protected:
    virtual void doGoodieThing();
};


class RepairGoodie : public Goodie
{
public:
    RepairGoodie(StudentWorld* w, double startX, double startY);
protected:
    virtual void doGoodieThing();
};

class TorpedoGoodie : public Goodie
{
public:
    TorpedoGoodie(StudentWorld* w, double startX, double startY);
protected:
    virtual void doGoodieThing();
};


#endif // ACTOR_H_
