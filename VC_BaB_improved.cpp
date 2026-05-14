#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <climits>

using namespace std;

class Vertex
{
public:
    int id;
    int degree;

    Vertex(int id) : id(id), degree(0) {}

    vector<int> get_neighbors(const vector<vector<int>> &adj_matrix) const
    {
        vector<int> neighbors;

        for (int j = 0; j < adj_matrix.size(); j++)
        {
            if (adj_matrix[id][j] == 1)
            {
                neighbors.push_back(j);
            }
        }

        return neighbors;
    }
};

class Edge
{
public:
    int u, v;

    Edge(int u, int v) : u(u), v(v) {}
};

class Graph
{
public:
    vector<Vertex> vertices;
    vector<Edge> edges;
    vector<vector<int>> adj_matrix;

    Graph(int n)
    {
        adj_matrix = vector<vector<int>>(n, vector<int>(n, 0));

        for (int i = 0; i < n; i++)
        {
            vertices.push_back(Vertex(i));
        }
    }

    void add_edge(int u, int v)
    {
        if (adj_matrix[u][v] == 1)
            return;

        adj_matrix[u][v] = 1;
        adj_matrix[v][u] = 1;

        vertices[u].degree++;
        vertices[v].degree++;

        edges.push_back(Edge(u, v));
    }

    bool has_edges() const
    {
        return !edges.empty();
    }

    Edge get_any_edge() const
    {
        return edges[0];
    }

    void remove_vertex(int v)
    {
        int n = adj_matrix.size();

        for (int u = 0; u < n; u++)
        {
            if (adj_matrix[v][u] == 1)
            {
                adj_matrix[v][u] = 0;
                adj_matrix[u][v] = 0;

                vertices[v].degree--;
                vertices[u].degree--;
            }
        }

        vector<Edge> new_edges;

        for (const Edge &e : edges)
        {
            if (e.u != v && e.v != v)
            {
                new_edges.push_back(e);
            }
        }

        edges = new_edges;
    }

    int choose_vertex() const
    {
        int best = -1;

        for (int i = 0; i < vertices.size(); i++)
        {
            if (vertices[i].degree > 0)
            {
                if (best == -1 || vertices[i].degree > vertices[best].degree)
                {
                    best = i;
                }
            }
        }

        return best;
    }

    bool noLoop4(const Vertex &v1, const Vertex &v2, const Vertex &v3, const Vertex &v4)
    {
        vector<int> neighbors1 = v1.get_neighbors(adj_matrix);
        vector<int> neighbors2 = v2.get_neighbors(adj_matrix);
        vector<int> neighbors3 = v3.get_neighbors(adj_matrix);
        vector<int> neighbors4 = v4.get_neighbors(adj_matrix);

        for (int n1 : neighbors1)
            for (int n2 : neighbors2)
                for (int n3 : neighbors3)
                    for (int n4 : neighbors4)
                        if (n1 == n2 && n2 == n3 && n3 == n4)
                            return false;

        return true;
    }

    vector<int> getLoopers3(const Vertex &v1, const Vertex &v2, const Vertex &v3, const Vertex &v4)
    {
        vector<int> neighbors1 = v1.get_neighbors(adj_matrix);
        vector<int> neighbors2 = v2.get_neighbors(adj_matrix);
        vector<int> neighbors3 = v3.get_neighbors(adj_matrix);
        vector<int> neighbors4 = v4.get_neighbors(adj_matrix);

        unordered_map<int, int> count;

        for (int x : neighbors1)
            count[x]++;
        for (int x : neighbors2)
            count[x]++;
        for (int x : neighbors3)
            count[x]++;
        for (int x : neighbors4)
            count[x]++;

        vector<int> loopers;

        for (const auto &p : count)
        {
            if (p.second >= 3)
                loopers.push_back(p.first);
        }

        return loopers;
    }

