#include  <TestHarness.h>

#include  "DBLogger.h"
#include  "SystemFacade.h"
#include  "ThreadManager.h"
#include  "PollConsumer.h"
#include  "RTCEncoder.h"

#include  <sqlplus.hh>

namespace
{
  struct Fixture
  {
      Fixture() : connection(new Connection(use_exceptions))
      {
        connection->connect("MPS", "localhost", "MPS_Admin", "MPS_Admin");

        Query query = connection->query();
        query << "delete from MPSLog";
        query.execute();
      }

    int getLogCount()
    {
      Query query = connection->query();
      query << "select count(*) from MPSLog";
      Result res = query.store();
      return res[0][0];
    }

      boost::shared_ptr<Connection> connection;
  };

  struct MockPollConsumer : public PollConsumer
  {
      MockPollConsumer(boost::shared_ptr<ErrorConsumer> logger_)
        : logger(logger_)
      {
      }
      
      void poll()
      {
        logger->reportPLCCommunicationsError("this is an error");
      }

      boost::shared_ptr<ErrorConsumer> logger;
  };
}

TEST(canLogErrors, DBLoggerTest)
{
  Fixture f;
  
  DBLogger logger(f.connection);
  logger.logError("Test Error Msg");

  Query query = f.connection->query();
  query << "select * from MPSLog";

  Result result = query.store();
  Row row = result[0];

  LONGS_EQUAL(1, result.rows());
  CHECK((long)(row["Timestamp"]) > 0);
  STRCMP_EQUAL("E", row["EventType"].get_string().c_str());
  STRCMP_EQUAL("Test Error Msg", row["Event"].get_string().c_str());
  STRCMP_EQUAL("Unknown Package", row["OrderId"].get_string().c_str());
}

TEST(canStorePackageCompletions, DBLoggerTest)
{
  Fixture f;

  DBLogger logger(f.connection);
  logger.packageComplete("PackageID");
  
  Query query = f.connection->query();
  query << "select * from MPSLog";
  Result result = query.store();

  Row row = result[0];
  LONGS_EQUAL(1, result.rows());
  CHECK((long)(row["Timestamp"]) > 0);
  STRCMP_EQUAL("I", row["EventType"].get_string().c_str());
  STRCMP_EQUAL("Package Completed", row["Event"].get_string().c_str());
  STRCMP_EQUAL("PackageID", row["OrderId"].get_string().c_str());
}

TEST(canLogMsgAllTheWayToDB, DBLoggerTest)
{
  Fixture f;
  boost::shared_ptr<DBLogger> logger(new DBLogger(f.connection));
  boost::shared_ptr<SystemFacade> system(new SystemFacade(logger));

  boost::shared_ptr<MockPollConsumer> mockConsumer(new MockPollConsumer(system));
 boost::shared_ptr<RTCEncoder> rtcEncoder(new RTCEncoder(2, mockConsumer, 10));
  boost::shared_ptr<EventDrivenRTCEncoder>
    activeEncoder(new EventDrivenRTCEncoder(rtcEncoder, system));

  ThreadManager mgr;
  mgr.addThread(system);
  mgr.addThread(activeEncoder);

  cerr << "canLogMsgAllTheWayToDB. Waiting..." << flush;
  mgr.wait(10000);
  cerr << "wait finished" << endl;

  LONGS_EQUAL(1, f.getLogCount());
}
