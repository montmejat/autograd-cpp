#include "scalar.h"
#include <iostream>

int main()
{
    autograd::Scalar s1 = autograd::Scalar(2);
    // autograd::Scalar s2 = autograd::Scalar(3);
    auto s3 = s1 + 3 + 4;

    std::cout << "Result: " << s3.data() << std::endl;

    s3.json();
}