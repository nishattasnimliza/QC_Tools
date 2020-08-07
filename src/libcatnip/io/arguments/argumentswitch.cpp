
#include "argumentswitch.hpp"
#include "properties/propertyswitch.hpp"
#include <iostream>

using namespace catnip;
using namespace std;

ArgumentSwitch::ArgumentSwitch(void) { 
  auto prop_switch = std::unique_ptr<PropertyObject>(new PropertySwitch);
  propobjs_.push_back(prop_switch);
}

bool ArgumentSwitch::positive(string val) const {
  if (val.compare("ON") == 0 || val.compare("TRUE") == 0 ||
      val.compare("1") == 0) {
    return true;
  } else if (val.compare("OFF") == 0 || val.compare("FALSE") == 0 ||
             val.compare("0") == 0) {
    return false;
  }
  throw invalid_argument("Argument value is unrecognized " + val);
}
