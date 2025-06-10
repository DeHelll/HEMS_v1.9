#include <iostream>
#include <vector>
#include <algorithm>


class EnergyDevice {
protected:
	std::string name;
	double powerConsumption; // in watts
	bool isActive;

public:
	EnergyDevice(const std::string& name, double powerConsumption, bool isActive = false)
		: name(name), powerConsumption(powerConsumption), isActive(isActive) {}

	virtual void activate() = 0;
	virtual void deactivate() = 0;
	virtual void displayStatus() const = 0;
	virtual double calculatePower() const = 0;
	virtual ~EnergyDevice() {};
	std::string getname() const { return name; }
};

