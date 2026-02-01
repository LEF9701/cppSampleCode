#include<iostream>

class testClass{
    private:
        double x{},y{};
    public:
        double getX() const { return x; }
        double getY() const { return y; }
        void setX(double _x) { x = _x; }
        void setY(double _y) { y = _y; }
};

int main(){
    testClass test[10];

    test[1].setX(3.14);
    test[1].setY(1.59);
    test[2].setX(1.414);
    test[2].setY(1.592);

    for(int i=0; i < 10; i++){
        std::cout << "[" << i << "]value of x: " << test[i].getX() << std::endl;
        std::cout << "[" << i << "]value of y: " << test[i].getY() << std::endl;
    }

    std::cout << "+++++++++++++++++++++++++++++++++++++++" << std::endl;


    std::vector<testClass> Vtest(10);

    Vtest.at(0).setX(114.514);
    Vtest.at(0).setY(1919.89);

    std::cout << "(Vtest)value of x: " << Vtest.at(0).getX() << std::endl;
    std::cout << "(Vtest)value of y: " << Vtest.at(0).getY() << std::endl;
    
    std::cout << "+++++++++++++++++++++++++++++++++++++++" << std::endl;

size_t idx = 0;
for (const testClass& i : Vtest) {
    std::cout << "[" << idx << "] value of x: " << i.getX() << std::endl;
    std::cout << "[" << idx << "] value of y: " << i.getY() << std::endl;
    ++idx;
}



}
