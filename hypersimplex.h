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

class Graph;
class AutGroup;

struct VtxTrnsSubgroup {
    VtxTrnsSubgroup(std::string sub) : m_gapName(sub) {}
    std::string m_gapName;
    std::vector<std::string> m_edgeEquivClasses;
};

class Hypersimplex {
public:
    virtual ~Hypersimplex();
protected:
    Hypersimplex(int d, int k);

    void initGraph();
    void initGroup();

    void startPermutate(std::string factoredPerm, int *result);
    void permutateVertices(std::string factor, int *vertices);
    std::string prepareForParsing(const std::string &perm);
    virtual std::vector<int *> parsePermutation(std::string perm) = 0;

    void calcVtxTrnsSubgroups();
    bool isVtxTrnsSubgroup(std::string sub);

    int m_d;
    int m_k;
    Graph *m_graph;
    AutGroup *m_group;
    std::vector<VtxTrnsSubgroup> m_vtxTrnsSubgroups;
    bool **m_vertices;
    int m_vertexCount;
};

class AsymHypers : public Hypersimplex {
public:
    AsymHypers(int d, int k);
    ~AsymHypers();

private:
    virtual std::vector<int *> parsePermutation(std::string perm) override;
    bool isVtxTrnsSubgroup(std::string sub);

    /*
     * Associates which combinadic vertex is
     * mapped to which other cominadic vertex.
     */
    int *m_genAsymSd_1d = nullptr;
    int *m_genAsymSd_1d_inv = nullptr;
    int *m_genAsymSd_12 = nullptr;
};

class SymHypers : public Hypersimplex {
public:
    SymHypers(int d, int k);
    ~SymHypers();

private:
    virtual std::vector<int *> parsePermutation(std::string perm) override;
    bool isVtxTrnsSubgroup(std::string sub);

    /*
     * Associates which combinadic vertex is
     * mapped to which other cominadic vertex.
     */
    int *m_genSymS2_12 = nullptr;
    int *m_genSymSd_1d = nullptr;
    int *m_genSymSd_1d_inv = nullptr;
    int *m_genSymSd_12 = nullptr;
};

static Hypersimplex *createHypersimplex(int d, int k)
{
    if (d == 2 * k) {
        return new SymHypers(d, k);
    } else {
        return new AsymHypers(d, k);
    }
}

