#!/bin/sh
set -x

# The user of this script has to source the environment script:
# . ${BASE_DIR}/graphite_env.sh

FIRST_MAIL_SENT=

DATE=`date +'%Y_%m_%d_%H_%M_%S'`

LOG_DIR_CURRENT=${LOG_DIR}/${DATE}
INSTALL_DIR_CURRENT=${INSTALL_DIR}/${DATE}
BUILD_DIR_CURRENT=${BUILD_DIR}/${DATE}
OBJ_DIR_CURRENT=${OBJ_DIR}/${DATE}
SRC_DIR_CURRENT=${SRC_DIR}
EXTRA_TESTS_OUTPUT=${LOG_DIR_CURRENT}/extra_tests_output
UPDATE_CONTAINS_MERGE=no

if [  -z "${IGNORE_RUNNING}" -a -e ${LOG_DIR}/running ]; then
	exit 0
fi

if [ -z "$SRC_DIR" ]; then
	echo "Build environment not set"
	exit 0
fi

touch ${LOG_DIR}/running

# Are we working with git or svn
GIT_SRC=""
if [ -e ${SRC_DIR}/.git ]; then
	GIT_SRC=1
fi
if [ "${GIT_BRANCH}x" != "x" ] ; then
	SRC_DIR_CURRENT=${TMP_SRC_DIR}/${DATE}
	git clone ${SRC_DIR} ${SRC_DIR_CURRENT}
	cd ${SRC_DIR_CURRENT}
	git fetch ${SRC_DIR} 'refs/remotes/*:refs/remotes/src/*'
	git fetch ${SRC_DIR} 'refs/heads/*:refs/remotes/src/*'
	git checkout src/$GIT_BRANCH
fi

MAIL_SUBJECT="[graphite] Regression testing on ${DATE}"
if [ "${GIT_SRC}x" = "x" ] ; then
	MAIL_SUBJECT="${MAIL_SUBJECT}"
else
	MAIL_SUBJECT="${MAIL_SUBJECT} ${GIT_BRANCH}"
fi

mkdir -p ${LOG_DIR_CURRENT}

touch ${LOG_DIR_CURRENT}/mutt.cfg
MUTT_CMD="mutt"

if [ "${EMAIL_REALNAME}x" != "x" -a "${EMAIL_FROM}x" != "x" ] ; then
	echo "set use_from=yes" >> ${LOG_DIR_CURRENT}/mutt.cfg
	echo "set realname=\"${EMAIL_REALNAME}\"" >> ${LOG_DIR_CURRENT}/mutt.cfg
	echo "set from=\"${EMAIL_FROM}\"" >> ${LOG_DIR_CURRENT}/mutt.cfg
	MUTT_CMD=${MUTT_CMD}\ -F\ ${LOG_DIR_CURRENT}/mutt.cfg
fi


cat ${BASE_DIR}/graphite_env.sh > ${LOG_DIR_CURRENT}/gcc_env.sh
echo 'PATH='${INSTALL_DIR_CURRENT}'/bin:${PATH}' >> ${LOG_DIR_CURRENT}/gcc_env.sh
echo 'LD_LIBRARY_PATH='${INSTALL_DIR_CURRENT}'/lib:${LD_LIBRARY_PATH}' \
	>> ${LOG_DIR_CURRENT}/gcc_env.sh
echo 'LD_LIBRARY_PATH='${INSTALL_DIR_CURRENT}'/lib64:${LD_LIBRARY_PATH}' \
	>> ${LOG_DIR_CURRENT}/gcc_env.sh

mail_success () {
  ATTACHMENTS=""
  for file in `ls ${LOG_DIR_CURRENT}/*.compare \
      ${EXTRA_TESTS_OUTPUT}/*`; do
    if [ -s ${file} ] ; then
      ATTACHMENTS="${ATTACHMENTS} -a ${file}"
    fi
  done
  if [ -e ${EXTRA_TEST_DIR}/report/${DATE} ]; then
      ATTACHMENTS="${ATTACHMENTS} -a ${EXTRA_TEST_DIR}/report/${DATE}"
  fi
  echo "BUILD SUCCESSFUL" >> ${LOG_DIR_CURRENT}/info.log
  ${MUTT_CMD} -s "Re: ${MAIL_SUBJECT}" -i ${LOG_DIR_CURRENT}/info.log \
    $ATTACHMENTS $EMAIL </dev/null
}

build_git_tag () {
	for file in `ls ${LOG_DIR_CURRENT}/info.log \
		     ${LOG_DIR_CURRENT}/*.compare` ; do
		echo ${file} >> ${LOG_DIR_CURRENT}/git.tag
		cat ${file} >> ${LOG_DIR_CURRENT}/git.tag
	done
}

