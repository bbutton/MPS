class ResettableStream
{
public:
  void write(const std::string & data)
  {
    resetStream();
    ostr << data << flush;
    closeStream();
  }

  void resetStream()
  {
    ostr.open(fileName);

    ostr.str("");
  }

  void closeStream()
  {
    ostr.close();
  }
};
