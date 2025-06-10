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


class Light : public EnergyDevice {
	double brightness;
public:
	Light(const std::string& name, double powerConsumption, double brightness, bool isActive = false)
		: EnergyDevice(name, powerConsumption, isActive), brightness(brightness) {}


	void activate() override { isActive = true; }
	void deactivate() override { isActive = false; }

	void displayStatus() const override {
		std::cout << "Light: " << name << ": "
			<< (isActive ? "ON" : "OFF") << ", "
			<< "Brightness: " << brightness << "%, "
			<< "power: " << (isActive ? powerConsumption * brightness / 100 : 0) << "W" << std::endl;
	}

	double calculatePower() const override {
		return isActive ? powerConsumption * brightness / 100 : 0;
	}

	void setBrightness(double level) {
		if (level < 0) brightness = 0;
		else if (level > 100) brightness = 100;
		else brightness = level;
	}
};


class Termostat : public EnergyDevice {
	double currentTemp;
	double targetTemp;
	public: 
		Termostat(const std::string& name, double powerConsumption, double currentTemp, double targetTemp, bool isActive = false)
			: EnergyDevice(name, powerConsumption, isActive), currentTemp(currentTemp), targetTemp(targetTemp) {}

