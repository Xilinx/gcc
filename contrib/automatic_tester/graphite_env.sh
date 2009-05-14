#!/bin/sh
source /n/16/grosser/daily_git_builds/graphite_env.sh.example
export CLOOG=/home/grosser/usr/install/cloog-ppl-tobias
export PPL=/home/grosser/usr/install/ppl 
export MPFR=/opt/cfarm/mpfr-2.3.2/
export LD_LIBRARY_PATH=${CLOOG}/lib:${PPL}/lib:${MPFR}/lib
export EMAIL="tobi-grosser@web.de, sebpop@gmail.com"
export BASE_DIR='/n/16/grosser/daily_git_builds'
export EXTRA_TEST_DIR='/n/16/grosser/daily_tests'
export SRC_DIR=${BASE_DIR}/src
export OBJ_DIR=${BASE_DIR}/obj
export TMP_SRC_DIR=${BASE_DIR}/tmp_src
export LOG_DIR=${BASE_DIR}/log
export INSTALL_DIR=${BASE_DIR}/install
export PATH=/n/16/grosser/usr/install/git_trunk/bin/:${PATH}
export ENABLE_LANGUAGES=""
export BOOTSTRAP_FLAG=""
export EMAIL="tobi-grosser@web.de, sebpop@gmail.com, nemokingdom@gmail.com"
export RUN_EXTRA_TESTS=0
export RUN_GCC_TESTS=1

