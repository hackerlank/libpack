// Copyright 2013 Timo Schneider and Fredrik Berg Kjolstad
//
// This file is part of the libpack packing library.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT in the top level directory for details.

#include <string>
#include <mpi.h>

#include "test.hpp"
#include "../ddt_jit.hpp"

int main(int argc, char** argv) {

    char* mpi_inbuf;
    char* mpi_outbuf;
    char* farc_inbuf;
    char* farc_outbuf;

    MPI_Init(&argc, &argv);

    test_start("unpack(2, indexed_block[blocklen=2, disp=(0,5,8), MPI_INT])");
    init_buffers(20*sizeof(int), &mpi_inbuf, &farc_inbuf, &mpi_outbuf, &farc_outbuf);

    MPI_Datatype mpitype; 
    int disp[3] = {0, 5, 8};
    int blocklen = 3;

    MPI_Type_create_indexed_block(3, blocklen, disp, MPI_INT, &mpitype);
    MPI_Type_commit(&mpitype);

    farc::DDT_Init();
    farc::Datatype* t1 = new farc::PrimitiveDatatype(farc::PrimitiveDatatype::INT);
    farc::Datatype* t2 = new farc::IndexedBlockDatatype(3, blocklen, disp, t1);
    farc::DDT_Commit(t2);
    farc::DDT_Unpack(farc_inbuf, farc_outbuf, t2, 2);

    int position = 0;
    MPI_Unpack(mpi_inbuf, 20*sizeof(int), &position, mpi_outbuf, 2, mpitype, MPI_COMM_WORLD);

    int res = compare_buffers(20*sizeof(int), &mpi_inbuf, &farc_inbuf, &mpi_outbuf, &farc_outbuf);
    free_buffers(&mpi_inbuf, &farc_inbuf, &mpi_outbuf, &farc_outbuf);
    test_result(res);

    MPI_Finalize();

    return 0;

}

