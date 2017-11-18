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

#ifndef VERTEX_H
#define VERTEX_H

#include <vector>

class Vertex {
public:
    Vertex(int combIndex, std::vector<int> comps, int k);
    Vertex(int combIndex, int dim, int k);
    Vertex(std::vector<int> comps, int k);
    Vertex(const Vertex &b);

    inline bool operator==(const Vertex &b) { return m_combIndex == b.combIndex(); }
    inline bool operator!=(const Vertex &b){ return !(*this == b); }
    int operator*(const Vertex &b) const;
    Vertex operator+(const Vertex &b) const;
    Vertex operator-(const Vertex &b) const;
    Vertex operator-() const;
    int operator[](const int index) const {return m_comps[index];}

    bool isVertex() const {
        return m_combIndex != -1;
    }

    int combIndex() const {
        return m_combIndex;
    }
    std::vector<int> comps() const {
        return m_comps;
    }
    int k() const {
        return m_k;
    }
    double len() const;

private:
    Vertex createVertex(std::vector<int> comps, int compSum, bool isVertex) const;

    int m_dim;
    int m_k;
    int m_combIndex = -1;
    std::vector<int> m_comps;
};

#endif // VERTEX_H
