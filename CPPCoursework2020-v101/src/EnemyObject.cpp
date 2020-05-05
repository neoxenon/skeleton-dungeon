#include "header.h"
#include "EnemyObject.h"

#include "PlayerObject.h"
#include "StateRunning.h"


EnemyObject::EnemyObject(int xStart, int yStart, BaseEngine* pEngine, int width, int height, bool topleft,
	std::string name, std::string desc, int maxHealth, int maxStamina, int expDrop, int maxAttacks)
	: CharObject(xStart, yStart, pEngine, width, height, topleft),
	name(name), desc(desc), maxHealth(maxHealth), health(maxHealth), maxStamina(maxStamina), stamina(maxStamina), expDrop(expDrop), maxAttacks(maxAttacks), attacks(maxAttacks)
{
	
}

EnemyObject::~EnemyObject()
{
}

void EnemyObject::virtDoUpdate(int iCurrentTime)
{

	//Query AI if movement is finished
	if (currentState == CharState::stateWalk) {
		objMovement.calculate(iCurrentTime);
		m_iCurrentScreenX = objMovement.getX();
		m_iCurrentScreenY = objMovement.getY();

		if (objMovement.hasMovementFinished(iCurrentTime)) {
			currentState = CharState::stateIdle;

			m_iCurrentScreenX = objMovement.getX();
			m_iCurrentScreenY = objMovement.getY();

			//Snap to grid in case we strayed a couple pixels somehow
			m_iCurrentScreenX = std::round(m_iCurrentScreenX / TILE_SIZE) * TILE_SIZE;
			m_iCurrentScreenY = std::round(m_iCurrentScreenY / TILE_SIZE) * TILE_SIZE;

			//Redo order of depth for charobjects by height
			pEngine->orderCharsByHeight();

			std::cout << "pos = " << m_iCurrentScreenX << ',' << m_iCurrentScreenY << std::endl;

			AI();
		}
	}
	//Ensure that the objects get redrawn on the display
	redrawDisplay();
}

void EnemyObject::turnStart()
{
	//Enemy Turn Sound
	//pEngine->GetAudio()->playAudio("sfx/monsters/Growl.ogg", -1, 0);

	//Generate path to desired location
	PlayerObject* player = pEngine->GetPlayer();
	path = calcPath(player->getXPos(), player->getYPos());

	//Print path
	/*
	for (auto it = path.begin(); it != path.end(); it++) {
		std::cout << '(' << (*it)->x << ',' << (*it)->y << ')' << std::endl;
	}*/

	AI();
}

//ALL DEFAULT AI BEHAVIOUR HERE
//will attempt to move into attack range of player and then attack
void EnemyObject::AI()
{
	//If within range of weapon with attack(s) left, attack
	PlayerObject* player = pEngine->GetPlayer();
	if (attacks > 0 && lineOfSight(m_iCurrentScreenX, m_iCurrentScreenY, player->getXPos(), player->getYPos(), 2)) {
		attack();
		AI();
		return;

	}
	//Otherwise, if stamina left then move towards player (unless already next to them)
	else if (stamina > 0 && path.size() > 1 ) {

		//Initiate movement
		Node* nextMove = path.front();
		path.pop_front();

		//Print current move
		std::cout << '(' << nextMove->x << ',' << nextMove->y << ')' << std::endl;

		move((nextMove->x) - m_iCurrentScreenX, (nextMove->y) - m_iCurrentScreenY, getEngine()->getModifiedTime(), 400);

		return;
	}

	//Enemies will try and stay at their max attack range
	//Enemy will attack instead of moving if possible
	//If enemy runs out of attacks while inside their range, they just end their turn instead of moving closer


	//Turn is over if not moving or attacking

	//Reset stamina n attacks
	stamina = maxStamina;
	attacks = maxAttacks;

	//Signal to Engine that it's the next enemy's turn
	auto e = dynamic_cast<StateEnemyTurn*>(pEngine->getState());
	if (e) e->triggerNextEnemy();
	else
		std::cout << "Should be in Enemy Turn state, but we ain't. Something is terribly wrong <0__0>"; //ERROR TIME (wrong state)

	return;

}


