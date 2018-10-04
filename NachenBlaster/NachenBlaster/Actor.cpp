#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp





//ACTOR CODE

Actor::Actor(StudentWorld* studentworldptr, int imageID, double startX, double startY, int dir, double size, int depth) : GraphObject(imageID, startX, startY, dir, size, depth)
{
    m_studentworld = studentworldptr;
    m_dead = false;
    
}

bool Actor::isAlien() const
{
    return false;
}

void Actor::setDead()
{
    m_dead = true;
}

bool Actor::isDead()
{
    return m_dead;
}

bool Actor::isOffScreen()
{
    if (getX() < 0 || getX() >= VIEW_WIDTH || getY() < 0 || getY() >= VIEW_WIDTH)
        return true;
    return false;
}

StudentWorld* Actor::getWorld()
{
    return m_studentworld;
}


void Actor::checkCollide(Actor* a, Actor* b)
{
    if (a == nullptr || b == nullptr)
        return;
    
    if (!a->isDead() && a->isAlien() && b == getWorld()->getNachenblaster())       //a is alien, b is nachenblaster
    {
        getWorld()->alienDestroyed(a, b);
    }
    else if ((a->getType() == 't' || a->getType() == 'a') && !b->isAlien()) //a is alien projectile, b is nachenblaster
    {
        a->setDead();
        static_cast<LivingActor*>(b)->minusHealth(a->getDamage());
        getWorld()->playSound(SOUND_BLAST);
    }
    else if ((a->getType() == 'c' || a->getType() == 'p') && b->isAlien())    //a is nachenblaster projectile, b is alien
    {
        a->setDead();
        static_cast<LivingActor*>(b)->minusHealth(a->getDamage());
        if (b->isDead())
        {
            getWorld()->alienDestroyed(b, a);
            return;
        }
        getWorld()->playSound(SOUND_BLAST);
    }
    else if (a->getType() == 'g' && b == getWorld()->getNachenblaster())  //a is goodie, b is nachenblaster
    {
        a->setDead();
        getWorld()->increaseScore(100);
        getWorld()->playSound(SOUND_GOODIE);
        static_cast<Goodie*>(a)->doGoodieThing();
    }
}



                

//EXPLOSION

Explosion::Explosion(StudentWorld* studentworldptr, double startX, double startY) : Actor(studentworldptr, IID_EXPLOSION, startX, startY, 0, 1, 0)
{
    m_ticks = 0;
}

void Explosion::doSomething()
{
    if (m_ticks == 4)
    {
        setDead();
        return;
    }
    if (m_ticks != 0)
        setSize(1.5 * getSize());
    m_ticks++;
}




//STAR CODE

Star::Star(StudentWorld* studentworldptr, double startX, double startY) : Actor(studentworldptr,IID_STAR, startX, startY, 0, ((double)randInt(5, 50)/100), 3)
{
}

void Star::doSomething()
{
    this->moveTo(this->getX()-1, this->getY());
    if (getX() <= -1)
        setDead();
}





//LIVINGACTOR CODE

LivingActor::LivingActor(StudentWorld* studentworldptr, int health, int imageID, double startX, double startY, int dir, double size, int depth) : Actor(studentworldptr, imageID, startX, startY, dir, size, depth)
{
    m_health = health;
}

void LivingActor::setHealth(int health)
{
    m_health = health;
}

void LivingActor::addHealth(int health)
{
    m_health += health;
}

int LivingActor::getHealth()
{
    return m_health;
}
    
void LivingActor::minusHealth(double amt)
{
    setHealth((getHealth() - amt));
    if (m_health <= 0)
        setDead();
}



////////////////////////////NACHENBLASTER//////////////////////////////////////////////



//NACHENBLASTER CODE

NachenBlaster::NachenBlaster(StudentWorld* studentworldptr) : LivingActor(studentworldptr, 50, IID_NACHENBLASTER, 0, 128, 0, 1.0, 0)
{
    m_cabbageEnergy = 30;
    m_torpedos = 0;
    setType('n');
}


