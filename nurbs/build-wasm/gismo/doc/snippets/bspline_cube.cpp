#include <gismo.h>

using namespace gismo;

int main(int, char**)
{
    gsKnotVector<> KV (-1, 0, 3,3, 1 ) ;
    gsInfo<< "A knot vector:"<< KV << "\n";

    return 0;
}
