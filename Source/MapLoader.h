#pragma once

#include "Application.h"
#include "Checkpoint.h"
#include "SlowZone.h"
#include "Finishline.h"
#include "PhysicEntity.h"
#include "Transform2D.h"
#include "Map.h"
#include "Circle.h"
#include "pugixml.hpp"
#include <algorithm>

class MapData {
public:
	MapData() {}
	int width = 0, height = 0;
	std::vector<Checkpoint*> checkpoints;
	std::vector<PhysicEntity*> obstacles;
	std::vector<SlowZone*> slowZones;
	std::vector<std::pair<Transform2D, int>> startPositions;
	Finishline* finishline = nullptr;
};

class MapLoader
{
public:
	static Map* LoadMap(int mapNumber, Application* app, Module* listener) {
		int* boundsIn = nullptr;
		unsigned int boundsInSize = 0;
		int* boundsOut = nullptr;
		unsigned int boundsOutSize = 0;
		int* navigationLayerIn = nullptr;
		unsigned int navigationLayerInSize = 0;
		int* navigationLayerOut = nullptr;
		unsigned int navigationLayerOutSize = 0;
		const char* mapImg = nullptr;
		const char* mapTmx = nullptr;
		const char* boxTex = nullptr;
		const char* circleTex = nullptr;
		if (mapNumber == 1)
		{
			int bIn[34] = {
				477, 480,
				477, 2505,
				1017, 2514,
				1020, 2017,
				1977, 2020,
				1977, 2514,
				2514, 2520,
				2517, 1980,
				1020, 1977,
				1022, 1020,
				2520, 1017,
				2520, 480,
				2482, 482,
				2477, 977,
				1020, 980,
				1020, 482,
				491, 477
			};
			int bOut[32] = {
				1492, 20,
				16, 24,
				20, 2976,
				1480, 2975,
				1484, 2484,
				1520, 2485,
				1522, 2980,
				2977, 2977,
				2976, 1523,
				1523, 1520,
				1523, 1480,
				2976, 1480,
				2976, 26,
				2026, 26,
				2020, 520,
				1473, 523
			};
			int navIn[84] = {
				368, 2492,
				368, 504,
				420, 416,
				500, 380,
				1000, 376,
				1080, 408,
				1132, 504,
				1200, 700,
				1312, 808,
				1472, 860,
				2028, 864,
				2216, 796,
				2312, 676,
				2380, 440,
				2492, 368,
				2588, 416,
				2624, 508,
				2620, 996,
				2576, 1084,
				2484, 1128,
				1484, 1124,
				1196, 1276,
				1120, 1512,
				1288, 1796,
				1540, 1872,
				2500, 1872,
				2580, 1916,
				2620, 1996,
				2620, 2484,
				2584, 2576,
				2472, 2620,
				1988, 2612,
				1904, 2568,
				1852, 2460,
				1696, 2184,
				1488, 2128,
				1280, 2196,
				1124, 2488,
				1084, 2584,
				996, 2620,
				496, 2620,
				412, 2568
			};
			int navOut[96] = {
				217, 262,
				495, 127,
				1020, 140,
				1240, 217,
				1372, 502,
				1427, 597,
				1502, 625,
				2000, 622,
				2077, 585,
				2120, 497,
				2196, 288,
				2480, 136,
				2716, 196,
				2828, 340,
				2868, 552,
				2864, 1020,
				2804, 1212,
				2672, 1320,
				2488, 1364,
				1504, 1368,
				1384, 1436,
				1368, 1512,
				1420, 1584,
				1504, 1624,
				2500, 1624,
				2704, 1696,
				2816, 1824,
				2860, 1972,
				2864, 2500,
				2800, 2708,
				2692, 2812,
				2528, 2860,
				1964, 2864,
				1792, 2808,
				1680, 2676,
				1624, 2476,
				1556, 2384,
				1452, 2372,
				1380, 2448,
				1360, 2576,
				1296, 2708,
				1128, 2836,
				1020, 2860,
				476, 2860,
				276, 2788,
				168, 2640,
				128, 2500,
				132, 480
			};

			boundsIn = bIn;
			boundsInSize = 34;
			boundsOut = bOut;
			boundsOutSize = 32;
			navigationLayerIn = navIn;
			navigationLayerInSize = 84;
			navigationLayerOut = navOut;
			navigationLayerOutSize = 96;
			mapImg = "Assets/Maps/Road/road.png";
			mapTmx = "Assets/Maps/Road/road.tmx";
			boxTex = "Assets/Maps/Road/boxObstacle.png";
			circleTex = "Assets/Maps/Road/circleObstacle.png";
		}
		else if (mapNumber == 2) 
		{
			// add map 2
		}

		Map* map = new Map(app, 0, 0, 0, boundsIn, boundsInSize, boundsOut, boundsOutSize, navigationLayerIn, navigationLayerInSize, navigationLayerOut, navigationLayerOutSize, listener, LoadTexture(mapImg));
		MapData data = GetMapData(mapTmx, app, listener, boxTex, circleTex);

		std::sort(data.startPositions.begin(), data.startPositions.end(),
			[](const std::pair<Transform2D, int>& a, const std::pair<Transform2D, int>& b) {
				return a.second < b.second;
			});

		for (auto& p : data.startPositions) map->addStartPosition(p.first);
		for (auto& c : data.checkpoints) map->addCheckPoint(c);
		for (auto& o : data.obstacles) map->addObstacle(o);
		for (auto& z : data.slowZones) map->addSlowZone(z);
		map->addFinishLine(data.finishline);
		return map;
	}