void NachenBlaster::doSomething()
{
    if (initialConditions())
        return;
    
    
    checkCollide(getWorld()->getOneCollidingAlien(this), this);
    if (isDead())
        return;
    
    if (m_cabbageEnergy < 30)
        m_cabbageEnergy++;
        
    int ch;
    if (getWorld()->getKey(ch))
    {
        switch(ch)
        {
        case KEY_PRESS_LEFT:
            if (getX() >= 6)
                moveTo(getX()-6, getY());
            break;
        case KEY_PRESS_RIGHT:
            if (getX() <= VIEW_WIDTH - 6)
                moveTo(getX()+6, getY());
            break;
        case KEY_PRESS_UP:
            if (getY() <= VIEW_HEIGHT - 6)
                moveTo(getX(), getY()+6);
            break;
        case KEY_PRESS_DOWN:
            if (getY() >= 6)
                moveTo(getX(), getY()-6);
            break;
        case KEY_PRESS_SPACE:
            if (m_cabbageEnergy >= 5)
            {
                m_cabbageEnergy -=5;
                getWorld()->pushProjectile(getX(), getY(), 'c');
            }
            break;
        case KEY_PRESS_TAB:
            if (m_torpedos > 0)
            {
                m_torpedos--;
                getWorld()->pushProjectile(getX(), getY(), 'n');
            }
            break;
        }
    }
    
    checkCollide(getWorld()->getOneCollidingAlien(this), this);
    if (isDead())
        return;
}




////////////////////////////PROJECTILES


Projectile::Projectile(StudentWorld* studentworldptr, int damage, int imageID, double startX, double startY, int dir, double size, int depth) : Actor(studentworldptr, imageID, startX, startY, dir, size, depth)
{
    m_damage = damage;
}

int Projectile::getDamage()
{
    return m_damage;
}




void Projectile::doSomething()
{
    if (initialConditions())
        return;
    checkProjCollision();
    if (isDead())
        return;
    moveProj();
    if (isDead())
        return;
    checkProjCollision();
}




void Projectile::checkProjCollision()
{
    checkCollide(this, getWorld()->getOneCollidingAlien(this));
}



//cabbage

Cabbage::Cabbage(StudentWorld* studentworldptr, double startX, double startY) : Projectile(studentworldptr, 2, IID_CABBAGE, startX + 12, startY, 0, .5, 1)
{
    getWorld()->playSound(SOUND_PLAYER_SHOOT);
    setType('c');
    setDamage(2);
    setPoints(0);
}

void Cabbage::moveProj()
{
    moveTo(getX() + 8, getY());
    setDirection(getDirection()+20);
}


//turnip

Turnip::Turnip(StudentWorld* studentworldptr, double startX, double startY) : Projectile(studentworldptr, 2, IID_TURNIP, startX - 14, startY, 0, .5, 1)
{
    getWorld()->playSound(SOUND_ALIEN_SHOOT);
    setType('t');
    setDamage(2);
    setPoints(0);
}

void Turnip::moveProj()
{
    moveTo(getX() - 6, getY());
    setDirection(getDirection() + 20);
}

void Turnip::checkProjCollision()
{
    checkCollide(this, getWorld()->getCollidingPlayer(this));
}


//torpedo

Torpedo::Torpedo(StudentWorld* studentworldptr, double startX, double startY, int dir) : Projectile(studentworldptr, 8, IID_TORPEDO, startX, startY, dir, .5, 1)
{
    getWorld()->playSound(SOUND_TORPEDO);
    if (dir == 180)
        setType('a');
    else
        setType('p');
    setDamage(8);
    setPoints(0);
    
    //dir is 180 for snagglegon, 0 for nachenblaster
    //startX is getX() + 12 for nachenblaster, getX() - 14 for snagglegon
}

void Torpedo::moveProj()
{
    if (getDirection() == 0)
        moveTo(getX() + 8, getY());
    if (getDirection() == 180)
        moveTo(getX() - 8, getY());
}


void Torpedo::checkProjCollision()
{
    if (getType() == 'a')
        checkCollide(this, getWorld()->getCollidingPlayer(this));//check collision and check who it was fired by (check direction)
    else if (getType() == 'p')
        checkCollide(this, getWorld()->getOneCollidingAlien(this));
}





//////ALIENS

Alien::Alien(StudentWorld* studentworldptr, int health, int imageID, int dir, double size, int depth, int flightplan, double speed) : LivingActor(studentworldptr, health, imageID, VIEW_WIDTH-1, randInt(0, VIEW_HEIGHT-1), dir, size, depth)
{
    m_flightPlanSize = flightplan;
    m_flightPlanDir = 0.0;
    m_speed = speed;
}

bool Alien::isAlien() const
{
    return true;
}

