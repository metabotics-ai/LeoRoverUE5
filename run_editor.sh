#!/bin/sh
if [ -z "${UE5_ROOT}" ]; then
	printf "Please set UE5_ROOT to path of UnrealEngine's root folder\n"
	exit 1
fi

#change default level, generating DefautlEngine.ini
DEFAULT_RATE=${FIXED_FRAME_RATE:-"100.0"}
DEFAULT_RTF=${TARGET_RTF:-"1.0"}
sed -i -e 's/${FIXED_FRAME_RATE}/'${DEFAULT_RATE}'/g' Config/DefaultEngine.ini
sed -i -e 's/${TARGET_RTF}/'${DEFAULT_RTF}'/g' Config/DefaultEngine.ini

PROJECT_NAME="LeoRoverUE5"

EDITOR_COMMAND="${UE5_ROOT}/Engine/Binaries/Linux/UnrealEditor"

(exec "$EDITOR_COMMAND" "${PWD}/$PROJECT_NAME.uproject")
