#include <string>
#include <mpi.h>

#include "../ddt_jit.hpp"
#include "test.hpp"

int main(int argc, char** argv) {

    int position = 0;
    char* mpi_inbuf;
    char* mpi_outbuf;
    char* farc_inbuf;
    char* farc_outbuf;

    MPI_Init(&argc, &argv);

    test_start("vector_bug");

    FARC_DDT_Init();
    FARC_Datatype* t1 = new FARC_PrimitiveDatatype(FARC_PrimitiveDatatype::DOUBLE);
    FARC_Datatype* t2 = new FARC_VectorDatatype(t1, 64, 7, 6480);
    FARC_DDT_Commit(t2);

    MPI_Datatype newtype;
    MPI_Type_vector(64, 7, 6480, MPI_INT, &newtype);
    MPI_Type_commit(&newtype);

    int extent = t2->getExtent();
    init_buffers(extent, &mpi_inbuf, &farc_inbuf, &mpi_outbuf, &farc_outbuf);

    FARC_DDT_Pack(farc_inbuf, farc_outbuf, t2, 1);
    MPI_Pack(mpi_inbuf, 1, newtype, mpi_outbuf, extent, &position, MPI_COMM_WORLD);

    int res = compare_buffers(extent, &mpi_inbuf, &farc_inbuf, &mpi_outbuf, &farc_outbuf);
    free_buffers(&mpi_inbuf, &farc_inbuf, &mpi_outbuf, &farc_outbuf);
    test_result(res);

    MPI_Finalize();

    return 0;

}
