#ifndef connection_h
#define connection_h

#include <string>

class Connection{
  // a data type to hold connections
  public:
    Connection();
    void Connected(int number, std::string m_address);

    int m_number;
    std::string m_address;
    bool m_isConnected;
};

#endif