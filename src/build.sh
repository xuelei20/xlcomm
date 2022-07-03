projectnames=(xlbase test)

while [[ i -lt ${#projectnames[@]} ]]; do
	echo -e "\n====== Build ${projectnames[i]}... ======"
	cd $XLCOMM_ROOT_PATH/src/${projectnames[i]}/makefile.build
	./build.sh
  let i++
done
echo -e "\nall project compile success !"

# cd $XLCOMM_ROOT_PATH/src/xlbase/makefile.build
# ./build.sh