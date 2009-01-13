/****************************************************************************

    effectstackmodel.hpp

    Copyright (C) 2007 Lars Luthman <lars.luthman@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef EFFECTSTACKMODEL_HPP
#define EFFECTSTACKMODEL_HPP

#include <string>
#include <vector>

#include "effectmodel.hpp"


class EffectStackModel {
public:
  
  EffectStackModel();
  
  bool insert_effect(size_t pos, const std::string& name);
  bool remove_effect(size_t pos);
  bool bypass_effect(size_t pos, bool bypass);
  
  const std::vector<EffectModel>& get_effects() const;
  
protected:
  
  std::vector<EffectModel> m_effects;

};


#endif
