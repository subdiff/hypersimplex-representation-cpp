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

#ifndef HYPERSIMPLEX_H
#define HYPERSIMPLEX_H

#include <string>
#include <vector>

#include "vertex.h"

class AutGroup;
class GiMatrix;

struct Edge {
    Edge(int _v, int _w);
    inline bool operator==(Edge const& b) { return v == b.v && w == b.w; }
    inline bool operator!=(Edge const& b){ return !(*this == b); }
    inline bool operator<(Edge const& b) const { return v <= b.v && w < b.w; }
    inline bool has(int vertex) { return v == vertex || w == vertex; }
    int v, w;
};

struct EdgeEquivClass {
    EdgeEquivClass(std::vector<Edge> edges);
    bool has(Edge edge);
    std::vector<Edge> m_edges;
    int calcMultiplicity();
    int multiplicity = 0;
};

struct VtxTrnsSubgroup {
    VtxTrnsSubgroup(std::string sub, int index, AutGroup *parent);
    std::string m_gapName;
    int m_index;
    AutGroup *m_parent;
    std::vector<EdgeEquivClass *> m_edgeEquivClasses;
};

class Hypersimplex {
public:
    virtual ~Hypersimplex();

    inline int d() { return m_d; }
    inline int k() { return m_k; }
    inline int vertexCount() { return m_vertexCount; }
    inline int degree() { return m_degree; }

    std::vector<std::string> getVtxTrSubgroupNames();
    GiMatrix getGiMatrix(int subgroup);

    bool isEdge(int vertex1, int vertex2);

    std::pair<std::vector<Vertex>, std::vector<Vertex> > getFacetPair(int index = 0) const;

protected:
    Hypersimplex(int d, int k);

    void initEdges();
    void initGroup();
    void initCalculations();

    bool haveEdge(int v, int w);

    void startPermutate(std::string factoredPerm, int *result);
    void permutateVertices(std::string factoredPerm, int *vertices);
    int permutateVertex(std::string factoredPerm, int vertex);
    std::string prepareForParsing(const std::string &perm);
    virtual std::vector<int *> parsePermutation(std::string perm) = 0;

    int m_d;
    int m_k;
    int m_degree;
    AutGroup *m_group;
    std::vector<VtxTrnsSubgroup *> m_vtxTrnsSubgroups;
    std::vector<Edge> m_edges;
    int m_vertexCount;

private:
    void calcEdgeEquivClasses();
    void initVertices();

    void calcVtxTrnsSubgroups();
    bool isVtxTrnsSubgroup(int sub);

    std::vector<Edge> getEdgesToVertex(int vertex);

    std::vector<Vertex> m_vertices;
};

class AsymHypers : public Hypersimplex {
public:
    AsymHypers(int d, int k);
    ~AsymHypers();

private:
    virtual std::vector<int *> parsePermutation(std::string perm) override;

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

    /*
     * Associates which combinadic vertex is
     * mapped to which other cominadic vertex.
     */
    int *m_genSymS2_12 = nullptr;
    int *m_genSymSd_1d = nullptr;
    int *m_genSymSd_1d_inv = nullptr;
    int *m_genSymSd_12 = nullptr;
};

static bool s_ready = true;
static Hypersimplex *s_hypers = nullptr;

static void *s_createHypersimplex(int d, int k)
{
    if (k <= 0 || d <= 1 || d <= k) {
        return nullptr;
    }

    delete s_hypers;
    s_hypers = nullptr;

    s_ready = false;
    if (d == 2 * k) {
        s_hypers = new SymHypers(d, k);
    } else {
        s_hypers = new AsymHypers(d, k);
    }
    s_ready = true;
}

#endif // HYPERSIMPLEX_H
