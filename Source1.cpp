#include <SFML/Graphics.hpp>
#include <math.h>
#include <iostream>

using namespace std;

const float EPS = 20;
const int height = 1080;
const int width = 1920;
const float speed = 0.8;


struct Rect
{
	float x1;
	float y1;   // 1 - upper
	float x2;
	float y2;	// 2 - lower
};

struct Proximity
{
	float left;
	float right;
	float up;
	float down;
};

struct Door
{
	//Room *room; // телепорт
	Rect d;
	sf::RectangleShape door;
	sf::Texture texture;
};

bool left_contact(Rect rect, Rect charact, float length, float* prox);
bool right_contact(Rect rect, Rect charact, float length, float* prox);
bool up_contact(Rect rect, Rect charact, float length, float* prox);
bool down_contact(Rect rect, Rect charact, float length, float* prox);
bool contact(Rect rect, Rect charact);
bool is_equal(float a, float b);

class Room
{
public:
	Rect *obstacle;
	sf::RectangleShape *obst;
	int obs_num;
	sf::Texture *texture;
	
	Door *door;
	Room *room;
	int door_num;

	Room(int obs, int doors, /*Room* rm[],*/ float x1[], float y1[], float x2[], float y2[])
	{
		obstacle = new Rect[obs];
		obst = new sf::RectangleShape[obs];
		texture = new sf::Texture[obs];
		door = new Door[doors];
		obs_num = obs;
		door_num = doors;

		for (int i = 0; i < obs; i++)
		{
			obstacle[i].x1 = x1[i];
			obstacle[i].y1 = y1[i];
			obstacle[i].x2 = x2[i];
			obstacle[i].y2 = y2[i];
			float t1 = obstacle[i].x2 - obstacle[i].x1;
			float t2 = obstacle[i].y2 - obstacle[i].y1;
			obst[i].setSize(sf::Vector2f(t1, t2));
			if(t1 >= t2)
				texture[i].loadFromFile("block.jpg", sf::IntRect(0, 0, t1, t2));
			else
				texture[i].loadFromFile("block2.jpg", sf::IntRect(0, 0, t1, t2));
			obst[i].setTexture(&texture[i]);
			obst[i].setPosition(obstacle[i].x1, obstacle[i].y1);
		}

		for (int i = 0; i < doors; i++)
		{
			door[i].d.x1 = x1[i + obs_num];
			door[i].d.y1 = y1[i + obs_num];
			door[i].d.x2 = x2[i + obs_num];
			door[i].d.y2 = y2[i + obs_num];
			float t1 = door[i].d.x2 - door[i].d.x1;
			float t2 = door[i].d.y2 - door[i].d.y1;
			door[i].door.setSize(sf::Vector2f(t1, t2));
			door[i].texture.loadFromFile("portal.jpg", sf::IntRect(0, 0, t1, t2));
			door[i].door.setTexture(&door[i].texture);
			door[i].door.setPosition(door[i].d.x1, door[i].d.y1);
			//door[i].room = rm[i];
		}
	}

};
/*
class Map
{
public:
	Room *current_room;
	Room **room;
	int room_num;
	Map(int rn, Room *rm[]) 
	{
		room_num = rn;
		room = new Room *[rn];
		for (int i = 0; i < rn; i++)
		{
			*room[i] = *rm[i];
		}

	}
};
*/
class My_character
{
public:
	sf::RectangleShape my_char;
	Rect mychar;
	float length;
	sf::Texture texture;
	Proximity obs_prox;

	My_character(float len, float x0, float y0)
	{
		length = len;
		mychar.x1 = x0;
		mychar.y1 = y0;
		mychar.x2 = x0 + len;
		mychar.y2 = y0 + len;
		my_char.setSize(sf::Vector2f(len, len));
		obs_prox.left = 0;
		obs_prox.right = 0;
		obs_prox.up = 0;
		obs_prox.down = 0;

		texture.loadFromFile("mychar1.png");
		//my_char.setFillColor(sf::Color(100, 50, 250));
		my_char.setTexture(&texture); // texture is a sf::Texture
		//my_char.setTextureRect(sf::IntRect(0, 0, 200, 200));
		my_char.setPosition(mychar.x1, mychar.y1);
	}

	void Move(Room room, float dist);
	void Act();

	bool Left_contact(Room room);
	bool Right_contact(Room room);
	bool Up_contact(Room room);
	bool Down_contact(Room room);
	int Door_contact(Room room);
};


int main()
{
	sf::RenderWindow window(sf::VideoMode(width, height), "RectangleTALE");

	sf::Clock clock;
	float time;
	window.setVerticalSyncEnabled(true);
	
	
	float x11[] = { 800, 1100, 100 , 0};
	float y11[] = { 100, 500, 600, 0};
	float x21[] = { 1200, 1200, 400, 200};
	float y21[] = { 200, 750, 700, 200 };

	//Room* d[1] = {};
	
	Room *room = &Room(3, 1, x11, y11, x21, y21);
	
	/*
	float x12[] = { 800, 0, 100 };
	float y12[] = { 100, 0, 600 };
	float x22[] = { 1200, 200, 400 };
	float y22[] = { 200, 200, 700 };
	
	Room *room2 = &Room(3, 1, d, x12, y12, x22, y22);

	
	float x13[] = { 800, 1100, 100 , 0 };
	float y13[] = { 100, 500, 600, 0 };
	float x23[] = { 1200, 1200, 400, 200 };
	float y23[] = { 200, 750, 700, 200 };

	Room *room3 = &Room(3, 1, x12, y12, x22, y22);
	*/
	My_character my_char = My_character(200, 333, 333);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed ||
				(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
				window.close();
		}

		window.clear();

		time = clock.getElapsedTime().asMilliseconds();
		clock.restart();

		my_char.Move(*room, speed * time);

		for(int i = 0; i < room->obs_num; i++)
			window.draw(room->obst[i]);

		for (int i = 0; i < room->door_num; i++)
			window.draw(room->door[i].door);

		window.draw(my_char.my_char);

		window.display();

	}

	return 0;
}


