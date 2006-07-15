#ifndef OSCCONTROLLER_HPP
#define OSCCONTROLLER_HPP

#include <string>


class LV2Host;


class OSCController {
public:
  
  OSCController(LV2Host& host);
  
  void start();
  void stop();
  
  std::string get_url() const;
  
protected:
  
  LV2Host& m_host;
  
};


#endif
