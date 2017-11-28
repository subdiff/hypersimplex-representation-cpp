/*********************************************************************
Hypersimplex Representer
Copyright (C) 2017 Roman Gilg

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
*********************************************************************/

#include <vector>
#include <string>

class AutGroup {
public:
    AutGroup(int d, int k);
    ~AutGroup();

    std::vector<std::string> getSubgroups() const {
        return m_subgroups;
    }

    std::vector<std::string> getFactorizations(int subIndex = -1) const;

private:
    void calcSubgroups();
    void createFactoredElements();

    void calcVtxTrnsSubgroups();

    void gapCreateGroup(int d, bool product);
    std::string gap_eval(const std::string _cmd, bool readOutput = true, std::string end = std::string()) const;

    std::vector<std::string> m_factorizations;
    std::vector<std::string> m_subgroups;

    std::vector<std::string> *m_subFactorizations = nullptr;

    std::string m_gapName;

    int m_writePipe;
    int m_readPipe;
};
