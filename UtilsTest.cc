#include  <TestHarness.h>

#include  "Utils.h"

#include  <map>
#include  <string>
using namespace std;
#include  <sqlplus.hh>

namespace
{
  struct Fixture
  {
      Connection connection;
      Fixture()
        : connection(use_exceptions)
      {
        connection.connect("MPS", "localhost", "MPS_Admin", "MPS_Admin");
      }

      ~Fixture()
      {
        connection.close();
      }
  };
}

TEST(getSerialPortNames, UtilsTest)
{
  Fixture f;
  
  Utils utils(f.connection);
  string scanPort = utils.getPortFor("scan");
  STRCMP_EQUAL("/dev/ttyP5", scanPort.c_str());
}

TEST(serialPortNotFound, UtilsTest)
{
  Fixture f;
  
  Utils utils(f.connection);

  try
  {
    string scanPort = utils.getPortFor("me");
    FAIL("Did not throw ConfigurationException");
  }
  catch(Utils::ConfigurationException & expectedException)
  {
    STRCMP_EQUAL("Configuration error: Unknown user requesting port <me>", expectedException.what());
  }
}