    vector<int> getCheries(int v)
    {
        vector<int> neighbors = vertices[v].get_neighbors(adj_matrix);
        vector<int> cheries;

        for (int x : neighbors)
        {
            if (vertices[x].degree == 1)
                cheries.push_back(x);
        }

        return cheries;
    }
};

vector<int> bestCover;

vector<int> intersection(const vector<int> &a, const vector<int> &b)
{
    vector<int> result;

    for (int x : a)
    {
        for (int y : b)
        {
            if (x == y)
            {
                result.push_back(x);
                break;
            }
        }
    }

    return result;
}

int lowerBoundByGreedyMatching(Graph g)
{
    int lb = 0;

    while (g.has_edges())
    {
        Edge e = g.get_any_edge();

        g.remove_vertex(e.u);
        g.remove_vertex(e.v);

        lb++;
    }

    return lb;
}

vector<int> greedyVertexCover(Graph g)
{
    vector<int> cover;

    while (g.has_edges())
    {
        int v = g.choose_vertex();

        if (v == -1)
            break;

        cover.push_back(v);
        g.remove_vertex(v);
    }

    return cover;
}

bool vertexCover(Graph g, int k, vector<int> &cover);

bool rule1(Graph &g, int v, const vector<int> &neighbors, int k, vector<int> &cover)
{
    int w = neighbors[0];

    Graph g1 = g;
    vector<int> cover1 = cover;

    cover1.push_back(w);
    g1.remove_vertex(w);

    if (vertexCover(g1, k - 1, cover1))
    {
        cover = cover1;
        return true;
    }

    return false;
}

bool rule2(Graph &g, int v, const vector<int> &neighbors, int k, vector<int> &cover)
{
    int w1 = neighbors[0];
    int w2 = neighbors[1];
    vector<int> commonNeighborhood =
        intersection(g.vertices[w1].get_neighbors(g.adj_matrix),
                     g.vertices[w2].get_neighbors(g.adj_matrix));

    if (g.adj_matrix[w1][w2] == 1)
    {
        if (k >= 2)
        {
            Graph g1 = g;
            vector<int> cover1 = cover;

            cover1.push_back(w1);
            cover1.push_back(w2);

            g1.remove_vertex(w1);
            g1.remove_vertex(w2);

            if (vertexCover(g1, k - 2, cover1))
            {
                cover = cover1;
                return true;
            }
        }
        return false;
    }

    if (g.adj_matrix[w1][w2] == 0 && commonNeighborhood.size() >= 2)
    {
        if (k >= 2)
        {
            Graph g2 = g;
            vector<int> cover2 = cover;

            int sel1 = commonNeighborhood[0];
            int sel2 = commonNeighborhood[1];
            cover2.push_back(sel1);
            cover2.push_back(sel2);

            g2.remove_vertex(sel1);
            g2.remove_vertex(sel2);

            if (vertexCover(g2, k - 2, cover2))
            {
                cover = cover2;
                return true;
            }
        }
        return false;
    }
    if (g.adj_matrix[w1][w2] == 0 && commonNeighborhood.size() == 1)
    {
        vector<int> unionNeighbors = g.vertices[w1].get_neighbors(g.adj_matrix);

        for (int x : g.vertices[w2].get_neighbors(g.adj_matrix))
        {
            if (find(
                unionNeighbors.begin(), 
                unionNeighbors.end(), x) == 
                unionNeighbors.end())
                unionNeighbors.push_back(x);
        }

        if (unionNeighbors.size() >= 3)
        {
            if (k >= 2)
            {
                Graph g4 = g;
                vector<int> cover4 = cover;

                cover4.push_back(w1);
                cover4.push_back(w2);

                g4.remove_vertex(w1);
                g4.remove_vertex(w2);

                if (vertexCover(g4, k - 2, cover4))
                {
                    cover = cover4;
                    return true;
                }
            }

            if (k >= unionNeighbors.size())
            {
                Graph g3 = g;
                vector<int> cover3 = cover;

                for (int x : unionNeighbors)
                {
                    cover3.push_back(x);
                    g3.remove_vertex(x);
                }
                if (vertexCover(g3, k - unionNeighbors.size(), cover3))
                {
                    cover = cover3;
                    return true;
                }
            }
            return false;
        }
    }
    return false;
}

