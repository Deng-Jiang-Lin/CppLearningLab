#include <iostream>
#include <string>

class Base {
public:
  virtual ~Base() = default;
};

class Derived : public Base {
public:
  void show() { std::cout << "Derived class" << std::endl; }
};

void demonstrateTypeCasting() {
  // 1. static_cast
  int a = 10;
  double b = static_cast<double>(a); // Basic type conversion
  std::cout << "static_cast: " << b << std::endl;

  Base *base = new Derived();
  Derived *derived = static_cast<Derived *>(base); // Safe downcasting
  derived->show();

  // 2. dynamic_cast
  Base *unrelated = new Base();
  Derived *invalid = dynamic_cast<Derived *>(unrelated); // Invalid conversion
  if (!invalid) {
    std::cout << "dynamic_cast: Invalid conversion" << std::endl;
  }

  // 3. const_cast
  const int num = 42;
  int *modifiable = const_cast<int *>(&num); // Remove const qualifier
  *modifiable = 100;                         // Undefined behavior
  std::cout << "const_cast: " << *modifiable << std::endl;

  // 4. reinterpret_cast
  int value = 65;
  union {
    int intValue;
    char charValue[sizeof(int)];
  } converter;

  converter.intValue = value;
  std::cout << "reinterpret_cast: First byte of value = "
            << converter.charValue[0] << std::endl;

  delete base;
  delete unrelated;
}

int main() {
  demonstrateTypeCasting();
  return 0;
}