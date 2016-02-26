BACKUP=$PYTHONPATH
export PYTHONPATH=$BACKUP:../../../mavlink/
rm -R onboard/
python -m pymavlink.tools.mavgen AnySense.xml --lang C -o onboard/
export PYTHONPATH=$BACKUP
