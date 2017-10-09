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

// A group

#include <vector>
#include <string>

#include <QString>

class AutGroup {
public:
    AutGroup(int d, int k);
    ~AutGroup();

    std::vector<std::string> getSubgroups() const {
        return m_subgroups;
    }

    std::vector<std::string> getFactorizations() const {
        return m_factorizations;
    }

private:
    void gapCreateGroup(int d, bool semi);
//    std::string gapGetAllElements(std::string set);

    void calcSubgroups();
    QString createFactorization(QString g);
    void createFactoredElements();

//    QString calcEdgeEquivClasses();

    std::vector<QString> m_elements;
    std::vector<std::string> m_factorizations;
    std::vector<std::string> m_subgroups;
};
