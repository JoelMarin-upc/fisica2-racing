#pragma once

#include "Application.h"
#include "Checkpoint.h"
#include "SlowZone.h"
#include "Finishline.h"
#include "PhysicEntity.h"
#include "Transform2D.h"
#include "Map.h"
#include "pugixml.hpp"

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
		int* points = nullptr;
		unsigned int size = 0;
		const char* mapImg = nullptr;
		const char* mapTmx = nullptr;
		if (mapNumber == 1)
		{
			int p[6] = { 
				1, 1, 
				2, 2, 
				3, 3 
			};
			points = p;
			size = 6;
			mapImg = "Assets/road.png";
			mapTmx = "Assets/road.tmx";
		}
		else if (mapNumber == 2) 
		{
			// change with map 2
			int p[6] = {
				1, 1,
				2, 2,
				3, 3
			};
			points = p;
			size = 6;
			mapImg = "Assets/road.png";
			mapTmx = "Assets/road.tmx";
		}

		mapImg = "Assets/road.png";
		Map* map = new Map(app, 0, 0, 0, points, size, listener, LoadTexture(mapImg));
		MapData data = GetMapData(mapTmx, app, listener);

		// order start positions by p.second

		for (auto& p : data.startPositions) map->addStartPosition(p.first);
		for (auto& c : data.checkpoints) map->addCheckPoint(c);
		for (auto& o : data.obstacles) map->addObstacle(o);
		for (auto& z : data.slowZones) map->addSlowZone(z);
		map->addFinishLine(data.finishline);
		return map;
	}

	static MapData GetMapData(const char* mapPath, Application* app, Module* listener) {
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
					// ?
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