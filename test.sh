OUT=out.bin
DECODED=decoded.txt

FILES=$(find txt -type f | sort)

for FILE in ${FILES}; do
    # echo Trying ${FILE}
    ./main --pack -s ${FILE} -d ${OUT} # > /dev/null
    ./main --unpack -s ${OUT} -d ${DECODED} # > /dev/null

    SUM_ORIG=($(md5sum ${FILE}))
    SUM_DECODED=($(md5sum ${DECODED}))

    if [[ "${SUM_ORIG}" == "${SUM_DECODED}" ]]; then
        echo "OK"
    else
        echo "FAIL"
    fi

    echo ""
    rm ${OUT} ${DECODED}
done
