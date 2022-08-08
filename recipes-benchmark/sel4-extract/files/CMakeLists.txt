# set minimum cmake verision
cmake_minimum_required(VERSION 2.8)

# set project name
project(sel4-extract)

# specify include dir
# include_directories("./include")

# add source
set(SOURCE sel4-extract.c)

# add executable
add_executable (sel4-extract ${SOURCE})


# set compiler options
set(CXX_WAR_AS_ERR -Wall -Wpedantic -Werror) # -Wextra
# set(CXX_MEM_DBG -fsanitize=address -fsanitize=leak -fsanitize=bounds)


target_compile_options(sel4-extract PRIVATE ${CXX_WAR_AS_ERR} ${CXX_MEM_DBG})
target_link_options(sel4-extract  PRIVATE ${CXX_WAR_AS_ERR} ${CXX_MEM_DBG})

# target install
install(TARGETS sel4-extract RUNTIME DESTINATION bin)
