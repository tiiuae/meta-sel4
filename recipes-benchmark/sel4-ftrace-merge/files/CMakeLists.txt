# set minimum cmake verision
cmake_minimum_required(VERSION 2.8)

# set project name
project(ftrace-merge)

# specify include dir
include_directories("./include")

# add source
set(SOURCE ftrace-merge.c)

# add executable
add_executable (ftrace-merge ${SOURCE})


# set compiler options
set(CXX_WAR_AS_ERR -Wall -Wpedantic -Werror) # -Wextra
# set(CXX_MEM_DBG -fsanitize=address -fsanitize=leak -fsanitize=bounds)


target_compile_options(ftrace-merge PRIVATE ${CXX_WAR_AS_ERR} ${CXX_MEM_DBG})
target_link_options(ftrace-merge  PRIVATE ${CXX_WAR_AS_ERR} ${CXX_MEM_DBG})

# target install
install(TARGETS ftrace-merge RUNTIME DESTINATION bin)
