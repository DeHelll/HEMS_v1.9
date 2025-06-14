#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <iomanip>

class EnergyDevice {
protected:
    std::string name;
    double powerConsumption; 
    bool isActive;

public:
    EnergyDevice(const std::string& name, double powerConsumption, bool isActive = false)
        : name(name), powerConsumption(powerConsumption), isActive(isActive) {}

    virtual ~EnergyDevice() = default;

    //core functionality
    virtual void togglePower(bool on) { isActive = on;}
    virtual void activate() {togglePower(true);}
    virtual void deactivate() { togglePower(false); }

    //status and power calculation
    virtual void displayStatus() const = 0;
    virtual double calculatePower() const = 0;
    virtual double getNetEn() const { return calculatePower() * 1.0; }
    
    virtual void updateH() {}
    //getters
    std::string getname() const { return name; }
    bool isDeviceActive() const { return isActive; }
};

class Light : public EnergyDevice {
    double brightness;
public:
    Light(const std::string& name, double powerConsumption, double brightness, bool isActive = false)
        : EnergyDevice(name, powerConsumption, isActive), brightness(brightness) {}

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

class Thermostat : public EnergyDevice {
    double currentTemp;
    double targetTemp;
public:
    Thermostat(const std::string& name, double powerConsumption, double currentTemp = 20.0, double targetTemp = 24.0)
        : EnergyDevice(name, powerConsumption), currentTemp(currentTemp), targetTemp(targetTemp) {}

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

    void updateH() override {
        if (isActive && currentTemp < targetTemp) {
            currentTemp = std::min(currentTemp + 2.0, targetTemp);
        }
    }

private:
    void adjustTemp() {
        if (currentTemp < targetTemp) {
            std::cout << name << " heating to " << targetTemp << "°C...\n";
        }
    }
};

class Appliance : public EnergyDevice {
public:
    Appliance(const std::string& name, double powerConsumption)
        : EnergyDevice(name, powerConsumption) {}

    void activate() override { isActive = true; }
    void deactivate() override { isActive = false; }

    void displayStatus() const override {
        std::cout << "Appliance: " << name << ": "
            << (isActive ? "Running" : "Off") << ", "
            << "Power: " << (isActive ? powerConsumption : 0) << "W\n";
    }

    double calculatePower() const override {
        return isActive ? powerConsumption : 0;
    }
};

class SolarPanel : public EnergyDevice {
    double efficiency; 
    double sunlevel;

public:
    SolarPanel(const std::string& name, double maxOutput, double efficiency = 20.0)
        : EnergyDevice(name, maxOutput), efficiency(efficiency), sunlevel(100.0) {}

    void displayStatus() const override {
        std::cout << "Solar " << name << ": "
            << (isActive ? "Generating" : "Idle") << ", "
            << "Output: " << calculatePower() << "W\n";
    }

    double calculatePower() const override {
        return isActive ? powerConsumption * (efficiency / 100.0) * (sunlevel / 100.0) : 0;
    }

    double getNetEn() const override { return -calculatePower() * 1; }

    void setsunlevel(double level) {
        if (level < 0) sunlevel = 0;
        else if (level > 100) sunlevel = 100;
        else sunlevel = level;
    }
};

class SmartOutlet : public Appliance {
    double totalPower;


public:
    SmartOutlet(const std::string& name, double powerConsumption)
        : Appliance(name, powerConsumption), totalPower(0.0) {};

    void updateH() override {
        if (isActive) {
            totalPower += calculatePower() * 1;
        }
    }

    void displayStatus() const override {
        std::cout << "Smart Outlet: " << name << ": "
            << (isActive ? "Running" : "Off") << ", "
            << "Total Power: " << totalPower << "W\n";
    }

    void resetTotalPower() { totalPower = 0.0; }
};

class BatteryStorage : public EnergyDevice {
public:
    enum class Mode { Idle, Charging, Discharging };

private:
    double capacityWh;
    double currentChargeWh;
    double maxChargeRateW;
    double maxDischargeRateW;
    Mode currentMode;

public:
    BatteryStorage(const std::string& name, double capacity, double maxChargeRate, double maxDischargeRate, double initialCharge = 0.0)
        : EnergyDevice(name, 0.0), capacityWh(capacity),currentChargeWh(initialCharge),maxChargeRateW(maxChargeRate),maxDischargeRateW(maxDischargeRate),currentMode(Mode::Idle) {}

    void setMode(Mode mode) { currentMode = mode; }
    void activate() override { currentMode = Mode::Discharging; }
    void deactivate() override { currentMode = Mode::Idle; }

