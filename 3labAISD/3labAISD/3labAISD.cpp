#pragma once
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <stdexcept>
#include <queue>
#include <functional>
#include <iterator>


template<typename Vertex, typename Distance = double>
class Graph {
public:
    struct Edge {
        Vertex from;
        Vertex to;
        Distance distance;
    };

    bool has_vertex(const Vertex& v) const {
        return _vertices.count(v) > 0;
    }

    void add_vertex(const Vertex& v) {
        if (has_vertex(v))
            throw std::invalid_argument("Вершина уже существует в графе");
        _vertices.insert(v);
        _edges.emplace(v, std::vector<Edge>{});
    }

    bool remove_vertex(const Vertex& v) {
        if (!has_vertex(v)) return false;
        _vertices.erase(v);
        _edges.erase(v);
        for (auto& kv : _edges) {
            auto& list_edge = kv.second; // Получаем ссылку на вектор рёбер, который связан с текущей вершиной
            list_edge.erase(std::remove_if(list_edge.begin(), list_edge.end(),
                [v](const Edge& e) { return e.to == v; }), list_edge.end());
        }
        return true;
    }

    void add_edge(const Vertex& from, const Vertex& to, const Distance& d) {
        if (!has_vertex(from) || !has_vertex(to))
            throw std::out_of_range("Вершина не существует в графе");
        _edges[from].push_back({ from, to, d });
    }

    // между двумя вершинами
    bool remove_edge(const Vertex& from, const Vertex& to) {
        if (!has_vertex(from) || !has_vertex(to))
            return false;
        auto& list = _edges[from];
        auto it = std::remove_if(list.begin(), list.end(),
            [to](const Edge& e) { return e.to == to; });
        if (it != list.end()) {
            list.erase(it, list.end());
            return true;
        }
        return false;
    }

    // определенное ребро
    bool remove_edge(const Edge& e) {
        if (!has_vertex(e.from) || !has_vertex(e.to))
            return false;
        auto& list = _edges[e.from];
        auto it = std::remove_if(list.begin(), list.end(),
            [&e](const Edge& edg) { return e.from == edg.from && e.to == edg.to && e.distance == edg.distance; });
        if (it != list.end()) {
            list.erase(it, list.end());
            return true;
        }
        return false;
    }

    // проверяет, существует ли ребро между двумя вершинами
    bool has_edge(const Vertex& from, const Vertex& to) const {
        if (!has_vertex(from) || !has_vertex(to))
            return false;
        const auto& edges = _edges.at(from);
        return std::find_if(edges.begin(), edges.end(),
            [to](const Edge& e) { return e.to == to; }) != edges.end();
    }

    // проверяет, существует ли конкретное ребро в графе
    bool has_edge(const Edge& e) {
        if (!has_vertex(e.from) || !has_vertex(e.to))
            return false;
        const auto& edges = _edges.at(e.from);
        return std::find(edges.begin(), edges.end(), e) != edges.end();
    }

    // все вершины
    size_t order() const {
        return _vertices.size();
    }

    // степень или число ребер вершины
    size_t degree(const Vertex& v) const {
        if (!has_vertex(v))
            throw std::invalid_argument("Вершина не существует в графе");

        size_t degree = _edges.at(v).size(); // начальная степень равна количеству инцидентных ребер

        for (const auto& kv : _edges) { // цикл по парам ключ значение
            if (kv.first != v) { // не равен ли текущий ключ равным вершине
                for (const Edge& e : kv.second) { // цикл по каждому ребру в списке ребер текущей вершины
                    // если конечная вершина рассматриваемого ребра равна вершине v,
                    // то ребро инцидентно этой вершине, и степень увеличивается на 1
                    if (e.to == v)
                        degree++;
                }
            }
        }
        return degree;
    }

