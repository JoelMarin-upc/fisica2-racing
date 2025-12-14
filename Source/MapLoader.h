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
        int totalCars = 0;
        std::string carsBasePath = "";
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
            int bIn[260] = {
                1722, 1588,
                1722, 1132,
                1702, 1132,
                1702, 1569,
                771, 1569,
                735, 1554,
                685, 1556,
                639, 1572,
                608, 1594,
                566, 1563,
                508, 1532,
                474, 1505,
                444, 1499,
                428, 1473,
                433, 1437,
                424, 1416,
                429, 1396,
                447, 1364,
                445, 1327,
                428, 1308,
                424, 1289,
                440, 1268,
                442, 1234,
                438, 1210,
                421, 1178,
                430, 1133,
                438, 1111,
                422, 1096,
                425, 1078,
                436, 1068,
                433, 1044,
                429, 1030,
                403, 1025,
                384, 1027,
                384, 717,
                394, 708,
                403, 717,
                419, 705,
                430, 631,
                433, 594,
                425, 575,
                429, 564,
                426, 531,
                416, 493,
                403, 468,
                390, 457,
                393, 433,
                1688, 433,
                1688, 415,
                412, 411,
                426, 385,
                453, 334,
                451, 307,
                433, 288,
                419, 274,
                398, 274,
                385, 284,
                371, 298,
                355, 297,
                343, 282,
                330, 264,
                307, 256,
                279, 259,
                264, 278,
                261, 308,
                271, 325,
                273, 343,
                276, 366,
                283, 387,
                274, 404,
                268, 426,
                269, 451,
                279, 488,
                294, 516,
                315, 539,
                315, 564,
                306, 584,
                292, 605,
                270, 621,
                256, 634,
                253, 653,
                262, 677,
                269, 703,
                283, 731,
                296, 749,
                323, 762,
                346, 755,
                372, 723,
                372, 1034,
                362, 1076,
                365, 1092,
                356, 1116,
                357, 1135,
                351, 1157,
                347, 1215,
                346, 1290,
                340, 1329,
                339, 1384,
                339, 1402,
                330, 1452,
                323, 1476,
                323, 1491,
                301, 1521,
                280, 1558,
                268, 1600,
                266, 1650,
                278, 1688,
                298, 1725,
                326, 1757,
                367, 1782,
                403, 1772,
                433, 1756,
                457, 1754,
                484, 1741,
                522, 1733,
                564, 1731,
                599, 1734,
                631, 1743,
                663, 1760,
                694, 1773,
                716, 1774,
                731, 1772,
                753, 1763,
                786, 1736,
                800, 1708,
                805, 1669,
                803, 1632,
                794, 1606,
                801, 1588,
                1704, 1588
            };
            int bOut[186] = {
                19, 40,
                20, 73,
                22, 1843,
                27, 1901,
                59, 1911,
                106, 1960,
                148, 1989,
                179, 2004,
                1870, 2006,
                1902, 1975,
                1928, 1951,
                1943, 1921,
                1949, 1885,
                1981, 1861,
                2006, 1811,
                2030, 1789,
                2030, 1103,
                2012, 1069,
                2008, 1033,
                1989, 1006,
                1998, 969,
                1998, 914,
                1979, 870,
                1960, 823,
                1926, 814,
                1894, 774,
                1879, 731,
                1452, 731,
                1426, 744,
                1425, 1084,
                717, 1084,
                717, 822,
                736, 822,
                736, 1068,
                1405, 1068,
                1408, 740,
                1445, 712,
                1883, 712,
                1902, 667,
                1929, 628,
                1958, 600,
                1990, 581,
                2024, 567,
                2024, 114,
                1978, 90,
                1958, 55,
                1915, 37,
                1879, 17,
                1787, 17,
                1734, 8,
                1705, 20,
                1681, 20,
                1655, 12,
                1591, 13,
                1573, 4,
                1545, 6,
                1523, 17,
                1492, 22,
                1439, 29,
                1389, 22,
                1364, 20,
                1349, 12,
                1236, 10,
                1190, 20,
                1165, 24,
                1137, 10,
                1110, 10,
                1083, 17,
                1052, 13,
                995, 38,
                954, 65,
                884, 77,
                838, 52,
                799, 29,
                742, 19,
                723, 6,
                708, 13,
                678, 12,
                625, 19,
                575, 6,
                494, 27,
                389, 35,
                352, 27,
                276, 28,
                211, 26,
                188, 32,
                160, 28,
                140, 19,
                118, 36,
                93, 37,
                76, 28,
                52, 35,
                26, 35
            };
            int navIn[310] = {
                435, 284,
                1669, 285,
                1709, 311,
                1728, 346,
                1729, 392,
                1705, 426,
                1669, 451,
                1394, 451,
                1335, 463,
                1286, 480,
                1267, 480,
                1222, 511,
                1192, 554,
                1178, 600,
                1174, 666,
                1174, 732,
                1176, 777,
                1188, 801,
                1165, 817,
                1134, 841,
                1106, 849,
                1075, 837,
                1048, 831,
                1023, 820,
                1018, 780,
                1021, 759,
                1046, 748,
                1057, 710,
                1050, 669,
                1034, 643,
                1027, 627,
                1036, 577,
                1025, 539,
                1010, 500,
                1009, 480,
                997, 463,
                966, 457,
                928, 451,
                493, 449,
                467, 472,
                442, 480,
                424, 500,
                428, 553,
                433, 603,
                428, 655,
                415, 718,
                415, 1024,
                434, 1039,
                435, 1069,
                426, 1100,
                436, 1117,
                419, 1176,
                438, 1215,
                440, 1266,
                425, 1294,
                439, 1318,
                445, 1356,
                431, 1396,
                429, 1427,
                434, 1485,
                443, 1501,
                471, 1507,
                525, 1537,
                589, 1576,
                605, 1592,
                634, 1576,
                671, 1556,
                719, 1554,
                750, 1562,
                1580, 1562,
                1620, 1554,
                1645, 1536,
                1664, 1526,
                1677, 1485,
                1672, 1455,
                1659, 1443,
                1658, 1317,
                1670, 1295,
                1673, 1245,
                1677, 1221,
                1668, 1194,
                1651, 1178,
                1656, 1137,
                1651, 1115,
                1638, 1091,
                1635, 1055,
                1652, 1027,
                1686, 1015,
                1727, 1014,
                1754, 1027,
                1764, 1051,
                1761, 1091,
                1747, 1126,
                1761, 1160,
                1760, 1340,
                1750, 1375,
                1761, 1421,
                1761, 1635,
                1751, 1682,
                1731, 1720,
                1695, 1748,
                1646, 1766,
                1614, 1772,
                1563, 1764,
                1526, 1747,
                1489, 1706,
                1445, 1672,
                1373, 1628,
                1312, 1603,
                1289, 1594,
                1068, 1594,
                984, 1604,
                906, 1636,
                849, 1674,
                796, 1718,
                769, 1754,
                730, 1770,
                703, 1775,
                662, 1760,
                607, 1736,
                540, 1729,
                483, 1741,
                462, 1751,
                421, 1763,
                367, 1783,
                339, 1770,
                316, 1755,
                292, 1719,
                271, 1674,
                271, 1609,
                287, 1554,
                285, 744,
                266, 704,
                253, 658,
                261, 626,
                288, 609,
                307, 581,
                314, 563,
                310, 538,
                284, 504,
                270, 463,
                264, 430,
                276, 403,
                283, 385,
                276, 348,
                276, 326,
                261, 307,
                261, 280,
                279, 260,
                312, 255,
                335, 268,
                349, 296,
                372, 297,
                398, 278,
                422, 274
            };
            int navOut[200] = {
                65, 119,
                79, 88,
                106, 69,
                860, 67,
                900, 76,
                950, 68,
                1940, 67,
                1970, 81,
                1981, 108,
                1981, 585,
                1942, 614,
                1915, 650,
                1900, 671,
                1514, 669,
                1468, 678,
                1431, 701,
                1407, 736,
                1407, 800,
                1389, 859,
                1362, 911,
                1325, 959,
                1276, 1000,
                1230, 1025,
                1175, 1046,
                1121, 1053,
                1052, 1052,
                987, 1039,
                931, 1016,
                887, 991,
                849, 956,
                818, 919,
                806, 877,
                806, 742,
                785, 696,
                748, 669,
                701, 668,
                663, 692,
                640, 731,
                636, 754,
                634, 1197,
                648, 1254,
                676, 1304,
                709, 1331,
                740, 1340,
                1356, 1341,
                1399, 1322,
                1425, 1292,
                1439, 1251,
                1440, 1169,
                1428, 1130,
                1426, 1096,
                1426, 1048,
                1440, 989,
                1441, 882,
                1460, 844,
                1489, 818,
                1537, 815,
                1582, 806,
                1603, 792,
                1663, 794,
                1710, 796,
                1757, 800,
                1786, 812,
                1838, 813,
                1879, 796,
                1919, 803,
                1956, 822,
                1984, 872,
                2001, 929,
                1994, 987,
                1985, 1019,
                1981, 1862,
                1947, 1889,
                1946, 1925,
                1914, 1962,
                1889, 1984,
                1412, 1980,
                1375, 1960,
                1324, 1907,
                1274, 1862,
                1216, 1829,
                1151, 1815,
                1084, 1815,
                1028, 1832,
                974, 1862,
                941, 1902,
                913, 1946,
                861, 1979,
                677, 1979,
                645, 1961,
                566, 1955,
                479, 1953,
                425, 1961,
                398, 1974,
                366, 1976,
                332, 1981,
                140, 1979,
                96, 1948,
                67, 1915,
                67, 136
            };

            totalCars = 9;
            carsBasePath = "Assets/Maps/River/Cars/";
			boundsIn = bIn;
			boundsInSize = 260;
			boundsOut = bOut;
			boundsOutSize = 186;
			navigationLayerIn = navIn;
			navigationLayerInSize = 310;
			navigationLayerOut = navOut;
			navigationLayerOutSize = 200;
			mapImg = "Assets/Maps/River/MapaRacing1.png";
			mapTmx = "Assets/Maps/River/Map1.tmx";
			boxTex = "Assets/Maps/River/boxObstacle.png";
			circleTex = "Assets/Maps/River/circleObstacle.png";
		}
		else if (mapNumber == 2) 
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

            totalCars = 4;
            carsBasePath = "Assets/Maps/Road/Cars/";
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

		Map* map = new Map(app, 0, 0, 0, totalCars, carsBasePath, boundsIn, boundsInSize, boundsOut, boundsOutSize, navigationLayerIn, navigationLayerInSize, navigationLayerOut, navigationLayerOutSize, listener, LoadTexture(mapImg));
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