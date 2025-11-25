#include <gismo.h>

using namespace gismo;

int main(int, char**)
{
    gsKnotVector<>   kv (-1, 0, 3, 3, 1 );
    gsBSplineBasis<> basis(kv);
    gsInfo << basis.detail() << "\n";

    basis.uniformRefine();
    gsInfo << basis.detail() << "\n";

    return 0;
}
