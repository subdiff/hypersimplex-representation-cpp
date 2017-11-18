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

#include "vertex.h"

#include "combinadic.h"

#include <cmath>

#include <QDebug>

Vertex::Vertex(int combIndex, std::vector<int> comps, int k)
    : m_combIndex(combIndex),
      m_comps(comps),
      m_dim(comps.size()),
      m_k(k)
{
}

Vertex::Vertex(int combIndex, int dim, int k)
    : m_combIndex(combIndex),
      m_dim(dim),
      m_k(k)
{
    int comps[dim] = {0};
    combinadicComb(k, combIndex, comps);

    m_comps = std::vector<int>(comps, comps + dim);
}

Vertex::Vertex(std::vector<int> comps, int k)
    : m_comps(comps),
      m_dim(comps.size()),
      m_k(k)
{
}

Vertex::Vertex(const Vertex &b)
    : m_combIndex(b.combIndex()),
      m_comps(b.comps()),
      m_dim(b.comps().size()),
      m_k(b.k())
{
}

int Vertex::operator*(const Vertex &b) const
{
    auto compsB = b.comps();
    int sum = 0;
    for (int i = 0; i < m_comps.size(); i++) {
        sum += m_comps[i] * compsB[i];
    }
    return sum;
}

Vertex Vertex::operator+(const Vertex &b) const
{
    std::vector<int> comps;
    auto compsB = b.comps();
    auto compSum = 0;
    bool isVertex = true;
    for (int i = 0; i < m_comps.size(); i++) {
        auto c = m_comps[i] + compsB[i];
        compSum += c;
        if (c != 0 || c!= 1) {
            isVertex = false;
        }
        comps.push_back(c);
    }
    return createVertex(comps, compSum, isVertex);
}

Vertex Vertex::operator-(const Vertex &b) const
{
    return operator+(-b);
}

Vertex Vertex::operator-() const
{
    std::vector<int> comps;
    int compSum = 0;
    bool isVertex = true;
    for (int i = 0; i < m_comps.size(); i++) {
        auto c = - m_comps[i];
        compSum += c;
        if (c != 0 || c!= 1) {
            isVertex = false;
        }
        comps.push_back(c);
    }
    return createVertex(comps, compSum, isVertex);
}

double Vertex::len() const
{
    auto scalarPr = (*this) * (*this);
    return std::sqrt(scalarPr);
}

Vertex Vertex::createVertex(std::vector<int> comps, int compSum, bool isVertex) const
{
    if (!isVertex || compSum != m_k) {
        return Vertex(-1, comps, m_k);
    } else {
        return Vertex(comps, m_k);
    }
}