	static MapData GetMapData(const char* mapPath, Application* app, Module* listener, const char* boxTex, const char* circleTex) {
		MapData data;

		pugi::xml_document mapFileXML;
		pugi::xml_parse_result result = mapFileXML.load_file(mapPath);

		data.width = mapFileXML.child("map").attribute("width").as_int();
		data.height = mapFileXML.child("map").attribute("height").as_int();

		for (pugi::xml_node objectGroupNode = mapFileXML.child("map").child("objectgroup"); objectGroupNode != NULL; objectGroupNode = objectGroupNode.next_sibling("objectgroup")) {

			std::string name = objectGroupNode.attribute("name").as_string();

			for (pugi::xml_node objectNode = objectGroupNode.child("object"); objectNode != NULL; objectNode = objectNode.next_sibling("object")) {
				int x = objectNode.attribute("x").as_int();
				int y = objectNode.attribute("y").as_int();
				int width = objectNode.attribute("width").as_int();
				int height = objectNode.attribute("height").as_int();
				float rot = objectNode.attribute("rotation").as_float();

				float halfW = width / 2.f;
				float halfH = height / 2.f;
				float centerX = x + halfW;
				float centerY = y + halfH;

				float newX = centerX;
				float newY = centerY;

				if (rot != 0) {
					float r = rot * (PI / 180.f);
					newX = x + halfW * cos(r) - halfH * sin(r);
					newY = y + halfW * sin(r) + halfH * cos(r);
				}

				if (name == "StartPositions")
				{
					int order = objectNode.child("properties").child("property").attribute("value").as_int();
					Transform2D t(newX, newY, rot);
					data.startPositions.push_back(std::make_pair(t, order));
				}
				else if (name == "SlowZones")
				{
					float scale = objectNode.child("properties").child("property").attribute("value").as_float();
					SlowZone* z = new SlowZone(app, newX, newY, width, height, rot, listener, scale);
					data.slowZones.push_back(z);
				}
				else if (name == "Obstacles")
				{
					std::string type = "";
					float restitution = 0;
					
					for (pugi::xml_node propNode = objectNode.child("properties").child("property"); propNode != NULL; propNode = propNode.next_sibling("property")) {
						std::string prop = propNode.attribute("name").as_string();
						pugi::xml_attribute valAttr = propNode.attribute("value");
						if (prop == "type") type = valAttr.as_string();
						if (prop == "restitution") restitution = valAttr.as_float();
					}
					
					PhysicEntity* o = nullptr;
					if (type == "box") o = new Box(app->physics, app->renderer, newX, newY, listener, LoadTexture(boxTex), EntityType::OBSTACLE, rot, false, restitution);
					else if (type == "circle") o = new Circle(app->physics, app->renderer, newX, newY, listener, LoadTexture(circleTex), EntityType::OBSTACLE, rot, false, restitution);
					
					if (o) data.obstacles.push_back(o);
				}
				else if (name == "Checkpoints")
				{
					int order = objectNode.child("properties").child("property").attribute("value").as_int();
					Checkpoint* c = new Checkpoint(app, newX, newY, width, height, rot, listener, order);
					data.checkpoints.push_back(c);
				}
				else if (name == "Finishline")
				{
					data.finishline = new Finishline(app, newX, newY, width, height, rot, listener);
				}
			}
		}
		
		return data;
	}
};