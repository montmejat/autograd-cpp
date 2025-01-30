#include "scalar.h"
#include <iostream>

int main()
{
    autograd::Scalar s1 = autograd::Scalar(2);
    autograd::Scalar s2 = autograd::Scalar(3);
    auto s3 = 1 + s1 + s2 + 4;

    s3.json(); // export the graph to a JSON file
    s3.html(); // visualize the graph in an HTML file
}