void Alien::doSomething()
{
    if (initialConditions())
        return;
    
    setNewFlightPath();
    
    possiblyShoot();
    
    moveTo(((double)getX() - m_speed), (double)getY() + (double)((double)getFlightPlanDir() * m_speed));
    
    reduceFlightPlanSize();
}



//smallgons

Smallgon::Smallgon(StudentWorld* studentworldptr) : Alien(studentworldptr, 1000000, IID_SMALLGON, 0, 1.5, 1, 0, 2.0)
{
    setHealth(5 * (1 + (getWorld()->getLevel() - 1) * .1));
    setPoints(250);
    setDamage(5);
}

void Smallgon::possiblyShoot()
{
    if (getWorld()->playerInLineOfFire(this))
    {
        if (randInt(1, (20/getWorld()->getLevel())+5) == 1)
        {
            getWorld()->pushProjectile(getX(), getY(), 't');
            return;
        }
    }
}





//smoregon

Smoregon::Smoregon(StudentWorld* studentworldptr) : Alien(studentworldptr, 1000000, IID_SMOREGON, 0, 1.5, 1, 0, 2.0)
{
    setHealth(5 * (1 + (getWorld()->getLevel() - 1) * .1));
    setPoints(250);
    setDamage(5);
    setType('r');
}

void Smoregon::possiblyShoot()
{
    if (getWorld()->playerInLineOfFire(this))
    {
        if (randInt(1, (20/getWorld()->getLevel())+5) == 1)
        {
            getWorld()->pushProjectile(getX(), getY(), 't');
            return;
        }
        
        if (randInt(1, (20/getWorld()->getLevel())+5) == 1)
        {
            setFlightPlanDir(0);
            setFlightPlanSize(VIEW_WIDTH);
            setSpeed(5.0);
        }
    }
}




//snagglegon

Snagglegon::Snagglegon(StudentWorld* studentworldptr) : Alien(studentworldptr, 1000000, IID_SNAGGLEGON, 0, 1.5, 1, 500, 1.75)
{
    setHealth(10 * (1 + (getWorld()->getLevel() - 1) * .1));
    setSpeed(1.75);
    setFlightPlanDir(-1);
    setPoints(1000);
    setDamage(15);
    setType('x');
}

void Snagglegon::possiblyShoot()
{
    if (getWorld()->playerInLineOfFire(this))
    {
        if (randInt(1, (15/getWorld()->getLevel())+10) == 1)
        {
            getWorld()->pushProjectile(getX(), getY(), 's');
            return;
        }
    }
}



////GOODIES



Goodie::Goodie(StudentWorld* studentworldptr, double startX, double startY, int imageID) : Actor(studentworldptr, imageID, startX, startY, 0, .5, 1)
{
    setPoints(100);
    setDamage(0);
    setType('g');
}


void Goodie::doSomething()
{
    if (initialConditions())
        return;
    
    checkCollide(this, getWorld()->getCollidingPlayer(this));
    if (isDead())
        return;
    
    moveTo(getX() - .75, getY() - .75);
    
    checkCollide(this, getWorld()->getCollidingPlayer(this));
    
    if (isDead())
        return;
    
}



//extra life goodie

ExtraLifeGoodie::ExtraLifeGoodie(StudentWorld* studentworldptr, double startX, double startY) : Goodie(studentworldptr, startX, startY, IID_LIFE_GOODIE)
{
}

void ExtraLifeGoodie::doGoodieThing()
{
    getWorld()->incLives();
}



//repair goodie

RepairGoodie::RepairGoodie(StudentWorld* studentworldptr, double startX, double startY) : Goodie(studentworldptr, startX, startY, IID_REPAIR_GOODIE)
{
}

void RepairGoodie::doGoodieThing()
{
    if (getWorld()->getNachenblaster()->getHealth() >= 40)
        getWorld()->getNachenblaster()->setHealth(50);
    else
        getWorld()->getNachenblaster()->setHealth(getWorld()->getNachenblaster()->getHealth()+10);
}



//torpedo goodie

TorpedoGoodie::TorpedoGoodie(StudentWorld* studentworldptr, double startX, double startY) : Goodie(studentworldptr, startX, startY, IID_TORPEDO_GOODIE)
{
}

void TorpedoGoodie::doGoodieThing()
{
    getWorld()->getNachenblaster()->increaseTorpedos();
}











