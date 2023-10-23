
#include "pugixml.hpp"
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>
typedef struct{
    std::string name;
    int i;
    std::pair<double, double> Point1;
    std::pair<double, double> Point2;
}part;
extern std::vector<part> allpart;
void makeallpart();