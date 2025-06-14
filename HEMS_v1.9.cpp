#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
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
	Termostat(const std::string& name, double powerConsumption, double currentTemp = 20.0, double targetTemp = 24.0)
		: EnergyDevice(name, powerConsumption), currentTemp(currentTemp), targetTemp(targetTemp) {}


	void activate() override { isActive = true; adjustTemp(); }
	void deactivate() override { isActive = false; }

	void displayStatus() const override {
		std::cout << "Termostat: " << name << ": "
			<< (isActive ? "Heating" : "idle") << ", "
			<< "Current Temp: " << currentTemp << "°C, "
			<< "Target Temp: " << targetTemp << "°C, "
			<< "power: " << (isActive ? powerConsumption : 0) << "W\n";
	}

	double calculatePower() const override {
		return isActive ? powerConsumption : 0;
	}

	void setTarget(double temp) {
		targetTemp = temp;
		if (isActive) adjustTemp();
	}

private:
	void adjustTemp() {
		if (currentTemp < targetTemp) {
			std::cout << name << " heating to " << targetTemp << "°C...\n";
		}
	}
};


class Appliance : public EnergyDevice 
{
public: 
	Appliance(const std::string& name, double powerConsumption)
				: EnergyDevice(name, powerConsumption) {}

	void activate() override { isActive = true; }
	void deactivate() override { isActive = false; }

	void displayStatus() const override {
		std::cout << "Appliance: " << name << ": "
			<< (isActive ? "Running": "Off")<< ", "
			<< "Power: " << (isActive ? powerConsumption : 0) << "W\n";}

	double calculatePower() const override { return isActive ? powerConsumption : 0; }};

class SolarPanel : public EnergyDevice {
	double efficiency; // REMEMBER PRECENTAGE
	double sunlevel; //also precentage

public:
	SolarPanel(const std::string& name, double maxOutput, double efficiency = 20.0)
		:EnergyDevice(name, maxOutput), efficiency(efficiency), sunlevel(99.0) {}

	void activate() override { isActive = true; }
	void deactivate() override { isActive = false; }

	void displayStatus() const override {
		std::cout << "Solar " << name << "; "
			<< (isActive ? "Generating" : "Idle") << ", "
			<< "Output: " << calculatePower() << "W\n";
	}

	double calculatePower() const override {
		return isActive ? powerConsumption * efficiency / 100.0 * sunlevel / 100.0 : 0; // and "()" if be some problems
	}

	void setsunlevel(double level) {
		if (level < 0) sunlevel = 0;
		else if (level > 100) sunlevel = 100;
		else sunlevel = level; 
	}
};


class House {
	std::vector<std::shared_ptr<EnergyDevice>> devices;

	std::shared_ptr<EnergyDevice> createDeviceByType(
		const std::string& type,
		const std::string& name,
		double powerConsumption,
		bool isActive = false)

	{
		if (type == "light") return std::make_shared<Light>(name, powerConsumption);
		if (type == "termostat") return std::make_shared<Termostat>(name, powerConsumption);
		if (type == "appliance") return std::make_shared<Appliance>(name, powerConsumption);
		if (type == "solar") return std::make_shared<SolarPanel>(name, powerConsumption);
		return nullptr;
	}

public:
	void readFile(const std::string& filename)
	{
		std::ifstream file(filename);
		if (!file) {
			std::cerr << "Can't read file. " << std::endl;
			return;
		}

		std::string line, type, name, state;
		double powerConsumption = 0.0;

		while (std::getline(file, line))
		{
			if (line.rfind("@Type: ", 0) == 0)
			{
				type = line.substr(6);
				type.erase(0, type.find_first_not_of(" \t\r\n"));
				type.erase(type.find_last_not_of(" \t\r\n") + 1);
			}
			else if (line.rfind("@Name: ", 0) == 0)
			{
				name = line.substr(6);
				type.erase(0, type.find_first_not_of(" \t\r\n"));
				type.erase(type.find_last_not_of(" \t\r\n") + 1);
			}
			else if (line.rfind("@Power: ", 0) == 0)
			{
				powerConsumption = std::stood(line.substr(7));
			}
			else if (line.rfind("@State: ", 0) == 0)
			{
				name = line.substr(7);
				type.erase(0, type.find_first_not_of(" \t\r\n"));
				type.erase(type.find_last_not_of(" \t\r\n") + 1);

				auto device = createDeviceByType(type, name, powerConsumption, state == "active");
				
				if (device)
				{
					if (state == "active") device->activate();
					devices.push_back(device);
				}
			}
		}
	}







};
