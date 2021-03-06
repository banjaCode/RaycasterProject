#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <list>
#include <iterator>
#include <vector>
#include <algorithm>
#include <thread>

#define PI  3.14159265358979323846
#define PI2 PI/2
#define PI3 PI + PI2
#define DR 0.0174532925

using namespace std;

std::unique_ptr<olc::Sprite> sprTile;

struct Wall_Values
{
	float angle = 0;
	float offset = 0;
	int width = 1;
};

struct rgb_color
{
	int r, g, b;
	string name;
};

struct Player_Values
{
	float x, y, z = 0;						                        //spelarens x och y position
	float angle;                                                    // angle of the caracter
	float previous_x, previous_y;									//spelarens position förra framen
	olc::vf2d velocity;          									//spelarens hastighet
	float velocity_angle;
	float width;													//spelare bred för kolision 
	float moved_Distance = 0;										// x value of sin in walking animation
	float xSensitivity = 0.5, ySensitivity = 200;				    // sensitivity of mouse
	float movement_speed;											// movment speed
	bool walk_animation = false;									// desides if the walking animation is on
	float walk_animation_speed, walk_animation_waveLength;			//  1: speed of sin wave in walking animation  2:amplituden på sin vågen
	int foW;
};

struct ray_Values
{
	float disT;
	float x, y;
	int mapPos, mapPosY, mapPosX;
	int kontrast = 0;
};

struct list_rayPoints_Values 
{
	vector <ray_Values> list;
	int orderInZ;
	int orderInArray;
};

bool compare(const ray_Values& first, const ray_Values& second)
{
	if (first.disT > second.disT)
		return true;
	else
		return false;
}

bool compare2(const list_rayPoints_Values& first, const list_rayPoints_Values& second)
{
	if (first.orderInArray >= second.orderInArray)
		return true;
	else
		return false;
}

class Example : public olc::PixelGameEngine
{

	olc::vf2d pMouseCoordinates;
	Player_Values player;
	Wall_Values wall;
	
	const static int mapS = 64 / 2;
	const static int mapWidth = 16, mapHeight = 16, mapS2 = mapWidth * mapHeight;
	const static int layers = 1;
	int map[layers][mapS2] =
	{
		{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
		0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
		0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
		0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
		0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
		0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
		0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,
		0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
		0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
		0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
		0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		},
	};

	// [ Draw player on 2d map ]
	void Drawplayer()
	{
		FillCircle(player.x, player.y, player.width / 2, olc::YELLOW);
		DrawLinePro(player.x, player.y, player.x + cos(player.angle) * 30, player.y + sin(player.angle) * 30, 5, olc::DARK_GREEN);
	}

	// [ Draws 2D map ]
	void DrawMap2D()
	{
		olc::Pixel color;
		//x offset, y offset
		int x, y, xo, yo;
		for (y = 0; y < mapHeight; y++)
		{
			for (x = 0; x < mapWidth;x++)
			{
				if (map[0][y * mapWidth + x] > 0) { color = olc::WHITE; }
				else { color = olc::BACK;; }
				xo = x * mapS ; yo = y * mapS;
				FillRect(xo + 3, yo + 3, mapS - 3, mapS  - 3, color);
			}
		}
	}

