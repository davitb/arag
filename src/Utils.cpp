#include <iostream>
#include "Utils.h"

using namespace std;
using namespace cache_server;

void Utils::printVector(const std::vector<std::string>& vec)
{
    cout << "vector size: " << vec.size() << endl;
    for (auto str : vec) {
        cout << str << " ";
    }
    cout << endl;
}

int Utils::convertToInt(std::string val)
{
    try {
        size_t idx = 0;
        int intVal = std::stoi(val, &idx);
        if (idx != val.length()) {
            throw invalid_argument("Must be a number");
        }
        
        return intVal;
    }
    catch (invalid_argument& e) {
    }
    
    throw invalid_argument("Must be a number");
}