void My_character::Move(Room room, float dist)
{

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		if (!Up_contact(room) && !is_equal(0, mychar.y1))
		{
			mychar.y1 -= dist;
			mychar.y2 -= dist;
		}
		else
		{
			mychar.y1 -= obs_prox.up;
			mychar.y2 -= obs_prox.up;
			obs_prox.up = 0;
		}
		my_char.setPosition(mychar.x1, mychar.y1);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		if (!Down_contact(room) && !is_equal(height, mychar.y2))
		{
			mychar.y1 += dist;
			mychar.y2 += dist;
		}
		else
		{
			mychar.y1 += obs_prox.down;
			mychar.y2 += obs_prox.down;
			obs_prox.down = 0;
		}
		my_char.setPosition(mychar.x1, mychar.y1);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		if (!Left_contact(room) && !is_equal(0, mychar.x1))
		{
			mychar.x1 -= dist;
			mychar.x2 -= dist;
		}
		else
		{
			mychar.x1 -= obs_prox.left;
			mychar.x2 -= obs_prox.left;
			obs_prox.left = 0;
		}
		my_char.setPosition(mychar.x1, mychar.y1);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		if (!Right_contact(room) && !is_equal(width, mychar.x2))
		{
			mychar.x1 += dist;
			mychar.x2 += dist;
		}
		else
		{
			mychar.x1 += obs_prox.right;
			mychar.x2 += obs_prox.right;
			obs_prox.right = 0;
		}
		my_char.setPosition(mychar.x1, mychar.y1);
	}
	sf::RectangleShape my_char(sf::Vector2f(mychar.x2 - mychar.x1, mychar.y2 - mychar.y1));
	my_char.setPosition(mychar.x1, mychar.y1);
	my_char.setFillColor(sf::Color(100, 50, 250));

}
void My_character::Act()
{

}

bool My_character::Left_contact(Room room)
{
	for (int i = 0; i < room.obs_num; i++)
	{
		if (left_contact(room.obstacle[i], mychar, length, &obs_prox.left))
			return true;
	}
	return false;
}

bool My_character::Right_contact(Room room)
{
	for (int i = 0; i < room.obs_num; i++)
	{
		if (right_contact(room.obstacle[i], mychar, length, &obs_prox.right))
			return true;
	}
	return false;
}

bool My_character::Up_contact(Room room)
{
	for (int i = 0; i < room.obs_num; i++)
	{
		if (up_contact(room.obstacle[i], mychar, length, &obs_prox.up))
			return true;
	}
	return false;
}

bool My_character::Down_contact(Room room)
{
	for (int i = 0; i < room.obs_num; i++)
	{
		if (down_contact(room.obstacle[i], mychar, length, &obs_prox.down))
			return true;
	}
	return false;
}

int My_character::Door_contact(Room room) 
{
	for (int i = 0; i < room.door_num; i++)
		if (contact(room.door[i].d, mychar))
			return i;
	return 0;
}

bool left_contact(Rect rect, Rect charact, float length, float* prox)
{
	if (is_equal(rect.x2, charact.x1) && rect.y2 > charact.y1 && charact.y1 + length > rect.y1)
	{
		*prox = charact.x1 - rect.x2;
		return true;
	}
	else
		return false;
}


bool right_contact(Rect rect, Rect charact, float length, float* prox)
{
	if (is_equal(rect.x1, charact.x2) && rect.y2 > charact.y1 && charact.y1 + length > rect.y1)
	{
		*prox = rect.x1 - charact.x2;
		return true;
	}
	else
		return false;
}

bool up_contact(Rect rect, Rect charact, float length, float* prox)
{
	if (is_equal(rect.y2, charact.y1) && rect.x2 > charact.x1 && charact.x1 + length > rect.x1)
	{
		*prox = charact.y1 - rect.y2;
		return true;
	}
	else
		return false;
}

bool down_contact(Rect rect, Rect charact, float length, float* prox)
{
	if (is_equal(rect.y1, charact.y2) && rect.x2 > charact.x1 && charact.x1 + length > rect.x1)
	{
		*prox = rect.y1 - charact.y2;
		return true;
	}
	else
		return false;
}

bool contact(Rect rect, Rect charact)
{
	if ((charact.x1 > rect.x1 && charact.x1 < rect.x2 && charact.y1 > rect.y1 && charact.y1 < rect.y2) || 
		(charact.x2 > rect.x1 && charact.x2 < rect.x2 && charact.y2 > rect.y1 && charact.y2 < rect.y2))
		return true;
	else
		return false;
}

bool is_equal(float a, float b)
{
	if (abs(a - b) < EPS)
		return true;
	else
		return false;
}