BACKUP=$PYTHONPATH
export PYTHONPATH=$BACKUP:../../../mavlink/
rm -R onboard/
cp ../../../mavlink/message_definitions/v1.0/common.xml .
python -m pymavlink.tools.mavgen AnySense.xml --lang C -o onboard/
export PYTHONPATH=$BACKUP