mail_status () {
  if [ -z "${FIRST_MAIL_SENT}" ]; then
	${MUTT_CMD} -s "${MAIL_SUBJECT}" -i ${LOG_DIR_CURRENT}/info.log $EMAIL < /dev/null
	FIRST_MAIL_SENT="1"
  else
	${MUTT_CMD} -s "Re: ${MAIL_SUBJECT}" -i ${LOG_DIR_CURRENT}/info.log $EMAIL < /dev/null
  fi
}

mail_failure () {
  ATTACHMENTS=""
  for file in `ls ${LOG_DIR_CURRENT}/*.compare \
      ${LOG_DIR_CURRENT}/build_single_thread.log` ; do
    ATTACHMENTS="${ATTACHMENTS} -a ${file}"
  done
  echo "BUILD FAILED" >> ${LOG_DIR_CURRENT}/info.log
  ${MUTT_CMD} -s "Re: ${MAIL_SUBJECT}" -i ${LOG_DIR_CURRENT}/info.log \
    $ATTACHMENTS $EMAIL </dev/null
}

log () {
  NOW=`date +'%H:%M'`
  echo ${NOW} ='>' $1 >>  ${LOG_DIR_CURRENT}/info.log
}

error () {
	log $1
	mail_failure
	rm -f ${LOG_DIR}/running
	if [ "${GIT_SRC}x" != "x" ]; then
		cd ${SRC_DIR_CURRENT}
		build_git_tag
		git tag -F ${LOG_DIR_CURRENT}/git.tag test_${DATE}_error
		git push --tags origin
	fi
	exit 1
}

git_get_last_hash () {
	cd ${SRC_DIR_CURRENT}
	LAST_HASH="HEAD^"
	for h in `git log --pretty=format:%H`; do
		RUNS="`git tag -l --contains=$h test_\*_success`"
		if ! test -z "$RUNS"; then
			LAST_HASH=$h
			break
		fi
	done
}

update_src () {
	if [ "${GIT_SRC}x" = "x" ]; then
		log "SVN update"

		OLD_REV=`cd ${SRC_DIR_CURRENT} && svn info | grep "Last Changed Rev:" | cut -d " " -f 4`
		OLD_REV_PLUS_ONE=`echo "${OLD_REV} + 1" | bc -l`
		cd ${SRC_DIR_CURRENT} && svn update 2>&1 > ${LOG_DIR_CURRENT}/update.log

		if [ -z "${FORCE_BUILD}" -a -z "`grep 'Updated to' ${LOG_DIR_CURRENT}/update.log`" ] ; then
			rm -f ${LOG_DIR}/running
			rm -rf ${LOG_DIR_CURRENT}
			exit 0
		fi

		log "Updated to:"
		CURRENT_REV=`cd ${SRC_DIR_CURRENT} && svn info | grep "Last Changed Rev:" | cut -d " " -f 4`
		echo "http://gcc.gnu.org/viewcvs?root=gcc&view=rev&rev=${CURRENT_REV}" >> ${LOG_DIR_CURRENT}/info.log
		cd ${SRC_DIR_CURRENT} && svn log -r${OLD_REV_PLUS_ONE}:${CURRENT_REV} >> ${LOG_DIR_CURRENT}/info.log
		if cat ${LOG_DIR_CURRENT}/info.log | grep -q "Merge from mainline"; then
		    UPDATE_CONTAINS_MERGE=yes
		fi
	else
		log "GIT build"
		git_get_last_hash
		GIT_WEB_REPOSITORY=`echo ${GIT_WEB_REPOSITORY} \
			| sed -e 's/\//\\\\\//g'`
	      	cd ${SRC_DIR_CURRENT} && git log $LAST_HASH.. \
		| sed -e 's/\(.*\)\(git-svn-id.*@\)\([0-9]*\)/\1http:\/\/gcc.gnu.org\/viewcvs?root=gcc\&view=rev\&rev=\3\
\1\2\3/g'  \
		| sed -e "s/commit \([0-9a-f]*\)/commit \1\n${GIT_WEB_REPOSITORY}\1/" >> ${LOG_DIR_CURRENT}/info.log
	fi
}

delete_src_dir () {
	if [ "${GIT_SRC}x" != "x" ]; then
		rm -rf ${SRC_DIR_CURRENT}
	fi
}

get_previous_date () {
	if [ "${GIT_SRC}x" = "x" ]; then
    		PREVIOUS_DATE=`ls ${LOG_DIR}/ | grep _success | sort \
			       | tail -n 1`
	else
		git_get_last_hash
		PREVIOUS_DATE=`echo $RUNS | sed -e 's/ /\n/g' | sort -r \
			       | tail -n 1| sed -e 's/test_//g' \
			       | sed -e 's/_success//g'`
	fi
}