	// [ Button controls ]
	void buttons() {
		// Time Correctiion
		float tc = GetElapsedTime();
		player.previous_x = player.x;
		player.previous_y = player.y;

		// -- VERTICAL AND HORIZONTAL MOVEMENT ON 2D MAP --
		player.velocity_angle = 0;

		if (GetKey(olc::Key::A).bHeld) { player.velocity_angle = - PI2;  }
		if (GetKey(olc::Key::D).bHeld) { player.velocity_angle = + PI2;  }
		if (GetKey(olc::Key::W).bHeld) { player.velocity_angle +=  0;    }
		if (GetKey(olc::Key::S).bHeld) { player.velocity_angle += PI;    }

		if (GetKey(olc::Key::W).bHeld && (GetKey(olc::Key::D).bHeld || GetKey(olc::Key::A).bHeld))
		{
			player.velocity_angle = player.velocity_angle / 2;
		}
		if (GetKey(olc::Key::S).bHeld && (GetKey(olc::Key::D).bHeld || GetKey(olc::Key::A).bHeld))
		{
			player.velocity_angle = (player.velocity_angle - PI) / -2 + PI;
		}

		player.velocity.x = cos(player.angle + player.velocity_angle) * player.movement_speed * tc;
		player.velocity.y = sin(player.angle + player.velocity_angle) * player.movement_speed * tc;

		if (GetKey(olc::Key::A).bHeld || GetKey(olc::Key::D).bHeld || GetKey(olc::Key::W).bHeld || GetKey(olc::Key::S).bHeld) 
		{
			player.x += player.velocity.x;
			player.y += player.velocity.y;

			// Animation for walking
			if (player.walk_animation == true) {
				wall.offset = (sin(DR * player.moved_Distance - PI2) + 1) / player.walk_animation_waveLength;
				player.moved_Distance += player.walk_animation_speed * 1000 * tc;
			}
		}

		// --VERTICAL MOVEMENT IN 3D PLANE--

		if (GetKey(olc::Key::C).bHeld) { player.z += 2 * tc; }
		if (GetKey(olc::Key::V).bHeld) { player.z -= 2 * tc; }

		//------------------------------------------------------------------------------------------------------------------

		// -- ROTATIONAL MOVEMENT --
		olc::vf2d rotational_Movement_Speed;
		rotational_Movement_Speed.x = abs(pMouseCoordinates.x - GetMouseX()) * tc * player.xSensitivity;
		rotational_Movement_Speed.y = abs(pMouseCoordinates.y - GetMouseY()) * tc * player.ySensitivity;

		// rotation i x-led
		if (( pMouseCoordinates.x > GetMouseX() && GetMouseX() > 512 && GetMouseX() < ScreenWidth()))
		{ 
			player.angle -= rotational_Movement_Speed.x;      if (player.angle < DR)      player.angle += 2 * PI;
		} 
		else if (GetKey(olc::Key::G).bHeld) { player.angle -= 5 * tc * player.xSensitivity;  if (player.angle < DR)      player.angle += 2 * PI; }

		if (pMouseCoordinates.x < GetMouseX() && GetMouseX() > 512 && GetMouseX() < ScreenWidth() || GetKey(olc::Key::RIGHT).bHeld)
		{ 
			player.angle += rotational_Movement_Speed.x;   if (player.angle > 359 * DR)   player.angle -= 2 * PI;
		}
		else if (GetKey(olc::Key::J).bHeld) { player.angle += 5 * tc * player.xSensitivity;  if (player.angle > 359 * DR)   player.angle -= 2 * PI;}

		// rotation i y-led
		
		/*
		if ((pMouseCoordinates.y > GetMouseY() && GetMouseY() < ScreenHeight() && GetMouseY() > 0) || GetKey(olc::Key::DOWN).bHeld) { 
			wall.angle += rotational_Movement_Speed.y;
		}
		*/
	    if (GetKey(olc::Key::Y).bHeld) { wall.angle += 5 * tc * player.ySensitivity;  }
		/*
		if ((pMouseCoordinates.y < GetMouseY() && GetMouseY() < ScreenHeight() && GetMouseY() > 0) || GetKey(olc::Key::UP).bHeld) {
			wall.angle -= rotational_Movement_Speed.y; 
		}
		*/
		if (GetKey(olc::Key::H).bHeld) { wall.angle -= 5 * tc * player.ySensitivity;  }

		// saves mouse coordinates
		pMouseCoordinates = { (float)GetMouseX(), (float)GetMouseY() };

		// -- SET WORLD LIMITS --  

		olc::vf2d borderPos, mapPosCoordinates;
		int mapPos;

		// ritar en cirkel runt spelaren och kollar om någon vägg rör cirkeln

		/*

		for (int i = 0; i < 10; i++) 
		{
			borderPos.x = player.width / 2 * cos(360 / 10 * DR * i);
			borderPos.y = player.width / 2 * sin(360 / 10 * DR * i);

			mapPosCoordinates.x = ((int)(player.previous_x + borderPos.x) / mapS) * mapS;
			mapPosCoordinates.y = ((int)(player.previous_y + borderPos.y) / mapS) * mapS;
			mapPos = (mapPosCoordinates.y * mapWidth) / mapS + (mapPosCoordinates.x / mapS);

			if (mapPosCoordinates.x <= mapWidth * mapS && mapPosCoordinates.x >= 0 && mapPosCoordinates.y <= mapHeight * mapS && mapPosCoordinates.y >= 0)
			{
				if (player.x + borderPos.x < mapPosCoordinates.x && map[mapPos - 1] > 0) { player.x = player.previous_x; }
				if (player.x + borderPos.x > mapPosCoordinates.x + mapS && map[mapPos + 1] > 0) { player.x = player.previous_x; }

				if (player.y + borderPos.y < mapPosCoordinates.y && map[mapPos - mapWidth] > 0) { player.y = player.previous_y; }
				if (player.y + borderPos.y > mapPosCoordinates.y + mapS && map[mapPos + mapWidth] > 0) { player.y = player.previous_y; }
			}
		}
		*/
	}

