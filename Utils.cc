#include  "Utils.h"

#include  <sqlplus.hh>
#include  <exception>
#include  <assert.h>

using namespace std;

Utils::Utils(Connection & connection_)
  : dbConnection(connection_)
{
}

Utils::~Utils()
{
}

string Utils::getPortFor(std::string portUser)
{
  Query query = dbConnection.query();
  query << "select port_name from port_data where user = '" << portUser << "'";

  Result result = query.store();
  if(result.size() == 0)
  {
    string msg = "Unknown user requesting port <" + portUser + ">";
    throw ConfigurationException(msg);
  }
  Row row = result[0];
  return row[0].get_string();
}
