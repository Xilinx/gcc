#!/bin/sh
set -x

run_command_for_branch () {
       echo '--->' Running build
       BRANCH=$1
       cd $SRC_DIR
       HASH=`git log --pretty=format:%H ${BRANCH}| head -n 1`
	if [ ! -z "${IGNORE_RUNNING}" -o ! -e ${LOG_DIR}/running ]; then
		export GIT_BRANCH=$BRANCH
		$BASE_DIR/run_build.sh 
	fi
}

if [  -z "${IGNORE_RUNNING}" -a -e ${LOG_DIR}/running ]; then
	exit 0
fi

cd ${SRC_DIR}
git remote update
if  test -z "`git tag -l --contains gcc/remotes/graphite test_\*`"; then
		run_command_for_branch gcc/remotes/graphite
fi

# Run the command on every yet untested branch head
for branch in `git branch -a | sed -e 's/\*//;s/(no branch)//'` ; do
       echo Checking \'$branch\' for new commits
       if test -z "`git tag -l --contains $branch test_\*`"; then
	  run_command_for_branch $branch
       fi

done
