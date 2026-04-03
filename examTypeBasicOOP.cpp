#include <iostream>
#include <string>
#include <utility>

class Camera{
private:
    std::string serialNum;
    std::string brand;

public:
    Camera() = default;
    Camera(std::string _serialNum, std::string _brand)
        : serialNum(std::move(_serialNum)),
          brand(std::move(_brand)) {}
    virtual ~Camera() = default;

    void setSerialNum(const std::string& _serialNum){serialNum = _serialNum;}
    void setBrand(const std::string& _brand){brand = _brand;}
    const std::string& getSerialNum() const {return serialNum;}
    const std::string& getBrand() const {return brand;}
    virtual void printCameraInfo() = 0;
};

class DigitalCamera : public Camera{
private:
    int pixelInM;
    bool isCMOS;

public:
    DigitalCamera()
        : Camera("000000", "XXX"),
          pixelInM(1),
          isCMOS(false) {}
    DigitalCamera(const DigitalCamera& other)
        : Camera(other),
          pixelInM(other.pixelInM),
          isCMOS(other.isCMOS) {}
    DigitalCamera(int _pixelInM)
        : Camera("000000", "XXX"),
          pixelInM(_pixelInM),
          isCMOS(false) {}
    DigitalCamera(std::string _serialNum, std::string _brand, int _pixelInM, bool _isCMOS)
        : Camera(std::move(_serialNum), std::move(_brand)),
          pixelInM(_pixelInM),
          isCMOS(_isCMOS) {}
    ~DigitalCamera() = default;

    void setPixelInM(int _pixelInM){pixelInM = _pixelInM;}
    void setIsCMOS(bool _isCMOS){isCMOS = _isCMOS;}
    int getPixelInM() const {return pixelInM;}
    bool getIsCMOS() const {return isCMOS;}
    void printCameraInfo() override {
        std::cout << "This is a digital camera with the following info:\n";
        std::cout << "Serial Number: " << getSerialNum() << '\n';
        std::cout << "Brand: " << getBrand() << '\n';
        std::cout << "Pixel: " << getPixelInM() << " (M)\n";
        std::cout << "CMOS: " << (getIsCMOS() ? "YES" : "NO") << '\n';
    }
};

int main(){
    DigitalCamera A("CA_1974", "Canon", 450, true);
    A.printCameraInfo();

    return 0;
}