bool rule3(Graph &g, int v, const vector<int> &neighbors, int k, vector<int> &cover)
{
    int w1 = neighbors[0];
    int w2 = neighbors[1];
    int w3 = neighbors[2];

    vector<int> commonNeighborhood12 =
        intersection(g.vertices[w1].get_neighbors(g.adj_matrix),
                     g.vertices[w2].get_neighbors(g.adj_matrix));

    vector<int> commonNeighborhood13 =
        intersection(g.vertices[w1].get_neighbors(g.adj_matrix),
                     g.vertices[w3].get_neighbors(g.adj_matrix));

    vector<int> commonNeighborhood23 =
        intersection(g.vertices[w2].get_neighbors(g.adj_matrix),
                     g.vertices[w3].get_neighbors(g.adj_matrix));

    vector<int> commonNeighborhood = intersection(commonNeighborhood12, commonNeighborhood13);
    commonNeighborhood = intersection(commonNeighborhood, commonNeighborhood23);

    if (!commonNeighborhood.empty() &&
        commonNeighborhood[0] == v &&
        g.noLoop4(g.vertices[v], g.vertices[w1], g.vertices[w2], g.vertices[w3]))
    {
        vector<int> cheries_w1 = g.getCheries(w1);
        vector<int> cheries_w2 = g.getCheries(w2);
        vector<int> cheries_w3 = g.getCheries(w3);

        if (cheries_w1.size() >= 2 && cheries_w2.size() >= 2 && k >= 6)
        {
            Graph g3 = g;
            vector<int> cover3 = cover;

            int c = commonNeighborhood[0];

            vector<int> chosen = {
                c, w3,
                cheries_w1[0], cheries_w1[1],
                cheries_w2[0], cheries_w2[1]};

            for (int x : chosen)
            {
                cover3.push_back(x);
                g3.remove_vertex(x);
            }

            if (vertexCover(g3, k - 6, cover3))
            {
                cover = cover3;
                return true;
            }
        }

        if (cheries_w3.size() >= 2 && k >= 3)
        {
            Graph g1 = g;
            vector<int> cover1 = cover;

            int c = commonNeighborhood[0];

            vector<int> chosen = {c, cheries_w3[0], cheries_w3[1]};

            for (int x : chosen)
            {
                cover1.push_back(x);
                g1.remove_vertex(x);
            }

            if (vertexCover(g1, k - 3, cover1))
            {
                cover = cover1;
                return true;
            }
        }

        if (k >= 3)
        {
            Graph g2 = g;
            vector<int> cover2 = cover;

            vector<int> chosen = {w1, w2, w3};

            for (int x : chosen)
            {
                cover2.push_back(x);
                g2.remove_vertex(x);
            }

            if (vertexCover(g2, k - 3, cover2))
            {
                cover = cover2;
                return true;
            }
        }

        return false;
    }

    vector<int> chosenCommon;

    if (commonNeighborhood12.size() == 2)
        chosenCommon = commonNeighborhood12;
    else if (commonNeighborhood13.size() == 2)
        chosenCommon = commonNeighborhood13;
    else if (commonNeighborhood23.size() == 2)
        chosenCommon = commonNeighborhood23;

    if (chosenCommon.size() == 2)
    {
        int loopVertex;

        if (chosenCommon[0] == v)
            loopVertex = chosenCommon[1];
        else
            loopVertex = chosenCommon[0];

        if (k >= 2)
        {
            Graph g2 = g;
            vector<int> cover2 = cover;

            vector<int> chosen = {v, loopVertex};

            for (int x : chosen)
            {
                cover2.push_back(x);
                g2.remove_vertex(x);
            }

            if (vertexCover(g2, k - 2, cover2))
            {
                cover = cover2;
                return true;
            }
        }

        if (k >= 3)
        {
            Graph g3 = g;
            vector<int> cover3 = cover;

            vector<int> chosen = {w1, w2, w3};

            for (int x : chosen)
            {
                cover3.push_back(x);
                g3.remove_vertex(x);
            }

            if (vertexCover(g3, k - 3, cover3))
            {
                cover = cover3;
                return true;
            }
        }

        return false;
    }

    if (commonNeighborhood.size() == 1 &&
        !g.noLoop4(g.vertices[v], g.vertices[w1], g.vertices[w2], g.vertices[w3]))
    {
        vector<int> loopers =
            g.getLoopers3(g.vertices[v], g.vertices[w1], g.vertices[w2], g.vertices[w3]);

        int nonLooper = -1;

        if (find(loopers.begin(), loopers.end(), w1) == loopers.end())
            nonLooper = w1;
        else if (find(loopers.begin(), loopers.end(), w2) == loopers.end())
            nonLooper = w2;
        else if (find(loopers.begin(), loopers.end(), w3) == loopers.end())
            nonLooper = w3;

        if (nonLooper != -1)
        {
            vector<int> cherries = g.getCheries(nonLooper);

            if (cherries.size() >= 2 && k >= 3)
            {
                Graph g1 = g;
                vector<int> cover1 = cover;

                vector<int> chosen = {v, cherries[0], cherries[1]};

                for (int x : chosen)
                {
                    cover1.push_back(x);
                    g1.remove_vertex(x);
                }

                if (vertexCover(g1, k - 3, cover1))
                {
                    cover = cover1;
                    return true;
                }
            }
        }

        if (k >= 3)
        {
            Graph g2 = g;
            vector<int> cover2 = cover;

            vector<int> chosen = {w1, w2, w3};

            for (int x : chosen)
            {
                cover2.push_back(x);
                g2.remove_vertex(x);
            }

            if (vertexCover(g2, k - 3, cover2))
            {
                cover = cover2;
                return true;
            }
        }

        return false;
    }

    if (k >= 1)
    {
        Graph g1 = g;
        vector<int> cover1 = cover;

        cover1.push_back(v);
        g1.remove_vertex(v);

        if (vertexCover(g1, k - 1, cover1))
        {
            cover = cover1;
            return true;
        }
    }

    if (k >= 3)
    {
        Graph g2 = g;
        vector<int> cover2 = cover;

        vector<int> chosen = {w1, w2, w3};

        for (int x : chosen)
        {
            cover2.push_back(x);
            g2.remove_vertex(x);
        }

        if (vertexCover(g2, k - 3, cover2))
        {
            cover = cover2;
            return true;
        }
    }

    return false;
}

