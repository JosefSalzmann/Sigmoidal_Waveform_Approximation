/*
  File: vdd_potential.h
  Author: Josef Salzmann <josef.salzmann@tuwien.ac.at>
  Copyright (c) 2022

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VDD_POTENTIAL_H
#define VDD_POTENTIAL_H

#include "logic_gate.h"

// not really a transitionsource, only needed for determining initial values
class VDDPotential : public TransitionSource {
   private:
	std::vector<NORGateInput> subscribers;

   public:
	VDDPotential() : subscribers{} {};
	~VDDPotential();
	std::string GetOutputName() override;
	InitialValue GetInitialOutputValue() override;
	std::vector<NORGateInput>& GetSubscribers() override;
};
#endif