run_compare_tests () {
    TARGET=`${SRC_DIR_CURRENT}/config.guess || error "{$SRC_DIR_CURRENT} does not contain a proper GCC sources"`
    TESTLOGS="gcc/testsuite/gcc/gcc
gcc/testsuite/gfortran/gfortran
gcc/testsuite/g++/g++
gcc/testsuite/objc/objc
$TARGET/libstdc++-v3/testsuite/libstdc++
$TARGET/libffi/testsuite/libffi
$TARGET/libjava/testsuite/libjava
$TARGET/libgomp/testsuite/libgomp
$TARGET/libmudflap/testsuite/libmudflap"

# Copy testlogs
    for LOG in $TESTLOGS ; do
	if [ -f $OBJ_DIR_CURRENT/$LOG.sum ]; then
	    cp $OBJ_DIR_CURRENT/$LOG.sum ${LOG_DIR_CURRENT}
	    cp $OBJ_DIR_CURRENT/$LOG.log ${LOG_DIR_CURRENT}
	fi
    done

    get_previous_date
    LOG_DIR_PREVIOUS=${LOG_DIR}/${PREVIOUS_DATE}

    for LOG in ${TESTLOGS} ; do
	LOG=`basename $LOG`
    	#Remove date from libjava
    	sed -i -e "s/.*${DATE}//g" ${LOG_DIR_CURRENT}/$LOG.sum
	if [ -f ${LOG_DIR_CURRENT}/$LOG.sum -a \
	    -f ${LOG_DIR_PREVIOUS}/$LOG.sum ]; then
	    ${SRC_DIR_CURRENT}/contrib/compare_tests ${LOG_DIR_PREVIOUS}/$LOG.sum \
		${LOG_DIR_CURRENT}/$LOG.sum > ${LOG_DIR_CURRENT}/$LOG.compare
	fi
    done
}

log "Start run ${DATE}" 
update_src 
log "Configure"
mkdir -p ${OBJ_DIR_CURRENT}
cd ${OBJ_DIR_CURRENT}
${SRC_DIR_CURRENT}/configure --prefix=${INSTALL_DIR_CURRENT} --with-mpfr=${MPFR}\
    --with-ppl=${PPL} --with-cloog=${CLOOG} \
    ${ENABLE_LANGUAGES} ${BOOTSTRAP_FLAG} >> ${LOG_DIR_CURRENT}/build.log 2>&1 
if [ $? -ne 0 ] ; then
	error "Config failed"
fi

log "Build"
mail_status 

${MAKE} -j${JMAKE} >> ${LOG_DIR_CURRENT}/build.log 2>&1 
if [ $? -ne 0 ] ; then
	${MAKE} >> ${LOG_DIR_CURRENT}/build_single_thread.log 2>&1 
	error "Build failed"
fi

log "Install to ${INSTALL_DIR_CURRENT}"
${MAKE} -j${JMAKE} install >> ${LOG_DIR_CURRENT}/install.log 2>&1 
if [ $? -ne 0 ] ; then
	error "Install failed"
fi


if [ ${RUN_GCC_TESTS} -ne 0 ]; then
    log "Run tests (testlogs are in ${LOG_DIR_CURRENT})"
    mail_status 
    ${MAKE} -j${JMAKE} -k check > ${LOG_DIR_CURRENT}/test.log 2>&1 
    log "Run compare_tests "
    run_compare_tests
fi

rm -rf ${OBJ_DIR_CURRENT}

if [ ${RUN_EXTRA_TESTS} -ne 0 ]; then
    log "Run extra tests"
    mail_status
    if [ -e ${EXTRA_TEST_DIR}/go.sh ]; then
	. ${LOG_DIR_CURRENT}/gcc_env.sh
	mkdir -p ${EXTRA_TESTS_OUTPUT}
	${EXTRA_TEST_DIR}/go.sh ${DATE} ${EXTRA_TESTS_OUTPUT} ${UPDATE_CONTAINS_MERGE} > ${LOG_DIR_CURRENT}/extra_test.log 2>&1
    fi
fi

mail_success
ln -s ${LOG_DIR_CURRENT} ${LOG_DIR_CURRENT}_success
if [ "${GIT_SRC}x" != "x" ]; then
	build_git_tag
	git tag -F ${LOG_DIR_CURRENT}/git.tag test_${DATE}_success
	git push --tags origin
fi
rm -f ${LOG_DIR}/running
delete_src_dir
exit 0
