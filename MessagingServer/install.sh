#!/bin/bash
#This is the script used to install the service msgserverdd
echo "Installing Service msgserver..."
mkdir -p $MSGSERVER_HOME/sbin || { echo 'Making sbin folder failed' ; exit 1; }
mkdir -p $MSGSERVER_HOME/etc || { echo 'Making etc folder failed' ; exit 1; }
mkdir -p $MSGSERVER_HOME/var/run || { echo 'Making run folder failed' ; exit 1; }
mkdir -p $MSGSERVER_HOME/var/log || { echo 'Making log folder failed' ; exit 1; }
cp -p -f ./src/msgserverd $MSGSERVER_HOME/sbin/ || { echo 'Copying msgserver $MSGSERVER_HOME/sbin/ failed' ; exit 1; }
cp -p -f ./msgserverd.conf $MSGSERVER_HOME/etc/ || { echo 'Copying msgserverd.conf to $MSGSERVER_HOME/etc/ failed' ; exit 1; }

cp -f ./scripts/initmsgserverd /etc/init.d/msgserverd || { echo 'Copying initmsgserverd to /etc/init.d/ failed' ; exit 1; }
chkconfig --add msgserverd || { echo 'Adding msgserverd to chkconfig failed' ; exit 1; }
chkconfig --level 3 msgserverd on || { echo 'Making service ON at runlevel 3 failed' ; exit 1; }
echo "Installing Service msgserver Completed!!"
service msgserverd start
