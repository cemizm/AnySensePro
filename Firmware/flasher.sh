echo "Connect AnySense"
i="0"
while true; do
    st-flash --reset erase >&- 2>&-
    rc=$?;
    if [[ $rc == 0 ]];
    then
        echo "Flashing firmware..."
        st-flash --reset write Releases/AnySensePro.bin 0x08000000 >&- 2>&-
        rc=$?;
        if [[ $rc == 0 ]];
        then
        echo "Flashing bootloader..."
            st-flash --reset write Releases/Bootloader.bin 0x0807C000 >&- 2>&-
            rc=$?;
            if [[ $rc == 0 ]];
            then
                i=$[$i+1]
                printf '\7'
                echo "Count: $i done... Disconnect now"
                while  [[ $rc == 0 ]]; do
                    st-flash --reset read /dev/null 0x08000000 1 >&- 2>&-
                    rc=$?;
                done
            else
                echo "Error."
            fi
        else
            echo "Error."
        fi
        echo ""
        echo ""
        echo "Connect AnySense"
    fi
done