    // беллман - форд
    std::vector<Edge> shortest_path(const Vertex& from, const Vertex& to) const {
        if (!has_vertex(from) || !has_vertex(to))
            throw std::invalid_argument("Вершина не существует в графе");

        std::unordered_map<Vertex, Distance> distance; // словарь расстояний
        std::unordered_map<Vertex, Edge> predecessor; // словарь предшественников

        // приравниваем расстояния до всех вершин бесконечности, а до предшественников пустым ребром
        for (const Vertex& v : _vertices) {
            distance[v] = std::numeric_limits<Distance>::infinity();
            predecessor[v] = Edge();
        }

        distance[from] = Distance(); // дистанция от начальной до начальной равно нулю

        // Многократное ослабление рёбер
        for (size_t i = 1; i < _vertices.size(); ++i) {
            for (const auto& kv : _edges) { // цикл по вершинам
                const Vertex& u = kv.first; // получаем текущую вершину u и ее исхходящие ребра
                for (const Edge& e : kv.second) { // проходим по всем исходящим ребрам из вершины u -
                    Vertex v = e.to; // вершина в которую идет текущее ребро
                    Distance weight = e.distance;
                    if (distance[u] + weight < distance[v]) { // если можем улучшить текущее расстояние до вершины v через вершину u
                        distance[v] = distance[u] + weight; // то обновляем расстояние до вершины v
                        predecessor[v] = e; // запоминаем предшествующее ребро для вершины v
                    }
                }
            }
        }

        // Проверка на наличие отрицательных циклов
        for (const auto& kv : _edges) {
            const Vertex& u = kv.first; // проходим по всем вершинам в графе
            for (const Edge& e : kv.second) { // цикл по всем исходящим ребрам из вершины u
                Vertex v = e.to;
                Distance weight = e.distance;
                if (distance[u] + weight < distance[v]) { // еслим можем улучшить расстояние до v через u значит есть отриц. цикл
                    throw std::runtime_error("Граф содержит отрицательный цикл");
                }
            }
        }

        std::vector<Edge> path;
        // начиная с конечной вершины, добавляем рёбра в путь, используя информацию о предшественниках
        for (Vertex v = to; predecessor[v].from != Vertex(); v = predecessor[v].from) {
            path.push_back(predecessor[v]); // добавляем предшествующее ребро в путь
        }
        std::reverse(path.begin(), path.end()); // реверс чтобы вершины были упорядочены от начальной к конечной
        return path;
    }

    // обход графа в ширину начиная с указанной вершины с выполнением действия над каждой вершиной
    void walk(const Vertex& start_vertex, std::function<void(const Vertex&)> action) {
        if (!has_vertex(start_vertex))
            return;

        std::queue<Vertex> queue; // очередь для обхода в ширину
        std::unordered_set<Vertex> visited; // множество для посещенных вершин

        queue.push(start_vertex);
        visited.insert(start_vertex); // добавляем начальную вершину в очередь и отмечаем ее посещенной

        while (!queue.empty()) {
            Vertex current = queue.front(); // извлекаем текущую вершину из очереди
            queue.pop();
            action(current);

            for (const Edge& edge : _edges.at(current)) { // цикл по всем ребрам из текущей вершины
                if (visited.find(edge.to) == visited.end()) { // если вершине еще не посещена то добавляем в очередь
                    queue.push(edge.to);
                    visited.insert(edge.to);
                }
            }
        }
    }

    // находит самый удаленный травмпункт на основе среднего расстояния до всех других травмпунктов
    Vertex find_furthest_hospital() {
        std::unordered_map<Vertex, double> avg_distances; // хэш таблица для хранения средних расстояний до всех вершин

        std::unordered_set<Vertex> vertices_ = vertices(); // получаем все вершины графа

        for (const auto& v : vertices_) { // для каждой вершины из множества вершин
            double total_distance = 0.0;
            int num_neighbors = 0;

            for (const auto& u : vertices_) {
                if (u != v) { // убираем текующую вершину из рассмотрения
                    std::vector<Edge> path = shortest_path(v, u); // кратчайший путь от текущей вершины до соседней
                    double path_distance = 0.0; // длина найденного пути
                    for (const auto& edge : path) { // ищем длину найденного пути
                        path_distance += edge.distance;
                    }
                    total_distance += path_distance; // обновляем суммарное расстояние и количество соседних вершин
                    num_neighbors++;
                }
            }
            if (num_neighbors > 0) { // если есть сосед, то ищем среднее расстояние
                avg_distances[v] = total_distance / num_neighbors;
            }
        }

        Vertex furthest_hospital;
        double max_avg_distance = -1.0; // для хранения вершины с самым большим средним расстоянием и этим расстоянеим

        for (const auto& pair : avg_distances) { // выбираем вершину с макс авераг расстоянием
            const Vertex& v = pair.first;
            double distance = pair.second;

            if (distance > max_avg_distance) {
                max_avg_distance = distance;
                furthest_hospital = v;
            }
        }

        return furthest_hospital;
    }

private:
    // хранение вершин
    std::unordered_set<Vertex> _vertices;
    // хранение рёбер
    std::unordered_map<Vertex, std::vector<Edge>> _edges;

    // получение всех вершин
    std::unordered_set<Vertex> vertices() const {
        return _vertices;
    }
    // получение всех рёбер конкретной вершины
    std::vector<Edge> edges(const Vertex& vertex) {
        return _edges[vertex];
    }
};

int main() {
    setlocale(LC_ALL, "Rus");
    Graph<int> graph;

    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);
    graph.add_vertex(4);
    graph.add_vertex(5);

    graph.add_edge(1, 2, 5.0);
    graph.add_edge(2, 3, 2.0);
    graph.add_edge(3, 4, 1.0);
    graph.add_edge(4, 5, 3.0);
    graph.add_edge(5, 1, 4.0);
    graph.add_edge(2, 5, 2.5);

    int furthest_hospital = graph.find_furthest_hospital();
    std::cout << "Самый удаленный травмпункт находится в вершине: " << furthest_hospital << std::endl;

    return 0;
}