#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;


GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetDir)
: GameWorld(assetDir)
{

}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    m_studentworld = this;
    m_aliensDestroyed = 0;
    m_aliensOnScreen = 0;
    
    for (int i = 0; i < 30; i++)
        m_actors.push_back(new Star(m_studentworld, randInt(0, VIEW_WIDTH-1), randInt(0, VIEW_HEIGHT-1))); //create 30 start stars
    
    m_nachenblaster = new NachenBlaster(m_studentworld); //create nachenblaster
    
    return GWSTATUS_CONTINUE_GAME;
}


int StudentWorld::move()
{
    //pushes new stars
    if (randInt(1,15) == 1)
        m_actors.push_back(new Star(m_studentworld, VIEW_WIDTH-1, randInt(0, VIEW_HEIGHT-1)));   //create random stars
    
    //create aliens
    if (makeNewAlien())
    {
        m_aliensOnScreen++;
        
        double S1 = 60;
        double S2 = 20 + getLevel() * 5;
        double S3 = 5 + getLevel() * 10;
        double S = S1 + S2 + S3;
        int chance = randInt(1, S);
        
        if (chance <= S1/S * 100)
            m_actors.push_back(new Smallgon(m_studentworld));
        else if (chance > S1/S * 100 && chance <= (S1/S) * 100 + (S2/S) * 100)
            m_actors.push_back(new Smoregon(m_studentworld));
        else
            m_actors.push_back(new Snagglegon(m_studentworld));
    }
    
    
    //moves actors
    list<Actor*>::iterator it;
    it = m_actors.begin();
    while (it != m_actors.end())
    {
        (*it)->doSomething();
        it++;
        
        if (m_nachenblaster->isDead())
        {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        
        if (m_aliensDestroyed == (6+ (4 * (getLevel()))))
        {
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    
    
    if (!m_nachenblaster->isDead())     //nachenblaster do something
        m_nachenblaster->doSomething();
    
    
    
    removeDeadGameObjects();
    
    
    //updates status text at top
    ostringstream oss;
    oss << "Lives: " << getLives() << "  Health: " << setw(3) << ((double)m_nachenblaster->getHealth() / 50) * 100;
    oss << "%  Score: " << getScore() << "  Level: " << getLevel() << "  Cabbages: " << setw(3) << m_nachenblaster->cabbagePct();
    oss << "%  Torpedoes: " << m_nachenblaster->numTorpedos();
    string s = oss.str();
    setGameStatText(s);
    
    
    
    return GWSTATUS_CONTINUE_GAME;
    
}

void StudentWorld::alienDestroyed(Actor* a, Actor* b)       //a is alien, b is nachenblaster/projectile
{
    playSound(SOUND_DEATH);
    a->setDead();
    m_aliensDestroyed++;
    if (b->getType() == 'n')
        getNachenblaster()->minusHealth(a->getDamage());
    addActor(new Explosion(this, a->getX(), a->getY()));
    increaseScore(a->getPoints());
    
    if (a->getType() == 'x')    //snagglegon
        if (randInt(1, 6) == 1)
            m_actors.push_back(new ExtraLifeGoodie(this, a->getX(), a->getY()));
    if (a->getType() == 'r')    //smoregon
        if (randInt(1, 3) == 1)
        {
            if (randInt(1 ,2) == 1)
                m_actors.push_back(new RepairGoodie(this, a->getX(), a->getY()));
            else
                m_actors.push_back(new TorpedoGoodie(this, a->getX(), a->getY()));
        }
}


bool StudentWorld::makeNewAlien() //determine if new alien needs to be created
{
    int D = m_aliensDestroyed;
    int T = 6.0 + (4.0 * getLevel());
    int R = T - D;
    int M = 4.0 + (0.5 * getLevel());
    
    return m_aliensOnScreen < min(M, R);
}




void StudentWorld::removeDeadGameObjects()
{
    
    list<Actor*>::iterator it = m_actors.begin();   //remove dead actors
    while (it != m_actors.end())
    {
        if ((*it)->isDead())
        {
            if ((*it)->isAlien())
                m_aliensOnScreen--;
            delete *it;
            it = m_actors.erase(it);
        }
        if (!m_actors.empty())
            it++;
    }
}


void StudentWorld::cleanUp()
{
    
    m_aliensDestroyed = 0;
    
    if (m_nachenblaster != nullptr)
        delete m_nachenblaster;
    m_nachenblaster = nullptr;
    
    list<Actor*>::iterator it = m_actors.begin();   //clear dead actors
    while (it != m_actors.end())
    {
        delete *it;
        it = m_actors.erase(it);
    }
    m_actors.clear();
    
}


void StudentWorld::pushProjectile(int startX, int startY, char type)
{
    if (type == 'c')    //cabbage
        m_actors.push_back(new Cabbage(m_studentworld, startX, startY));
    if (type == 'n')    //nachenblaster torpedo
        m_actors.push_back(new Torpedo(m_studentworld, startX + 12, startY, 0));
    if (type == 's')    //smoregon torpedo
        m_actors.push_back(new Torpedo(m_studentworld, startX - 14, startY, 180));
    if (type == 't')    //turnip
        m_actors.push_back(new Turnip(m_studentworld, startX, startY));
}


bool StudentWorld::playerInLineOfFire(const Actor* a)
{
    int y = m_nachenblaster->getY();
    if (a->getY() - y <= 4 && a->getY() - y >= -4 && a->getX() > m_nachenblaster->getX())
        return true;
    return false;
}

bool StudentWorld::collision(Actor* a, Actor* b)
{
    double x = a->getX() - b->getX();
    double y = a->getY() - b->getY();
    
    double euclideanDist = sqrt(pow(x, 2) + pow(y, 2));
    if (euclideanDist < .75 * (a->getRadius() + b->getRadius()))
        return true;
    
    return false;
}
