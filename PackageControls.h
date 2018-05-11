#ifndef PACKAGE_CONTROLS_H
#define PACKAGE_CONTROLS_H

#include  <string>

class PackageControls
{
  public:
    virtual ~PackageControls();

    virtual void  packageDetected() = 0;
    virtual void  remoteResetReceived() = 0;
    virtual void  scan(const std::string & barCode) = 0;
    virtual void  verify(const std::string & barCode) = 0;
};

#endif
