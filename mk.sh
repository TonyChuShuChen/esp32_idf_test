set -e


#export SDK_PATH=/mnt/Share/ESP32/test_2016_7/esp32_sdkng_soc_driver_and_test/esp-idf

export SDK_PATH=/home/chushuchen/Share/Share/ESP32/test_2016_7/esp32-idf-soc-driver-and-test/esp-idf
echo "----------------------"
echo "PLease Check SDK_PATH"
echo "SDK_PATH"
echo $SDK_PATH 
echo "---------------------"
make
echo "------------------------------"
echo "the testje_bin copy to bin "
echo "the bootloader_bin copy to bin"
echo "the partication_bin copy to bin"
echo "--------------------------------"
cp ./build/testje.bin ../bin/
cp ./build/bootloader/bootloader.bin ../bin
cp ./build/partitions_singleapp.bin ../bin
