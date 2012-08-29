#include "kernel.h"
int main(int argc, char** argv){
    Kernel* kernel = new Kernel(argc, argv);
    kernel->main();
}
