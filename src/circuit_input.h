/*
  File: circuit_input.h
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

#ifndef CIRCUIT_INPUT_H
#define CIRCUIT_INPUT_H

#include <string>

#include "logic_gate.h"

class CircuitInput : public TransitionSource, public std::enable_shared_from_this<CircuitInput> {
   private:
	std::string file_name;
	std::string node_name;
	std::vector<NORGateInput> subscribers;
	InitialValue initial_value = UNDEFINED;
	std::vector<std::shared_ptr<Transition>> transitions;
	TransitionParameters ParseInputFileLine(const std::string& line);

   public:
	CircuitInput(const std::string& file_name,
	             const std::string& node_name) : file_name{file_name},
	                                             node_name{node_name},
	                                             transitions{} {};
	std::vector<NORGateInput>& GetSubscribers() override;
	InitialValue GetInitialOutputValue() override;
	std::string GetOutputName() override;
	void AddSubscriber(NORGateInput subscriber);
	void DetermineInitialValue();
	void ReadTransitionsFromInputFile();
	std::vector<std::shared_ptr<Transition>>& GetTransitions();
	~CircuitInput();
};
#endif