bool rule4(Graph &g, int v, const vector<int> &neighbors, int k, vector<int> &cover)
{
    int w1 = neighbors[0];
    int w2 = neighbors[1];
    int w3 = neighbors[2];
    int w4 = neighbors[3];

    if (k >= 1)
    {
        Graph g1 = g;
        vector<int> cover1 = cover;

        cover1.push_back(v);
        g1.remove_vertex(v);

        if (vertexCover(g1, k - 1, cover1))
        {
            cover = cover1;
            return true;
        }
    }

    if (k >= 4)
    {
        Graph g2 = g;
        vector<int> cover2 = cover;

        vector<int> chosen = {w1, w2, w3, w4};

        for (int x : chosen)
        {
            cover2.push_back(x);
            g2.remove_vertex(x);
        }

        if (vertexCover(g2, k - 4, cover2))
        {
            cover = cover2;
            return true;
        }
    }

    return false;
}

bool rule5orMore(Graph &g, int v, const vector<int> &neighbors, int k, vector<int> &cover)
{
    if (k >= 1)
    {
        Graph g1 = g;
        vector<int> cover1 = cover;

        cover1.push_back(v);
        g1.remove_vertex(v);

        if (vertexCover(g1, k - 1, cover1))
        {
            cover = cover1;
            return true;
        }
    }

    if (k >= neighbors.size())
    {
        Graph g2 = g;
        vector<int> cover2 = cover;

        for (int w : neighbors)
        {
            cover2.push_back(w);
            g2.remove_vertex(w);
        }

        if (vertexCover(g2, k - neighbors.size(), cover2))
        {
            cover = cover2;
            return true;
        }
    }

    return false;
}

