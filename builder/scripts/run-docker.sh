#!/bin/bash
set -e

THISDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source ${THISDIR}/common.sh

if [ ! -z ${CI_BUILD} ]; then
	set -x
fi

if [ -z "$(which docker)" ]; then
	error_and_exit "Install Docker on your system first"
fi

VM_HOME="/home/builder"
IMAGE_REVISION="20210804"
VOLUME_MOUNT_POINT="/workdir"
AAC_OPENSSL_VERSION=${AAC_OPENSSL_VERSION:-"1.1.1k"}
source ${THISDIR}/version
VOLUME_NAME="buildervolume${AAC_VERSION}_openssl${AAC_OPENSSL_VERSION}"
IMAGE_NAME="aac/ubuntu-base:${IMAGE_REVISION}_openssl${AAC_OPENSSL_VERSION}"


EXTRA_OPTIONS=""
if [ ! -z ${QNX_BASE} ] && [ -d ${QNX_BASE} ]; then
	EXTRA_OPTIONS="-v ${QNX_BASE}:${VM_HOME}/qnx700"
fi

execute_command() {
	local tty="-t"
	if [ ! -z ${CI_BUILD} ]; then
		# Runs docker commands without TTY to support CI tools such as Jenkins.
		tty=""
	fi
	docker run -i ${tty} --rm \
	-v ${VOLUME_NAME}:${VOLUME_MOUNT_POINT} \
	-v ${SDK_HOME}:${VM_HOME}/aac \
	-e ANDROID_TOOLCHAIN=${VOLUME_MOUNT_POINT}/android \
	-e HOST_PWD=${PWD} \
	-e HOST_SDK_HOME=${SDK_HOME} \
	${EXTRA_OPTIONS} \
	${IMAGE_NAME} $@
}

if [[ "$(docker images -q ${IMAGE_NAME} 2> /dev/null)" == "" ]] || [ ! -z ${FORCE_DOCKER_IMAGE_REBUILD} ]; then
	note "Building Docker image..."
	docker build --tag ${IMAGE_NAME} --build-arg AAC_OPENSSL_VERSION=${AAC_OPENSSL_VERSION} --build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g) ${BUILDER_HOME}/scripts
fi

if [[ "$(docker volume ls | grep ${VOLUME_NAME} 2> /dev/null)" == "" ]]; then
	note "Creating Docker volume \"${VOLUME_NAME}\"..."
	docker volume create --name ${VOLUME_NAME}
fi

note "Changing permissions for volume..."
execute_command "sudo chown -R $(id -u):$(id -g) ${VOLUME_MOUNT_POINT}" &> /dev/null || note "Non-zero exit code when adjusting volume permissions. Proceeding..."

note "Run Docker image..."
echo ""
echo "*******************"
echo "*** Docker Mode ***"
echo "*******************"
echo ""
execute_command $@
