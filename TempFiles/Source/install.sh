#!/bin/bash
#This is the script used to install the service msgserverdd
echo "Installing Service msgserver..."
mkdir -p /opt/msgserverd/sbin || { echo 'Making sbin folder failed' ; exit 1; }
echo -n "[."
mkdir -p /opt/msgserverd/etc || { echo 'Making etc folder failed' ; exit 1; }
echo -n "..."
mkdir -p /opt/msgserverd/var/run || { echo 'Making run folder failed' ; exit 1; }
echo -n "..."
mkdir -p /opt/msgserverd/var/log || { echo 'Making log folder failed' ; exit 1; }
echo -n "..."
cp -f msgserverd /opt/msgserverd/sbin/ || { echo 'Copying msgserverd /opt/msgserverd/sbin/ failed' ; exit 1; }
echo -n "..."
cp -f msgserverd.conf /opt/msgserverd/etc/ || { echo 'Copying msgserverd.conf to /opt/msgserverd/etc/ failed' ; exit 1; }
echo -n "..."
cp -f initmsgserverd /etc/init.d/msgserverd || { echo 'Copying msgserverd to /etc/init.d/ failed' ; exit 1; }
echo -n "..."
chkconfig --add msgserverd || { echo 'Adding msgserverd to chkconfig failed' ; exit 1; }
echo -n "..."
chkconfig --level 3 msgserverd on || { echo 'Making service ON at runlevel 3 failed' ; exit 1; }
echo "...]"
echo "Installing Service msgserver Completed!!"
service msgserverd start
