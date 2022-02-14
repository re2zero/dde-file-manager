#!/bin/bash

export INTERFACES_PATH=../../plugins/desktop/ddplugin-dbusregister

echo "-->make adaptor and interface of DeviceManagerDBus"
qdbusxml2cpp -i $INTERFACES_PATH/devicemanagerdbus.h -c DeviceManagerAdaptor -l DeviceManagerDBus -a $INTERFACES_PATH/dbus_adaptor/devicemanagerdbus_adaptor devicemanagerdbus.xml
qdbusxml2cpp -c DeviceManagerInterface -p dbus_interface/devicemanagerdbus_interface devicemanagerdbus.xml

echo "-->make adaptor and interface of OperationsStackManagerDbus"
qdbusxml2cpp -i $INTERFACES_PATH/operationsstackmanagerdbus.h -c OperationsStackManagerAdaptor -l OperationsStackManagerDbus -a $INTERFACES_PATH/dbus_adaptor/operationsstackmanagerdbus_adaptor operationsstackmanagerdbus.xml
qdbusxml2cpp -c OperationsStackManagerInterface -p dbus_interface/operationsstackmanagerdbus_interface operationsstackmanagerdbus.xml

