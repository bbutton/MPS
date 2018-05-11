lib_LIBRARIES		=	libMPS2.a

noinst_PROGRAMS		=	TestDriver PrintToFile

bin_PROGRAMS		=	CSS

CSS_SOURCES		=	main.cc

PrintToFile_SOURCES	=	PrintToFile.cc

TestDriver_SOURCES	= 	ConveyerBeltTest.cc		\
				DBLoggerTest.cc			\
				DBPackageFactoryTest.cc		\
				EventDrivenPrinterTest.cc	\
				EventDrivenSPMTest.cc		\
				LabelFieldTest.cc		\
				LabelFormatterTest.cc		\
				MaxicodeLabelDataTest.cc	\
				MPSTest.cc			\
				PLCTest.cc			\
				ParallelPortPrinterTest.cc	\
				ParallelPortTest.cc		\
				ProducerConsumerQueueTest.cc	\
				RTCEncoderTest.cc		\
				RunnableTest.cc			\
				SerialPortTest.cc		\
				SPMTest.cc			\
				SystemFacadeTest.cc		\
				TimerTest.cc			\
				UtilsTest.cc			\
				TestDriver.cc

LDADD			=	libMPS2.a
AM_LDFLAGS		= 	-L../lib/CppUnitLite/TestHarness -lCppUnitLite -L/usr/local/lib -lsqlplus -lmysqlclient_r -lboost_date_time -lboost_thread -lboost_regex -pthread

libMPS2_a_SOURCES	= 	BarCodeReader.cc			\
				ConveyerBelt.cc				\
				ConveyerBeltIF.cc			\
				CreationScanMPSAdapter.cc		\
				DBLogger.cc				\
				DBPackageFactory.cc			\
				ErrorConsumer.cc			\
				DefaultRunnable.cc			\
				EventDrivenBarCodeReader.cc		\
				EventDrivenParallelPortPrinter.cc	\
				EventDrivenSPM.cc			\
				HardwareParallelPort.cc			\
				LabelField.cc				\
				LabelFormatter.cc			\
				Logger.cc				\
				LoggingConveyerBelt.cc			\
				MaxicodeLabelData.cc			\
				MPS.cc					\
				PLC.cc					\
				Package.cc				\
				PackageControls.cc			\
				ParallelPort.cc				\
				ParallelPortPrinter.cc			\
				PollConsumer.cc				\
				RealPackage.cc				\
				RemotePLC.cc				\
				RTCEncoder.cc				\
				Runnable.cc				\
				SerialPort.cc				\
				SimpleBarCodeReader.cc			\
				SPM.cc					\
				SystemFacade.cc				\
				ThreadManager.cc			\
				Utils.cc				\
				VerifyScanMPSAdapter.cc

#CSS_SOURCES		=	main.cc

AM_CPPFLAGS		=	-I.. -I../lib/CppUnitLite/TestHarness -I/usr/include/mysql -I/usr/local/include -Wno-deprecated -pthread -W

CXXFLAGS		= -g
