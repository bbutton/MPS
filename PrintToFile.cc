#include  "DBPackageFactory.cc"
#include  "RealPackage.h"

#include  <sqlplus.hh>
#include  <iostream>
#include  <fstream>
#include  <string>
#include  <exception>
#include  <boost/shared_ptr.hpp>

using namespace std;

int main(int argc, char ** argv)
{
  if(argc < 2 )
  {
    cerr << "usage: <packageId> <outputFile -- optional>" << endl;
    return 1;
  }
  
  try
  {
    boost::shared_ptr<Connection> connection(new Connection(use_exceptions));
    connection->connect("MPS", "localhost", "MPS_Admin", "MPS_Admin");
    
    DBPackageFactory factory(connection);
    
    string packageId = argv[1];
    PackagePtr package = factory.create(packageId);
    
    string labelData = package->getLabelData();

    ofstream printFile(argv[2], ios::out | ios::binary);
    printFile.write(labelData.data(), labelData.size());
  }
  catch(exception & e)
  {
    cout << "Exception: " << e.what() << endl;
  }
    
  return 0;
}
