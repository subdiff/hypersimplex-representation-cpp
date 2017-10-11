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

class AutGroup;

struct Edge {
    Edge(int _v, int _w);
    inline bool operator==(Edge const& b) { return v == b.v && w == b.w; }
    inline bool operator!=(Edge const& b){ return !(*this == b); }
    inline bool has(int vertex) { return v == vertex || w == vertex; }
    int v, w;
};

struct EdgeEquivClass {
    EdgeEquivClass(std::vector<Edge> edges);
    std::vector<Edge> m_edges;
    void sortEdges();
    int calcMultiplicity();
    int multiplicity = 0;
};

struct VtxTrnsSubgroup {
    VtxTrnsSubgroup(std::string sub, AutGroup *parent);
    std::string m_gapName;
    AutGroup *m_parent;
    std::vector<std::string> m_facEl;
    std::vector<EdgeEquivClass *> m_edgeEquivClasses;
};

class Hypersimplex {
public:
    virtual ~Hypersimplex();
protected:
    Hypersimplex(int d, int k);

    void initEdges();
    void initGraph();
    void initGroup();
    void initCalculations();

    bool haveEdge(int v, int w);

    void startPermutate(std::string factoredPerm, int *result);
    void permutateVertices(std::string factoredPerm, int *vertices);
    int permutateVertex(std::string factoredPerm, int vertex);
    std::string prepareForParsing(const std::string &perm);
    virtual std::vector<int *> parsePermutation(std::string perm) = 0;

    void calcVtxTrnsSubgroups();
    bool isVtxTrnsSubgroup(std::string sub);

    void calcEdgeEquivClasses();

    int m_d;
    int m_k;
    AutGroup *m_group;
    std::vector<VtxTrnsSubgroup *> m_vtxTrnsSubgroups;
    std::vector<Edge> m_edges;
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

