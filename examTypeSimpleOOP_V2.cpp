#include <iostream>
#include <string>
#include <utility>

class Camera {
private:
    std::string serialNum;
    std::string brand;

public:
    Camera() = default;

    Camera(std::string _serialNum, std::string _brand)
        : serialNum(std::move(_serialNum)),
          brand(std::move(_brand)) {}

    // 1. Destructor
    virtual ~Camera() = default;

    // 2. Copy Constructor
    Camera(const Camera& other)
        : serialNum(other.serialNum),
          brand(other.brand) {
        std::cout << "[Camera] Copy constructor\n";
    }

    // 3. Copy Assignment Operator
    Camera& operator=(const Camera& other) {
        std::cout << "[Camera] Copy assignment\n";
        if (this != &other) {
            serialNum = other.serialNum;
            brand     = other.brand;
        }
        return *this;
    }

    // 4. Move Constructor
    Camera(Camera&& other) noexcept
        : serialNum(std::move(other.serialNum)),
          brand(std::move(other.brand)) {
        std::cout << "[Camera] Move constructor\n";
    }

    // 5. Move Assignment Operator
    Camera& operator=(Camera&& other) noexcept {
        std::cout << "[Camera] Move assignment\n";
        if (this != &other) {
            serialNum = std::move(other.serialNum);
            brand     = std::move(other.brand);
        }
        return *this;
    }

    void setSerialNum(const std::string& _serialNum) { serialNum = _serialNum; }
    void setBrand(const std::string& _brand)         { brand = _brand; }
    const std::string& getSerialNum() const          { return serialNum; }
    const std::string& getBrand()     const          { return brand; }

    virtual void printCameraInfo() = 0;
};

class DigitalCamera : public Camera {
private:
    int  pixelInM;
    bool isCMOS;

public:
    DigitalCamera()
        : Camera("000000", "XXX"),
          pixelInM(1),
          isCMOS(false) {}

    DigitalCamera(int _pixelInM)
        : Camera("000000", "XXX"),
          pixelInM(_pixelInM),
          isCMOS(false) {}

    DigitalCamera(std::string _serialNum, std::string _brand, int _pixelInM, bool _isCMOS)
        : Camera(std::move(_serialNum), std::move(_brand)),
          pixelInM(_pixelInM),
          isCMOS(_isCMOS) {}

    // 1. Destructor
    ~DigitalCamera() override = default;

    // 2. Copy Constructor
    DigitalCamera(const DigitalCamera& other)
        : Camera(other),               // invokes Camera copy constructor
          pixelInM(other.pixelInM),
          isCMOS(other.isCMOS) {
        std::cout << "[DigitalCamera] Copy constructor\n";
    }

    // 3. Copy Assignment Operator
    DigitalCamera& operator=(const DigitalCamera& other) {
        std::cout << "[DigitalCamera] Copy assignment\n";
        if (this != &other) {
            Camera::operator=(other);  // invokes Camera copy assignment
            pixelInM = other.pixelInM;
            isCMOS   = other.isCMOS;
        }
        return *this;
    }

    // 4. Move Constructor
    DigitalCamera(DigitalCamera&& other) noexcept
        : Camera(std::move(other)),    // invokes Camera move constructor
          pixelInM(other.pixelInM),
          isCMOS(other.isCMOS) {
        std::cout << "[DigitalCamera] Move constructor\n";
    }

    // 5. Move Assignment Operator
    DigitalCamera& operator=(DigitalCamera&& other) noexcept {
        std::cout << "[DigitalCamera] Move assignment\n";
        if (this != &other) {
            Camera::operator=(std::move(other)); // invokes Camera move assignment
            pixelInM = other.pixelInM;
            isCMOS   = other.isCMOS;
        }
        return *this;
    }

    void setPixelInM(int _pixelInM)  { pixelInM = _pixelInM; }
    void setIsCMOS(bool _isCMOS)     { isCMOS = _isCMOS; }
    int  getPixelInM() const         { return pixelInM; }
    bool getIsCMOS()   const         { return isCMOS; }

    void printCameraInfo() override {
        std::cout << "This is a digital camera with the following info:\n";
        std::cout << "Serial Number: " << getSerialNum()               << '\n';
        std::cout << "Brand: "         << getBrand()                   << '\n';
        std::cout << "Pixel: "         << getPixelInM()                << " (M)\n";
        std::cout << "CMOS: "          << (getIsCMOS() ? "YES" : "NO") << '\n';
    }
};

int main() {
    std::cout << "--- Construct A ---\n";
    DigitalCamera A("CA_1974", "Canon", 450, true);
    A.printCameraInfo();

    std::cout << "\n--- Copy construct B from A ---\n";
    DigitalCamera B(A);
    B.printCameraInfo();

    std::cout << "\n--- Copy assign C = A ---\n";
    DigitalCamera C;
    C = A;
    C.printCameraInfo();

    std::cout << "\n--- Move construct D from A ---\n";
    DigitalCamera D(std::move(A));
    D.printCameraInfo();

    std::cout << "\n--- Move assign C = D ---\n";
    C = std::move(D);
    C.printCameraInfo();

    return 0;
}
