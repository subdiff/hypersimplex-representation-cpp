// A graph

class Vertex;

class Graph {
public:
    Graph(int dim);

private:
    // TODOX: do this as std container
    Vertex *m_vals;
    int m_dim;
};