	// [ cast rays and draw 3D world ]
	void DrawRays2D()
	{

		// dof = depth off field, changes how far the ray goes
		int doF = 0, kontrast = 0, mapIn; 
		float rayAngle, xOffset, yOffset;
		float tanOfFoW = tan(player.foW * DR / 2);

		int rayCast = 1024;
		wall.width = (ScreenWidth() - 512) / rayCast;

		for (int r = 0; r < rayCast; r++)
		{
			list_rayPoints_Values list_rayPoints[layers];
			ray_Values newRay;

		    float rayAngleIncrease = atan(-tanOfFoW + (tanOfFoW / (rayCast / 2)) * r);
			rayAngle = player.angle + rayAngleIncrease;  if (rayAngle < 0) { rayAngle += 2 * PI; } if (rayAngle > 2 * PI) { rayAngle -= 2 * PI; }

			float tanOfAngle = tan(rayAngle);

			//--  CHECK HORIZONTAL LINES --
			doF = 0;
			float aTan = -1 / tanOfAngle;
			bool lookingUp = false;

			if (rayAngle > PI) //looking up
			{
				newRay.y = floor(player.y / mapS) * mapS;

				if (newRay.y < 0) { newRay.y = 0; }
				else if (newRay.y > mapS * mapHeight) { newRay.y = mapS * mapWidth; }   // newRay.x cap
				newRay.x = (player.y - newRay.y) * aTan + player.x;
				yOffset = -mapS; xOffset = -yOffset * aTan;
				lookingUp = true; mapIn = mapWidth;
			}

			else if (rayAngle < PI)	//looking down
			{
				newRay.y = floor(player.y / mapS) * mapS + mapS;

				if (newRay.y < 0) { newRay.y = 0; }
				else if (newRay.y > mapS * mapHeight) { newRay.x = mapS * mapWidth; }   // newRay.y cap
				newRay.x = (player.y - newRay.y) * aTan + player.x;
				yOffset = mapS; xOffset = -yOffset * aTan;
				mapIn = -mapWidth;
			}

			while (doF < mapHeight)
			{
				if (lookingUp) { newRay.mapPosY = (int)(newRay.y - 0.0001) / mapS; }
				else { newRay.mapPosY = (int)(newRay.y) / mapS; }
				newRay.mapPosX = (int)(newRay.x) / mapS; newRay.mapPos = newRay.mapPosY * mapWidth + newRay.mapPosX;

				for (int i = 0; i < layers; i++) 
				{
					if (newRay.mapPos > 0 && newRay.mapPos <= (mapWidth * mapHeight) && (map[i][newRay.mapPos] > 0 || map[i][newRay.mapPos + mapIn] > 0) && newRay.x < mapS * mapWidth && newRay.y < mapS * mapWidth && newRay.x > 0 && newRay.y > 0)
					{
						newRay.disT = Pyth(player.x, player.y, newRay.x, newRay.y);
						list_rayPoints[i].list.push_back(newRay);
					}
				}
				newRay.x += xOffset; newRay.y += yOffset; doF += 1;
			}

			//-- check vertical lines--
			doF = 0;
			bool lookingLeft = false;

			float nTan = -tanOfAngle;

			if (rayAngle > PI2 && rayAngle < PI3) // Looking left
			{
				newRay.x = floor(player.x / mapS) * mapS; 

				if (newRay.x < 0) { newRay.x = 0; }
				else if (newRay.x > mapS * mapWidth) { newRay.x = mapS * mapWidth; }   // newRay.x cap
				newRay.y = (player.x - newRay.x) * nTan + player.y;
				xOffset = -mapS; yOffset = -xOffset * nTan;
				lookingLeft = true; mapIn = 1;
			}

			else if (rayAngle < PI2 || rayAngle > PI3) // Looking right
			{
				newRay.x = floor(player.x / mapS) * mapS + mapS;

				if (newRay.x < 0) { newRay.x = 0; }
				else if (newRay.x > mapS * mapWidth) { newRay.x = mapS * mapWidth; }   // newRay.x cap
				newRay.y = (player.x - newRay.x) * nTan + player.y;
				xOffset = mapS; yOffset = -xOffset * nTan;
				mapIn = -1;
			} 

			while (doF < mapWidth)
			{
				if (lookingLeft) { newRay.mapPosX = (int)(newRay.x - 0.0001) / mapS; }
				else { newRay.mapPosX = (int)(newRay.x) / mapS; }

				newRay.mapPosY = (int)(newRay.y) / mapS; newRay.mapPos = newRay.mapPosY * mapWidth + newRay.mapPosX;
				for (int i = 0; i < layers; i++)
				{
					if (newRay.mapPos > 0 && newRay.mapPos <= (mapWidth * mapHeight) && (map[i][newRay.mapPos] > 0 || map[i][newRay.mapPos + mapIn] > 0) && newRay.x < mapS * mapWidth && newRay.y < mapS * mapWidth && newRay.x > 0 && newRay.y > 0)
					{
						newRay.disT = Pyth(player.x, player.y, newRay.x, newRay.y);
						list_rayPoints[i].list.push_back(newRay);
					}
				}
				newRay.x += xOffset; newRay.y += yOffset; doF += 1; 
			}

			for (int i = 0; i < layers; i++)
			{
				list_rayPoints[i].orderInZ = (i);
				list_rayPoints[i].orderInArray = abs(i + player.z);
			}

			sort(list_rayPoints, list_rayPoints + layers, compare2);

			for (int i = 0; i < layers; i++)
			{
				sort(list_rayPoints[i].list.begin(), list_rayPoints[i].list.end(), compare);

				for (vector<ray_Values>::iterator rayPoint = list_rayPoints[i].list.begin(); rayPoint != list_rayPoints[i].list.end(); rayPoint++)
				{
					//-- DRAW 3D WORLD --
					float ca = player.angle - rayAngle; if (ca < 0) { ca += 2 * PI; } if (ca > 2 * PI) { ca -= 2 * PI; } (*rayPoint).disT = (*rayPoint).disT * cos(ca);  // best�mmer distans till v�gg
					float lineH = (mapS * (mapS / ((tan(player.foW / 2 * DR) * mapS) / (rayCast / 2))) / (*rayPoint).disT) * wall.width;  if (lineH > 10000) { lineH = 10000; }                             //Line Height (OBS: ändra * 64 ifall mapp ändras)
					float lineOffset = wall.angle + lineH * (player.z + list_rayPoints[i].orderInZ);                                                                     //Line Offset
					float alphaV = 255 / ((*rayPoint).disT * 0.0085 + 1);

					//FillRect(r * wall.width + 512, lineOffset, wall.width, lineH, olc::Pixel(200, 150, 60, alphaV - kontrast));
				    PaintTextures(r * wall.width + 512, wall.width, lineOffset, lineH, (*rayPoint).x, (*rayPoint).y, (*rayPoint).mapPos, alphaV, kontrast);
					
					DrawLine(player.x, player.y, (*rayPoint).x, (*rayPoint).y, olc::DARK_RED);
				}
				list_rayPoints[i].list.clear();
			}
        }
    }
	

