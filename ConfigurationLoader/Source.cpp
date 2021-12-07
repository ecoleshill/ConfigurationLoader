#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

enum LOADTYPE {UNKNOWN, TELEM_ITEM, CONFIG_ITEM};
enum TYPES {tBOOL, tINT8, tINT16, tINT32, tFLOAT64};

struct MyData
{
	TYPES Type;
	std::string Name;
	std::string Units;
	char Data[4];
};

bool LoadConfig(std::string);
void CleanConfig();

std::vector<MyData> MyConfig;
std::string strPortNumber;
std::string strSimulationRate;
bool bAutoLoadTlm;

int main()
{
	std::string CfgFile = "config.txt";
	LoadConfig(CfgFile);

	CleanConfig();

	return 1;
}

void CleanConfig()
{
	while (!MyConfig.empty())
		MyConfig.pop_back();
}

bool LoadConfig(std::string Filename)
{
	bool bLoadResult = false;			
	std::string ReadLine;				
	std::string ParamType;				
	size_t CurrLoc, LastLoc;			
	bool bGood;							

	std::ifstream* ifs = new std::ifstream(Filename);
	if (ifs->is_open())
	{
		LOADTYPE currentType = UNKNOWN;

		while (!ifs->eof())
		{
			std::getline(*ifs, ReadLine);

			if (ReadLine == "#TELEM_ITEM")
				currentType = TELEM_ITEM;
			else if (ReadLine == "#CONFIG_ITEM")
				currentType = CONFIG_ITEM;
			else
			{
				if ((currentType == TELEM_ITEM) && (ReadLine != "") && (ReadLine[0] != '/'))
				{
					bGood = true;			
					MyData NewElement;					//The new data element to be added to the vector space

					//Parse the data using CSV format
					CurrLoc = ReadLine.find(",");
					NewElement.Name = ReadLine.substr(0, CurrLoc);
					LastLoc = CurrLoc;
					CurrLoc = ReadLine.find(",", LastLoc + 1);
					NewElement.Units = ReadLine.substr(LastLoc + 1, CurrLoc - (LastLoc + 1));
					ParamType = ReadLine.substr(CurrLoc + 1, ReadLine.length());

					if (ParamType == "BOOL")
						NewElement.Type = tBOOL;
					else if (ParamType == "INT8")
						NewElement.Type = tINT8;
					else if (ParamType == "INT16")
						NewElement.Type = tINT16;
					else if (ParamType == "INT32")
						NewElement.Type = tINT32;
					else if (ParamType == "FLOAT64")
						NewElement.Type = tFLOAT64;
					else
					{
						bGood = false;
					}

					if (bGood)
					{
						MyConfig.push_back(NewElement);
					}
				}
				else if ((currentType == CONFIG_ITEM) && (ReadLine != "") && (ReadLine[0] != '/'))
				{
					size_t result = ReadLine.find('=', 0);
					std::string Param = ReadLine.substr(0, result);
					std::string Value = ReadLine.substr(result + 1, ReadLine.length() - result);

					if (Param == "Network_SocketPort")
						strPortNumber = Value;
					else if (Param == "AutoLoadTelemetry")
					{
						if (Value == "true")
							bAutoLoadTlm = true;
						else
							bAutoLoadTlm = false;
					}
					else if (Param == "SimulationRate")
						strSimulationRate = Value;
				}
			}
		}

		ifs->close();
		bLoadResult = true;
	}
	else
	{
		bLoadResult = false;
	}
	return bLoadResult;
}