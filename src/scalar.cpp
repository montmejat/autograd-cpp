#include "scalar.h"
#include <filesystem>
#include <fstream>
#include <tuple>

const std::string html_begin = R"(
<!DOCTYPE html>
<html>

<head>
    <title>Binary Tree Visualization</title>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/d3/7.8.5/d3.min.js"></script>
    <style>
        .node circle {
            fill: white;
            stroke: dodgerblue;
            stroke-width: 2px;
        }

        .node text {
            font: 12px sans-serif;
        }

        .link {
            fill: none;
            stroke: #ccc;
            stroke-width: 2px;
        }
    </style>
</head>

<body>
    <script>
        // Graph data
)";

const std::string html_end = R"(
        const width = 600;
        const margin = { top: 40, right: 40, bottom: 40, left: 40 };

        // Calculate required height based on depth levels
        const maxDepth = d3.max(data.nodes, d => d.depth);
        const levelHeight = 100; // Height per level
        const height = (maxDepth + 1) * levelHeight + margin.top + margin.bottom;

        // Create SVG container with dynamic height
        const svg = d3.select("body")
            .append("svg")
            .attr("width", width)
            .attr("height", height);

        // Calculate vertical spacing based on actual depth
        const verticalSpacing = (height - margin.top - margin.bottom) / maxDepth;

        // Position nodes
        data.nodes.forEach(node => {
            // Calculate y based on depth
            node.y = margin.top + node.depth * verticalSpacing;

            // Calculate x position based on horizontal distribution
            const nodesAtDepth = data.nodes.filter(n => n.depth === node.depth);
            const position = nodesAtDepth.indexOf(node);
            const totalAtDepth = nodesAtDepth.length;
            const horizontalSpacing = (width - margin.left - margin.right) / (totalAtDepth + 1);
            node.x = margin.left + horizontalSpacing * (position + 1);
        });

        // Create edges
        const links = svg.selectAll(".link")
            .data(data.edges)
            .enter()
            .append("path")
            .attr("class", "link")
            .attr("d", d => {
                const sourceNode = data.nodes.find(n => n.id === d.source);
                const targetNode = data.nodes.find(n => n.id === d.target);
                return `M${sourceNode.x},${sourceNode.y} L${targetNode.x},${targetNode.y}`;
            });

        // Create node groups
        const nodes = svg.selectAll(".node")
            .data(data.nodes)
            .enter()
            .append("g")
            .attr("class", "node")
            .attr("transform", d => `translate(${d.x},${d.y})`);

        // Add circles to nodes
        nodes.append("circle")
            .attr("r", 20);

        // Add labels to nodes
        nodes.append("text")
            .attr("dy", ".35em")
            .attr("text-anchor", "middle")
            .text(d => d.label);
    </script>
</body>

</html>
)";

namespace autograd
{
    int Scalar::_next_id = 0;

    Scalar Scalar::operator+(const Scalar &other) const
    {
        return Scalar(_data + other._data, std::make_shared<Scalar>(*this), std::make_shared<Scalar>(other));
    }

    Scalar Scalar::operator+(const float other) const
    {
        return Scalar(_data + other, std::make_shared<Scalar>(*this), std::make_shared<Scalar>(Scalar(other)));
    }

    Scalar operator+(const float left, const Scalar &right)
    {
        return Scalar(left + right._data, std::make_shared<Scalar>(Scalar(left)), std::make_shared<Scalar>(right));
    }

    std::ostream &operator<<(std::ostream &ostream, Scalar &scalar)
    {
        std::vector<Scalar> stack;
        Scalar::ordered_graph(scalar, stack);

        for (auto &s : stack)
            ostream << s.data() << " ";

        return ostream;
    }

    void Scalar::ordered_graph(Scalar &scalar, std::vector<Scalar> &stack)
    {
        if (!scalar.visited)
        {
            scalar.visited = true;

            if (scalar._left.has_value())
                ordered_graph(*scalar._left.value(), stack);
            if (scalar._right.has_value())
                ordered_graph(*scalar._right.value(), stack);

            stack.push_back(scalar);
        }
    }

    std::tuple<std::vector<std::tuple<int, int, int>>, std::vector<std::tuple<int, int>>> Scalar::graph()
    {
        std::vector<std::tuple<int, int, int>> nodes;
        std::vector<std::tuple<int, int>> edges;

        std::vector<std::tuple<std::shared_ptr<Scalar>, int>> stack = {{std::make_shared<Scalar>(*this), 0}};
        while (stack.size())
        {
            std::shared_ptr<Scalar> node;
            int depth;

            std::tie(node, depth) = stack.back();
            stack.pop_back();

            if (node->_left.has_value())
            {
                stack.push_back({node->_left.value(), depth + 1});
                edges.push_back({node->id, node->_left.value()->id});
            }

            if (node->_right.has_value())
            {
                stack.push_back({node->_right.value(), depth + 1});
                edges.push_back({node->id, node->_right.value()->id});
            }

            nodes.push_back({node->id, node->_data, depth});
        }

        return {nodes, edges};
    }

    void Scalar::json(const std::string &filename)
    {
        std::vector<std::tuple<int, int, int>> nodes;
        std::vector<std::tuple<int, int>> edges;
        std::tie(nodes, edges) = graph();

        std::ofstream file(filename);

        file << "{\n    \"nodes\": [\n";
        for (int i = 0; i < nodes.size(); i++)
        {
            int id;
            int data;
            int depth;
            std::tie(id, data, depth) = nodes[i];

            file << "        { \"id\": " << id << ", "
                 << "\"label\": \"" << data << "\", "
                 << "\"depth\": " << depth << " }"
                 << (i == nodes.size() - 1 ? "\n" : ",\n");
        }

        file << "    ],\n    \"edges\": [\n";
        for (int i = 0; i < edges.size(); i++)
        {
            int source;
            int target;
            std::tie(source, target) = edges[i];

            file << "        { \"source\": " << source << ", "
                 << "\"target\": " << target << " }"
                 << (i == edges.size() - 1 ? "\n" : ",\n");
        }

        file << "    ]\n}\n";
        file.close();
    }

    void Scalar::html(const std::string &filename)
    {
        std::vector<std::tuple<int, int, int>> nodes;
        std::vector<std::tuple<int, int>> edges;
        std::tie(nodes, edges) = graph();

        std::ofstream file(filename);

        file << html_begin;

        file << "const data = {\n    \"nodes\": [\n";
        for (int i = 0; i < nodes.size(); i++)
        {
            int id;
            int data;
            int depth;
            std::tie(id, data, depth) = nodes[i];

            file << "        { \"id\": " << id << ", "
                 << "\"label\": \"" << data << "\", "
                 << "\"depth\": " << depth << " }"
                 << (i == nodes.size() - 1 ? "\n" : ",\n");
        }

        file << "    ],\n    \"edges\": [\n";
        for (int i = 0; i < edges.size(); i++)
        {
            int source;
            int target;
            std::tie(source, target) = edges[i];

            file << "        { \"source\": " << source << ", "
                 << "\"target\": " << target << " }"
                 << (i == edges.size() - 1 ? "\n" : ",\n");
        }

        file << "    ]\n}\n" << html_end;
        file.close();
    }
}
