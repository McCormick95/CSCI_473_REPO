make clean all

./make-2d 10 10 matrix_1

./print-2d matrix_1

# ./pth-stencil-2d 10 matrix_1 out_1 0 3 all_1.RAW
./omp-stencil-2d 10 matrix_1 out_1 0 3 all_1.RAW

./print-2d out_1

