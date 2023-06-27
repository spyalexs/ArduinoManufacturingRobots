#ifndef command_h
#define command_h

#include <Arduino.h>
#include "WiFiUdp.h"

#include "RobotContainer.h"
#include "Communicator.h"

class Command{
  public:
    Command(RobotContainer* MC, Communicator* CC, String name);
    Command(); //default constructor

    void setStatus(int status);

    void abort();
    void confirmCommand();

    virtual void startup() = 0;
    virtual void cycle() = 0;
    virtual bool ifEnd() = 0;
    virtual void cleanup() = 0;

    void run();
    void superCycle();

    bool isCompleted();

  protected:
    RobotContainer* mp_MC;
    Communicator* mp_CC;

    String m_name;

  private:
    int m_status = 0; //governs when the command is in terms of its lifecycle
    bool m_completed = false; //if the command has been completed and is ready to be removed

    void updateStatus(int Status);

    double m_confirmationRequestTime = 0.0;// the time of the confirmation request
    double m_confirmationPatience = 1.0; // how long to wait to see if confirmation needs requesting
};


#endif