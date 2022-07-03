# compile amd64 (x64)
export XL_CROSS_PLATFORM=linux_amd64
source $XLCOMM_ROOT_PATH/automation/cross_env/$XL_CROSS_PLATFORM.sh
make clean; make -j8 -k 2>&1 | tee build_$XL_CROSS_PLATFORM.log