    void displayStatus() const override {
        std::string modeStr;
        switch (currentMode) {
            case Mode::Charging: modeStr = "Charging"; break;
            case Mode::Discharging: modeStr = "Discharging"; break;
            default: modeStr = "Idle"; break;
        }

        std::cout << "Battery: " << name << ": " << modeStr
                  << ", Charge: " << std::fixed << std::setprecision(1)
                  << currentChargeWh << "Wh/" << capacityWh << "Wh ("
                  << (currentChargeWh / capacityWh) * 100 << "%)\n";
    }

    double calculatePower() const override {
        switch (currentMode) {
            case Mode::Charging: return maxChargeRateW;
            case Mode::Discharging: return -maxDischargeRateW;
            default: return 0;
        }
    }

    void updateH() override {
        switch (currentMode) {
            case Mode::Charging:
                currentChargeWh = std::min(currentChargeWh + maxChargeRateW, capacityWh);
                break;
            case Mode::Discharging:
                currentChargeWh = std::max(currentChargeWh - maxDischargeRateW, 0.0);
                break;
            default: break;
        }
    }

    double getNetEn() const override {
        return calculatePower();  
    }

    double getChargeLevel() const { return currentChargeWh; }
};

class House {
    std::vector<std::shared_ptr<EnergyDevice>> devices;

    std::shared_ptr<EnergyDevice> createDeviceByType(
        const std::string& type,
        const std::string& name,
        double powerConsumption)
    {
        if (type == "light")
            return std::make_shared<Light>(name, powerConsumption, 100.0);
        if (type == "termostat")
            return std::make_shared<Thermostat>(name, powerConsumption);
        if (type == "appliance")
            return std::make_shared<Appliance>(name, powerConsumption);
        if (type == "solar")
            return std::make_shared<SolarPanel>(name, powerConsumption);
        return nullptr;
    }

public:
    void addDevice(const std::shared_ptr<EnergyDevice>& device) {
        devices.push_back(device);
    }

    void readFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Can't read file." << std::endl;
            return;
        }

        std::string line, type, name, state;
        double powerConsumption = 0.0;

        auto trim = [](std::string& s) {
            s.erase(0, s.find_first_not_of(" \t\r\n"));
            s.erase(s.find_last_not_of(" \t\r\n") + 1);
            };

        while (std::getline(file, line)) {
            if (line.rfind("@Type: ", 0) == 0) {
                type = line.substr(6);
                trim(type);
            }
            else if (line.rfind("@Name: ", 0) == 0) {
                name = line.substr(6);
                trim(name);
            }
            else if (line.rfind("@Power: ", 0) == 0) {
                powerConsumption = std::stod(line.substr(7));
            }
            else if (line.rfind("@State: ", 0) == 0) {
                state = line.substr(7);
                trim(state);

                auto device = createDeviceByType(type, name, powerConsumption);
                if (device) {
                    if (state == "active") {
                        device->activate();
                    }
                    devices.push_back(device);
                }
            }
        }
    }

    void simulateHour() {
        for (auto& device : devices) {
            device->updateH();
        }
    }

    double calculateTotalPower() const {
        double totalPower = 0.0;
        for (const auto& device : devices) {
            totalPower += device->calculatePower();
        }
        return totalPower;
    }

    double calculateCost(double buyRate, double sellRate) const {
        double netEnergy = calculateTotalPower();
        double netKWh = netEnergy / 1000.0;

        if (netKWh > 0) {
            return netKWh * buyRate;
        }
        else {
            return -netKWh * sellRate;
        }
    }
        void runInterSess() {
            std::cout << "\nHouse Energy Management System\n"
                << "Total devices: " << devices.size() << "\n\n";

            int index = 1;
            for (const auto& device : devices) {
                std::cout << index++ << ". ";
                device->displayStatus();
            }

            double totalPower = 0.0;
            for (const auto& device : devices) {
                totalPower += device->calculatePower();
            }
            std::cout << "\nTotal Power Usage: " << std::fixed << std::setprecision(0) << totalPower << "W\n";


            simulateHour();

            const double buyRate = 0.15;
            const double sellRate = 0.05;

            double netEnergy = calculateTotalPower();
            double cost = calculateCost(buyRate, sellRate);

            std::cout << "\nAfter 1 hour simulation: \n";


            std::cout << "Net Energy: " << std::fixed << std::setprecision(0) << netEnergy << "Wh ("
                << std::fixed << std::setprecision(1) << netEnergy / 1000.0 << " kWh)\n";


            std::cout << "Estimated Cost: $" << std::fixed << std::setprecision(2) << cost
                << " (" << (cost > 0 ? "Paid" : "Earned") << ")\n";


        }
    };

int main() {
    House myHouse;
    myHouse.readFile("devices.txt");

    auto battery = std::make_shared<BatteryStorage>("Home Battery", 5000, 1000, 1500, 500);
    battery->activate();
    myHouse.addDevice(battery);

    myHouse.runInterSess();
    return 0;
}