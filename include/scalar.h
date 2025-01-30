#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace autograd
{
    class Scalar
    {
        using OptionalScalar = std::optional<std::shared_ptr<autograd::Scalar>>;

        int id;
        float _data;
        float _grad;
        OptionalScalar _left;
        OptionalScalar _right;
        bool visited = false;

        static int _next_id;

        Scalar(float data, OptionalScalar left, OptionalScalar right)
            : _data(data), _grad(0.0), _left(left), _right(right) { id = _next_id++; };
        static void ordered_graph(Scalar &scalar, std::vector<Scalar> &stack);

    public:
        Scalar() : _data(0.0), _grad(0.0) { id = _next_id++; };
        Scalar(float data) : _data(data), _grad(0.0) { id = _next_id++; };

        float data() { return _data; };
        float grad() { return _grad; };
        OptionalScalar left() { return _left; };
        OptionalScalar right() { return _right; };

        Scalar operator+(const Scalar &other) const;
        Scalar operator+(const float other) const;
        friend Scalar operator+(const float left, const Scalar &right);

        friend std::ostream &operator<<(std::ostream &os, Scalar &scalar);

        std::tuple<std::vector<std::tuple<int, int, int>>, std::vector<std::tuple<int, int>>> graph();
        void json(const std::string &filename = "graph.json");
        void html(const std::string &filename = "graph.html");
    };
}