	void PaintTextures(int lineX, int lineWidth, float lineOffset, float lineH, float rayPosX, float rayPosY, int mapPos, float alphaV, int kontrast) {

		float mapPosX = floor((mapPos - (mapPos / mapWidth) * mapWidth) * mapS);
		float mapPosY = floor((mapPos / mapHeight) * mapS);
		float facing = 0, column = 0;


		if (rayPosY == mapPosY && rayPosX != mapPosX && rayPosX != mapPosX + mapS) //uppifrån id 1
		{
			facing = 1;
			column = (rayPosX - mapPosX) / mapS;
		}
		else if (rayPosY == mapPosY + mapS && rayPosX != mapPosX + mapS && rayPosX != mapPosX) //nedifrån id 2
		{
			facing = 2;
			column = (mapS - (rayPosX - mapPosX)) / mapS;
		}
		if (rayPosX == mapPosX && rayPosY != mapPosY && rayPosY != mapPosY + mapS) //höger id 3
		{
			facing = 3;
			column = (mapS - (rayPosY - mapPosY)) / mapS;
		}
		else if (rayPosX == mapPosX + mapS && rayPosY != mapPosY + mapS && rayPosY != mapPosY) //Anta vänster id 4, alla övriga testade
		{
			facing = 4;
			column = (rayPosY - mapPosY) / mapS;
		}
			for (int i = 0; i < lineH; i++) {
				if (lineOffset + i >= 0 && lineOffset + i <= ScreenHeight()) 
				{
					olc::Pixel myColor = sprTile->Sample(column, 1 / lineH * i);
					Draw(lineX, lineOffset + i, olc::Pixel(myColor.r, myColor.g, myColor.b, 255));
				}
		}
	}
	


