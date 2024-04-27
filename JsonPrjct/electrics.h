#pragma once

#include <fstream>
#include <iostream>
#include <nlohmann\json.hpp>
#include <vector>
#include <string_view>

using EquipmentList = std::vector<std::pair<std::string, float>>;

class Electrics
{
public:
	Electrics(std::string jsonFile) : nameJsonFile(jsonFile) {};

private:
	std::string nameJsonFile{};	
	
	size_t mainsVoltage = 0;
	size_t ampere       = 0;
	float cableSection  = 0.0f;

	std::string cableType{};
	std::string wiringType{};
		
	const std::vector<size_t> switchRating       { 1, 2, 3, 6, 10, 16, 20, 25, 32, 40, 50, 63, 80, 100 };
	const std::vector<float>  sectionList        { 1.5f, 2.5f, 4.0f, 6.0f, 10.0f, 16.0f, 25.0f, 35.0f  };

private:

	// Error messages:
	consteval static std::string_view FileJsonNotFound()    noexcept;
	consteval static std::string_view EquipmentListEmpty()  noexcept;
	consteval static std::string_view ErrorCompletingJson() noexcept;

private:

	void ErrorCall(std::string_view errorMessage) const;
	EquipmentList getJsonInfo();

private:

	size_t getAmpere(float power);
	float  getCableSection(size_t ampere);
	size_t getRCD(size_t sumAmpere, size_t equipment—ount);

public:

	void —alculation();
};