#include "electrics.h"

consteval std::string_view Electrics::FileJsonNotFound()    noexcept { return "File .json not found.";           }
consteval std::string_view Electrics::EquipmentListEmpty()  noexcept { return "The equipment list is empty.";    }
consteval std::string_view Electrics::ErrorCompletingJson() noexcept { return "Error completing the.json file."; }

void Electrics::ErrorCall(std::string_view errorMessage) const
{
	std::cerr << errorMessage << std::endl;
	exit(1);
}

EquipmentList Electrics::getJsonInfo()
{
    std::ifstream read(nameJsonFile);

    if (!read.is_open()) { ErrorCall(FileJsonNotFound()); }

    EquipmentList readList;
    nlohmann::json jsonRead;

    try
    {
        read >> jsonRead;
        readList     = jsonRead["Equipment"];
        mainsVoltage = jsonRead["Mains voltage"];
        cableType    = jsonRead["Cable(copper/aluminum)"];
        wiringType   = jsonRead["Wiring type(open/close)"];

    }
    catch (...)
    {        
        ErrorCall(ErrorCompletingJson());
    }    

    if (readList.empty()) { ErrorCall(EquipmentListEmpty()); }

    read.close();

	return readList;
}

size_t Electrics::getAmpere(float power) 
{ 
    float res = 0.0f;

    res = power / mainsVoltage;

    for (auto rating : switchRating)
    {
        if (res <= rating) { return rating; }
    }

    return 100;
}

float Electrics::getCableSection(size_t ampere)
{    

    std::vector<size_t> wiringTolerance{};

    if      (wiringType == "open"  && cableType == "copper")   { wiringTolerance = { 23, 30, 41, 50, 80, 100, 140, 170 }; }
    else if (wiringType == "open"  && cableType == "aluminum") { wiringTolerance = { 0, 24, 32, 39, 60, 75, 105, 130 };   }
    else if (wiringType == "close" && cableType == "copper")   { wiringTolerance = { 15, 21, 27, 34, 50, 80, 100, 135 };  }
    else if (wiringType == "close" && cableType == "aluminum") { wiringTolerance = { 0, 16, 21, 26, 38, 55, 65, 75 };     }

   
    if (wiringTolerance.empty()) { ErrorCall(ErrorCompletingJson()); }

    for (size_t i = 0; i < wiringTolerance.size(); i++)
    {
        if (ampere <= wiringTolerance[i]) { return sectionList[i]; }
    }

    return 35.0f;
}

size_t Electrics::getRCD(size_t sumAmpere, size_t equipmentÑount)
{
    float coefficient = 0.0f;
    float RCDAmpere   = 0.0f;

    if      (equipmentÑount == 1)  { coefficient = 1.0f; }
    else if (equipmentÑount <= 3)  { coefficient = 0.8f; }
    else if (equipmentÑount <= 5)  { coefficient = 0.7f; }
    else if (equipmentÑount <= 9)  { coefficient = 0.6f; }
    else if (equipmentÑount >= 10) { coefficient = 0.5f; }

    RCDAmpere = sumAmpere * coefficient;

    for (auto rating : switchRating)
    {
        if (RCDAmpere <= rating) { return rating; }
    }

    return 100;
}

void Electrics::Ñalculation()
{
    EquipmentList jsonInfo = getJsonInfo();
    size_t sumAmpere = 0;

    std::ofstream resultÑalculation("result.json");
    if (!resultÑalculation.is_open())
    {
        ErrorCall(FileJsonNotFound());
    }

    nlohmann::json jsonResult;


    for (auto list : jsonInfo)
    {        
        ampere = getAmpere(list.second);
        cableSection = getCableSection(ampere);

        sumAmpere += ampere;

        jsonResult[list.first]["Power"] = list.second;
        jsonResult[list.first]["Ampere"] = ampere;
        jsonResult[list.first]["Cable Section"] = cableSection;                      
    }

    jsonResult["___RCD for the group___"] = getRCD(sumAmpere, jsonInfo.size());

    resultÑalculation << jsonResult.dump(4);
    resultÑalculation.close();
}

