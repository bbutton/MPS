#include  "DBLogger.h"
#include  "string_util.hh"

using namespace std;

void DBLogger::logError(const string & errorMsg, const string & packageId)
{
  cerr << "DBLogger: " << errorMsg << endl;
  try
  {
    Query query = connection->query();
    query << "insert into MPSLog (OrderId, EventType, Event) VALUES ("
          << "'" << packageId << "', "
          << "'E'" << ", "
          << "'" << errorMsg << "')";
    query.execute();
  }
  catch(exception & e)
  {
    cout << "DB Exception: " << e.what() << endl;
  }
}

void DBLogger::packageComplete(const string & packageId)
{
  cerr << "DBLogger: package complete <" << packageId << ">" << endl;
  try
  {
    Query query = connection->query();
    query << "insert into MPSLog (OrderId, EventType, Event) VALUES ("
          << "'" << packageId << "', "
          << "'I'" << ", "
          << "'" << "Package Completed" << "')";
    query.execute();
  }
  catch(exception & e)
  {
    cout << "DB Package Complete exception: " << e.what() << endl;
  }
}
