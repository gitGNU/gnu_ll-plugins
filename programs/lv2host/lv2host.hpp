#ifndef LV2HOST_HPP
#define LV2HOST_HPP

#include <string>
#include <vector>

#include "lv2-instrument.h"


enum PortDirection {
  InputPort,
  OutputPort
};


enum PortRate {
  ControlRate,
  AudioRate
};


/** A struct that holds information about a port in a LV2 plugin. */
struct LV2Port {
  void* buffer;
  std::string symbol;
  PortDirection direction;
  PortRate rate;
  float default_value;
  float min_value;
  float max_value;
  bool midi;
};


/** A class that loads a single LV2 plugin. */
class LV2Host {
public:
  LV2Host(const std::string& uri, unsigned long frame_rate);
  ~LV2Host();
  
  bool is_valid() const;
  
  const std::vector<LV2Port>& get_ports() const;
  std::vector<LV2Port>& get_ports();
  long get_default_midi_port() const;
  
  void activate();
  void run(unsigned long nframes);
  void deactivate();
  
  char* configure(const char* key, const char* value);
  unsigned int list_used_files(char*** keys, char*** filepaths);
  const LV2_ProgramDescriptor* get_program(unsigned long index);
  void select_program(unsigned long program);
  
  const std::vector<int>& get_midi_map() const;
  
  const std::string& get_gui_path() const;
  
protected:
  
  static void instrument_descriptor_callback(LV2_InstrumentFunction func);
  
  static LV2Host* m_current_object;
  
  void* m_libhandle;
  LV2_Handle m_handle;
  const LV2_Descriptor* m_desc;
  const LV2_InstrumentDescriptor* m_inst_desc;
  
  std::vector<LV2Port> m_ports;
  std::vector<int> m_midimap;
  long m_default_midi_port;
  std::string m_standalonegui;
};


#endif
