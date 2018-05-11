#include  <TestHarness.h>

#include  "HardwareParallelPort.h"

#include  <boost/shared_array.hpp>

TEST(canOpenPort, ParallelPortTest)
{
  HardwareParallelPort lp("/dev/lp0");
  CHECK(lp.isOpen());
}

TEST(canTimeoutWriteToPort, ParallelPortTest)
{
  HardwareParallelPort lp("/dev/lp0");
  boost::shared_array<char> buf(new char[4096 * 4096]);
  ::memset(buf.get(), 0x00, 4096 * 4096);

  int bytesWritten = lp.write(buf.get(), 4096 * 4096);
  LONGS_EQUAL(0, bytesWritten); 
}
