#include  "TestHarness.h"

#include  <iostream>
using namespace std;

int main(int argc, char ** argv) {
		TestResult result;
    try
    {
      TestRegistry::runAllTests(result);
    }
    catch(exception & e)
    {
      cerr << "exception occurred: " << e.what() << endl;
      exit(1);
    }
    
    return 0;
}
