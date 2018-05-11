#include  <sqlplus.hh>

#include  "Utils.h"
#include  "ConveyerBelt.h"
#include  "DBLogger.h"
#include  "HardwareParallelPort.h"
#include  "ParallelPortPrinter.h"
#include  "EventDrivenParallelPortPrinter.h"
#include  "SerialPort.h"
#include  "RemotePLC.h"
#include  "DBPackageFactory.h"
#include  "ThreadManager.h"
#include  "SystemFacade.h"
#include  "MPS.h"
#include  "CreationScanMPSAdapter.h"
#include  "VerifyScanMPSAdapter.h"
#include  "SimpleBarCodeReader.h"
#include  "EventDrivenBarCodeReader.h"
#include  "RTCEncoder.h"
#include  "EventDrivenSPM.h"
#include  "SPM.h"

#include  <ext/stdio_filebuf.h>
#include  <iostream>

using namespace std;

typedef boost::shared_ptr<ParallelPort> ParallelPortPtr;
typedef boost::shared_ptr<Printer>      PrinterPtr;
typedef boost::shared_ptr<EventDrivenParallelPortPrinter> ActivePrinterPtr;
typedef boost::shared_ptr<SerialPort>   SerialPortPtr;
typedef boost::shared_ptr<ConveyerBelt> ConveyerBeltPtr;
typedef boost::shared_ptr<Logger>       LoggerPtr;
typedef boost::shared_ptr<iostream>     SerialStreamPtr;
typedef boost::shared_ptr<PLC>          PLCPtr;
typedef boost::shared_ptr<MPS>          MPSPtr;
typedef boost::shared_ptr<SPM>          SPMPtr;
typedef boost::shared_ptr<SystemFacade> SystemFacadePtr;
typedef boost::shared_ptr<RTCEncoder>   RTCEncoderPtr;
typedef boost::shared_ptr<PackageFactory>           PackageFactoryPtr;
typedef boost::shared_ptr<CreationScanMPSAdapter>   ScanAdapterPtr;
typedef boost::shared_ptr<VerifyScanMPSAdapter>     VerifyAdapterPtr;
typedef boost::shared_ptr<SimpleBarCodeReader>      BarCodeReaderPtr;
typedef boost::shared_ptr<EventDrivenBarCodeReader> ActiveBarCodeReaderPtr;
typedef boost::shared_ptr<EventDrivenRTCEncoder>    ActiveRTCEncoderPtr;
typedef boost::shared_ptr<EventDrivenSPM>           ActiveSPMPtr;

namespace
{
  ActivePrinterPtr createPrinter(Utils & utils, SystemFacadePtr system)
  {
    string parallelPortName = utils.getPortFor("lp");
    ParallelPortPtr parallelPort(new HardwareParallelPort(parallelPortName));
    PrinterPtr parallelPortPrinter(new ParallelPortPrinter(parallelPort));
    return ActivePrinterPtr(new EventDrivenParallelPortPrinter(parallelPortPrinter, system));
  }

  ActiveBarCodeReaderPtr createScanReader(Utils & utils, SystemFacadePtr system)
  {
    string scanPortName = utils.getPortFor("scan");
    SerialPortPtr serialPort(new SerialPort(scanPortName));
    ScanAdapterPtr scanAdapter(new CreationScanMPSAdapter(system));
    BarCodeReaderPtr scanReader(new SimpleBarCodeReader(serialPort, scanAdapter));
    return ActiveBarCodeReaderPtr(new EventDrivenBarCodeReader(scanReader, system));
  }

  ActiveBarCodeReaderPtr createVerifyReader(Utils & utils, SystemFacadePtr system)
  {
    string verifyPortName = utils.getPortFor("verify");
    SerialPortPtr verifyPort(new SerialPort(verifyPortName));
    VerifyAdapterPtr verifyAdapter(new VerifyScanMPSAdapter(system));
    BarCodeReaderPtr verifyReader(new SimpleBarCodeReader(verifyPort, verifyAdapter));
    return ActiveBarCodeReaderPtr(new EventDrivenBarCodeReader(verifyReader, system));
  }

  ActiveSPMPtr createSPM(Utils & utils,
                         const std::string & masterAddress,
                         const string & slaveAddress,
                         SystemFacadePtr system,
			 SerialPortPtr plcSerialPort,
                         iostream & serialStream)
  {
    int masterPLCAddress = atoi(masterAddress.c_str());
    int slavePLCAddress = atoi(slaveAddress.c_str());
    PLCPtr plc(new RemotePLC(slavePLCAddress, plcSerialPort, serialStream));
    SPMPtr spm(new SPM(plc));
    return ActiveSPMPtr(new EventDrivenSPM(spm, system, system));
  }
}

int main(int argc, char ** argv)
{
  if(argc < 3)
  {
    cerr << "usage: " << argv[0] << " <plc master address> <plc slave address> <secondsToRunFor (optional)>" << endl;
    return 1;
  }
  
  int secondsToRunFor = 0;
  if(argc == 4)
  {
    secondsToRunFor = atoi(argv[3]) * 2;
  }
  
  boost::shared_ptr<Connection> connection;
  try
  {
    connection = boost::shared_ptr<Connection>(new Connection(use_exceptions));
    connection->connect("MPS", "localhost", "MPS_Admin", "MPS_Admin");
  }
  catch(exception & e)
  {
    cerr << "FATAL: cannot connect to database - " << e.what() << endl;
    return 1;
  }

   Utils utils(*connection);

  string plcPortName = utils.getPortFor("plc");
  SerialPortPtr plcPort(new SerialPort(plcPortName));
  __gnu_cxx::stdio_filebuf<char> iobuf(plcPort->getFd(), ios::in | ios::out | ios::binary, false, 64);
  iostream serialStream(&iobuf);

  LoggerPtr         logger(new DBLogger(connection));
  SystemFacadePtr   system(new SystemFacade(logger));

  PackageFactoryPtr packageFactory(new DBPackageFactory(connection));
  ConveyerBeltPtr   conveyerBelt(new ConveyerBelt);

  ActivePrinterPtr        printer =                 createPrinter(utils, system);
  ActiveBarCodeReaderPtr  scanReader =              createScanReader(utils, system);
  ActiveBarCodeReaderPtr  verifyReader =            createVerifyReader(utils, system);
  ActiveSPMPtr            systemPackagingMachine =  createSPM(utils, argv[1], argv[2], system, plcPort, serialStream);
  

  RTCEncoderPtr rtcEncoder(new RTCEncoder(2, systemPackagingMachine, secondsToRunFor));
  ActiveRTCEncoderPtr activeEncoder(new EventDrivenRTCEncoder(rtcEncoder, system));
  
  MPSPtr mps(new MPS(packageFactory, conveyerBelt, systemPackagingMachine, printer, logger));
  system->setPackageController(mps);

  ThreadManager threads;
  threads.addThread(system);
  threads.addThread(printer);
  threads.addThread(systemPackagingMachine);
  threads.addThread(scanReader);
  threads.addThread(verifyReader);
  threads.addThread(activeEncoder);
  
  if(secondsToRunFor > 0)
  {
    boost::xtime waitTime;
    boost::xtime_get(&waitTime, boost::TIME_UTC);
    waitTime.sec += secondsToRunFor;

    boost::thread::sleep(waitTime);
    threads.stopAll();
  }

  threads.waitForAllThreadsToExit();

  return 0;
}