int findVertexOfDegree(const Graph &g, int degree)
{
    for (int i = 0; i < g.vertices.size(); i++)
    {
        if (g.vertices[i].degree == degree)
            return i;
    }

    return -1;
}

bool vertexCover(Graph g, int k, vector<int> &cover)
{
    if (k < 0)
        return false;

    if (!bestCover.empty() && cover.size() + k >= bestCover.size())
        return false;

    if (!g.has_edges())
    {
        if (bestCover.empty() || cover.size() < bestCover.size())
        {
            bestCover = cover;
            cout << "New best cover size: " << bestCover.size() << endl;
        }

        return true;
    }

    int lb = lowerBoundByGreedyMatching(g);

    if (lb > k)
        return false;

    int v1 = findVertexOfDegree(g, 1);
    if (v1 != -1)
        return rule1(g, v1, g.vertices[v1].get_neighbors(g.adj_matrix), k, cover);

    int v2 = findVertexOfDegree(g, 2);
    if (v2 != -1)
        return rule2(g, v2, g.vertices[v2].get_neighbors(g.adj_matrix), k, cover);

    int v = g.choose_vertex();
    vector<int> neighbors = g.vertices[v].get_neighbors(g.adj_matrix);

    if (g.vertices[v].degree == 3)
    {
        return rule3(g, v, neighbors, k, cover);
    }
    if (g.vertices[v].degree == 4)
    {
        return rule4(g, v, neighbors, k, cover);
    }
    if (g.vertices[v].degree >= 5)
    {
        return rule5orMore(g, v, neighbors, k, cover);
    }
    return false;
}

Graph getGraphFromFile(const string &filename)
{
    ifstream f(filename);

    if (!f.is_open())
    {
        cerr << "Failed to open " << filename << endl;
        exit(1);
    }

    vector<pair<int, int>> tuples;
    string line;

    while (getline(f, line))
    {
        if (line.empty())
            continue;

        stringstream ss(line);
        int a, b;

        if (!(ss >> a >> b))
            continue;

        tuples.push_back({a, b});
    }

    f.close();

    if (tuples.empty())
    {
        cerr << "Input file is empty.\n";
        exit(1);
    }

    int n = tuples[0].first;

    Graph g(n);

    for (int i = 1; i < tuples.size(); i++)
    {
        int u = tuples[i].first - 1;
        int v = tuples[i].second - 1;

        g.add_edge(u, v);
    }

    return g;
}

void writeSolutionToFile(const string &filename, const vector<int> &cover)
{
    ofstream out(filename);

    if (!out.is_open())
    {
        cerr << "Failed to open " << filename << endl;
        return;
    }

    out << cover.size() << "\n";

    for (int v : cover)
    {
        out << v + 1 << "\n";
    }

    out.close();
}

int main()
{
    string userInput = ""; //path to graph

    Graph g = getGraphFromFile(userInput + ".in");

    cout << "For " << userInput << endl;

    bestCover = greedyVertexCover(g);

    cout << "Initial greedy upper bound: " << bestCover.size() << endl;

    int k = bestCover.size() - 1;

    while (k >= 0)
    {
        cout << "Trying k = " << k << endl;

        vector<int> cover;
        Graph gCopy = g;

        bool found = vertexCover(gCopy, k, cover);

        if (!found)
        {
            cout << "No cover of size " << k << " found." << endl;
            break;
        }

        k--;
    }

    cout << "Minimum vertex cover size found: " << bestCover.size() << endl;

    string solutionFileName = userInput + "_BBNF_solution.in";
    writeSolutionToFile(solutionFileName, bestCover);

    cout << "Solution written to " << solutionFileName << endl;

    return 0;
}