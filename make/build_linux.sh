versions=('2014' , '2015' , '2016')
op_system='linux'
plugin_name='polypaint_importer'
make_dir='/home/jani/Desktop/polypaint_importer/build/'$op_system'/'
destination_dir='/home/jani/Desktop/polypaint_importer/build/'$op_system'/'

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color
echo ""
printf "${RED}[Starting $plugin_name build...]${NC}\n"
echo ""

for i in "${versions[@]}"
	do
		if [ -e $make_dir$i ]
		then
			printf "${GREEN}[Found Maya v$i]${NC}\n"
			cmake --build $make_dir$i --config Release
			
			bundle_file=$make_dir$i'/src/Release/'$plugin_name'.su'
			
			
			if [ -e $bundle_file ]
			then
				mkdir -p $destination_dir$i
				
				cp $bundle_file $destination_dir$i
			fi
			
		fi
		
	done


echo ""
printf "${GREEN}[Build finished...]${NC}\n"
echo ""
