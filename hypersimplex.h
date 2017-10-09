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

// A hypersimplex

#include <array>
#include <vector>

//#include <QString>

class Graph;
class AutGroup;
//class Vertex;

class Hypersimplex {
public:
    Hypersimplex(int d, int k);
    ~Hypersimplex();

private:
    int m_d;
    int m_k;

    void initSym();
    void initAsym();

    void initGraph();
    void initGroup();

    void calcVtxTrnsSubgroups();
    bool isVtxTrnsSubgroup(std::string sub);

    Graph *m_graph;
    AutGroup *m_group;
    std::vector<std::string> m_vtxTrnsSubgroups;
    bool **m_vertices;

    /*
     * Associates which combinadic vertex is
     * mapped to which other cominadic vertex.
     */
    // case d != 2k
    int *m_genAsymSd_1d = nullptr;
    int *m_genAsymSd_12 = nullptr;
    // case d = 2k
    int *m_genSymS2_12 = nullptr;
    int *m_genSymSd_1d = nullptr;
    int *m_genSymSd_12 = nullptr;
};

//struct Vertex{
//    Vertex(int dim, bool *components);
//    ~Vertex();
//    int m_dim;
//    bool *m_comp;
//};