std::list<Node*> EnemyObject::calcPath (int goalX, int goalY)
{
#define MAX_ITERATIONS 1000

	int its = 0;

	//Create and initialise lists
	std::list<Node*> open_list;
	std::list<Node*> closed_list;

	open_list.clear();
	closed_list.clear();

	//Push starting node to open list
	open_list.push_back(new Node(m_iCurrentScreenX, m_iCurrentScreenY, 0, 0, nullptr));

	//While open list not empty
	while (open_list.size() > 0) {

		//find lowest f value element in open list
		Node* lowest = open_list.front();
		for (std::list<Node*>::iterator it = open_list.begin(); it != open_list.end(); it++) {
			Node* current = *it;

			if (current->f < lowest->f) lowest = current;
		}

		//Remove lowest node from the list
		open_list.remove(lowest);

		//Add lowest node to closed list
		closed_list.push_back(lowest);

		//generate successors of q (one for each of the four possible directions)
		int x = lowest->x;
		int y = lowest->y;
		int g = lowest->g;

		std::queue<Node*> children;
		children.push(new Node(x + TILE_SIZE, y, g + TILE_SIZE, 0, lowest));
		children.push(new Node(x - TILE_SIZE, y, g + TILE_SIZE, 0, lowest));
		children.push(new Node(x, y + TILE_SIZE, g + TILE_SIZE, 0, lowest));
		children.push(new Node(x, y - TILE_SIZE, g + TILE_SIZE, 0, lowest));

		//for each successor
		while (children.size() > 0) {
			Node* child = children.front();
			children.pop();

			int x = child->x;
			int y = child->y;

			//calculate sucessor f
			int f = child->g + calcHeuristic(*child, goalX, goalY);
			child->f = f;

			//if successor is goal, stop here
			if (x == goalX && y == goalY) {
				//Return a list containing the full path
				std::list<Node*> path;

				Node* current = child;
				while (current != nullptr) {
					path.push_back(current);
					current = current->parent;
				}
				path.reverse();
				return path;

			}

			//If a solid tile exist here, skip
			SolidTileManager* tiles = pEngine->GetTilesSolid();
			if (tiles->isValidTilePosition(x, y)) {
				if (tiles->getMapValue(tiles->getMapXForScreenX(x), tiles->getMapYForScreenY(y)) != 0) {
					continue;
				}
			}

			bool skip = false;

			//If a CharObject exists here, skip
			DisplayableObject* pObj;
			for (int i = 0; i < pEngine->getContentCount(); i++) {
				if ((pObj = pEngine->getDisplayableObject(i)) == NULL) continue; //skip null objects
				CharObject* pChar = dynamic_cast<CharObject*> (pObj);
				if (pChar && pChar->getXPos() == x && pChar->getYPos() == y) skip = true;
			}
			if (skip) continue;

			//if node with same <x,y> as successor exists in open_list with a lower f than successor, then skip
			for (std::list<Node*>::iterator it = open_list.begin(); it != open_list.end(); it++) {
				if ((*it) == child && (*it)->f < f) skip = true;
			}
			
			//if node with same <x,y> as successor exists in closed_list with a lower f than successor, then skip
			for (std::list<Node*>::iterator it = closed_list.begin(); it != closed_list.end(); it++) {
				if ((*it) == child && (*it)->f < f) skip = true;
			}
			
			//else add to open_list
			if (skip) continue;
			else open_list.push_back(child);			

		}

		//Break out of an infinite loop at some point
		if (++its > MAX_ITERATIONS) break;
	
	}

	std::cout << "Error: Could not find route" << std::endl;

}

//Using Manhattan Distance - aka. total distance travelled using right-angles only, since no diagonal moves are permitted
int EnemyObject::calcHeuristic(Node n, int goalX, int goalY)
{
	return std::abs(n.x - goalX) + std::abs(n.y - goalY);
}

void EnemyObject::damage(int amount)
{
	health -= amount;
	std::cout << "Enemy " << name << " took " << amount << " damage, has " << health << " health left.\n";

	//DEBUG: do line of sight check
	//lineOfSight(64, 64, 256, 192, 3);

	//DEAD
	if (health <= 0) {
		std::cout << "Enemy " << name << " was killed.\n";
		getEngine()->removeDisplayableObject(this);
		delete this;
	}
}

//Override this where necessary
void EnemyObject::attack()
{
	attacks--;

	std::cout << "Enemy attack";
	pEngine->health--;
}

void EnemyObject::move(int xmove, int ymove, int currentTime, int time)
{
	stamina--;

	CharObject::move(xmove, ymove, currentTime, time);
}