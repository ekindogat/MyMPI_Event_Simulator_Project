# AUTOMATIZATION FOR BUILDING THE PROGRAM WITH CMAKE THEN MAKE
# THEN EXECUTING THE PROGRAM

# Initial creation of build/ folder
if [ ! -d "build" ]; then
	mkdir build
fi

# Into the build/ folder
cd build

# Build with CMakeList
cmake ..

# Compile & create executables
make

# Execute the program

./mpi_parallel_sort_simulator