	void backgroundColor(olc::Pixel color) {
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), color);
	}

	float Pyth(float ax, float ay, float bx, float by)  //pythagoras sats
	{
		return sqrt(pow((bx - ax), 2) + pow((by - ay), 2));
	}

	olc::vf2d addVectors(olc::vf2d vektor1, olc::vf2d vektor2)
	{
		olc::vf2d resVector;
		resVector.x = vektor1.x + vektor2.x;
		resVector.y = vektor1.y + vektor2.y;
		return resVector;
	}

	void FillPolygon(olc::vf2d p1, olc::vf2d p2, olc::vf2d p3, olc::vf2d p4, olc::Pixel c)
	{
		FillTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, c);
		FillTriangle(p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, c);
	}

	void DrawLinePro(double lineX, double lineY, double lineX2, double lineY2, int width, olc::Pixel color)
	{
		double dy, dx;
		double lineK = (lineY2 - lineY) / (lineX2 - lineX);
		double lineA = atan(lineK);
		dy = sin(PI2 - lineA); dx = -cos(PI2 - lineA);

		int xLimit = (width / 2);
		if (xLimit * 2 < width) { DrawLine(lineX,lineY, lineX2, lineY2, color); }

		for (int i = 0; i < xLimit; i++)
		{
			DrawLine(lineX + (dx * i), lineY + (dy * i), lineX2 + (dx * i), lineY2 + (dy * i), color);
			DrawLine(lineX - (dx * i), lineY - (dy * i), lineX2 - (dx * i), lineY2 - (dy * i), color);
		}
	}


public:
	Example()
	{
		// Name your application
		sAppName = "Raycaster";
	}


public:
	bool OnUserCreate() override
	{
		sprTile = std::make_unique<olc::Sprite>("photo-1552288092-76e7d732366c.jpg");

		backgroundColor(olc::DARK_GREY);
		player.x = 288; player.y = 130; player.z = 0; player.width = 1;
		player.angle = PI2;
		player.movement_speed = 100;
		player.walk_animation_speed = 1;
		player.walk_animation_waveLength = 4;
		player.walk_animation = false;
		player.foW = 60;
		wall.angle = ScreenHeight() / 4;
		player.ySensitivity *= 2;
		player.xSensitivity *= 0.5;

		return true;
	}

	float showVariable;
	bool OnUserUpdate(float fElapsedTime) override
	{
		if (GetKey(olc::Key::ESCAPE).bPressed) { return false; }

		backgroundColor(olc::DARK_BLUE);
		FillRect(0, 0, 512, ScreenHeight(), olc::Pixel(255, 255, 255, 100));
		DrawMap2D();
		DrawRays2D();
		Drawplayer();
		buttons();
	    
		return true;
	}
};



int main()
{

	Example demo;
	if (demo.Construct(512 * 3, 512 * 2 , 1, 1, true))
		demo.Start();
	return 0;
}
