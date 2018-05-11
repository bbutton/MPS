#include  <TestHarness.h>
#include  "TestMacros.h"

#include  "MockParallelPort.h"
#include  "HardwareParallelPort.h"
#include  "ParallelPortPrinter.h"

#include  <cstring>
#include  <boost/shared_array.hpp>

TEST(writeDataToPort, ParallelPortTest)
{
  boost::shared_ptr<MockParallelPort> parallelPort(new MockParallelPort);
  ParallelPortPrinter printer(parallelPort);

  printer.print("asdf");

  LONGS_EQUAL(4, parallelPort->savedLength);
  CHECK(memcmp("asdf", parallelPort->savedBuf, 4) == 0);
}

TEST(writeAbortToPort, ParallelPortTest)
{
  boost::shared_ptr<MockParallelPort> parallelPort(new MockParallelPort);
  ParallelPortPrinter printer(parallelPort);

  printer.abort();

  LONGS_EQUAL(2, parallelPort->savedLength);

  const char knownGood[] = { 0x1b, '*' };
  BUFS_EQUAL(knownGood, parallelPort->savedBuf, 2);
}

TEST(shortWriteThrowsException, ParallelPortTest)
{
  boost::shared_ptr<HardwareParallelPort> parallelPort(new HardwareParallelPort("/dev/lp0"));
  ParallelPortPrinter printer(parallelPort);

  boost::shared_array<char> data(new char[4096 * 4096]);
  ::memset(data.get(), 'a', 4096 * 4096);
  data[4096*4096-1] = 0x00;
  
  try
  {
    printer.print(data.get());
    FAIL("Did not throw PrintingFailedException");
  }
  catch(ParallelPortPrinter::PrintingFailedException & expectedException)
  {
    STRCMP_EQUAL("ParallelPortPrinter: Failed to write data to parallel port. Check printer.", expectedException.what());
  }
}
