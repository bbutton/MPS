#include  "SPM.h"
#include  "PLC.h"
#include  "Itinerary.h"

#include  <sstream>
#include  <iostream>
#include  <iomanip>
using namespace std;

namespace
{
  struct ItineraryWriteCommand : public SPM::WriteCommand
  {
      ItineraryWriteCommand(const Itinerary & itinerary_)
        : itinerary(itinerary_)
      {
        insertionData = createInsertionData(itinerary);
      }

      void write(PLC & plc)
      {
        plc.writeMessage(insertion_addr,
                         insertionData.length(),
                         insertionData.data());
        
        plc.writeMessage(order_id_addr,
                         itinerary.getProductId().length(),
                         itinerary.getProductId().data());
      }
      
      string ItineraryWriteCommand::createInsertionData(const Itinerary & itinerary) const
      {
        ostringstream ostr(ios::out | ios::binary);
        ostr.setf(ios::uppercase);
        
        ostr.put(0);
        ostr.put(itinerary.getInsertionsAtStation1());
        ostr.put(0);
        ostr.put(itinerary.getInsertionsAtStation2());
        ostr.put(0);
        ostr.put(itinerary.getInsertionsAtStation3());
        ostr.put(0);
        ostr.put(itinerary.getInsertionsAtStation4());
        ostr.put(0);
        ostr.put(itinerary.getInsertionsAtStation5());
        ostr.put(0);
        ostr.put(itinerary.getInsertionsAtStation6());
        
        return ostr.str();
      }

      string insertionData;
      const Itinerary & itinerary;
  };

  struct SingleWordCommand : public SPM::WriteCommand, public SPM::ReadCommand
  {
      SingleWordCommand(unsigned short startingAddress_, const char * data_)
        : startingAddress(startingAddress_),
          data(const_cast<char *>(data_))
      {
      }

      SingleWordCommand(unsigned short startingAddress_, char * data_)
        : startingAddress(startingAddress_),
          data(data_)
      {
      }
      
      void write(PLC & plc)
      {
        plc.writeMessage(startingAddress, 2, data);
      }

      unsigned char read(PLC & plc)
      {
        plc.readMessage(startingAddress, 2, data);
        return data[1];
      }

      unsigned short  startingAddress;
      char *    data;
  };
};

void SPM::writeItinerary(const Itinerary & itinerary)
{
  ItineraryWriteCommand writeCommand(itinerary);
  write(writeCommand);
}

void SPM::writePackageCreationError()
{
  static const char packageCreationError[] = { 0x00, 0x01 };
  SingleWordCommand writeCommand(WriteCommand::errors_addr, packageCreationError);
  write(writeCommand);
}

void SPM::writePackageVerifyError()
{
  static const char packageVerifyError[] = { 0x00, 0x02 };
  SingleWordCommand writeCommand(WriteCommand::errors_addr, packageVerifyError);
  write(writeCommand);
}

void SPM::writeGeneralError()
{
  static const char packageGeneralError[] = { 0x00, 0x03 };
  SingleWordCommand writeCommand(WriteCommand::errors_addr, packageGeneralError);
  write(writeCommand);
}

void SPM::acknowledgePackageHandshake()
{
  static const char handshakeBit[] = { 0x00, 0x02 };
  SingleWordCommand writeCommand(WriteCommand::control_word_addr, handshakeBit);
  write(writeCommand);
}

unsigned char SPM::readMachineStatusWord()
{
  char machineStatusWord[2];
  SingleWordCommand readCommand(ReadCommand::status_word_addr, machineStatusWord);
  return read(readCommand);
}

void SPM::write(WriteCommand & writeCommand)
{
  string exceptionMsg;
  
  for(int i = 0; i < max_tries; i++)
  {
    try
    {
      writeCommand.write(*plc);
      return;
    }
    catch(PLC::CommunicationException & e)
    {
      exceptionMsg = e.what();
    }
  }
  
  throw SystemPackagingMachine::CommunicationsException(exceptionMsg);
}

unsigned char SPM::read(ReadCommand & readCommand)
{
  string exceptionMsg;

  for(int i = 0; i < max_tries; i++)
  {
    try
    {
      unsigned char data = readCommand.read(*plc);
      return data;
    }
    catch(PLC::CommunicationException & e)
    {
      exceptionMsg = e.what();
    }
  }

  throw SystemPackagingMachine::CommunicationsException(exceptionMsg);
}

SPM::WriteCommand::~WriteCommand() {}
SPM::ReadCommand::~ReadCommand() {}
