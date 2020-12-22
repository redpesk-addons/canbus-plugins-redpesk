# Abstract

This project proposes several plugins for the CAN bus Binding for redpesk@. This
includes the following:

* **ars408**: Marine Radar to detect near object support.
* **canEthernet**: Add support to a CAN Ethernet device, this is not a normal
CANbus plugins as it doesn't provide any new signals to subscribe but only the
support of a special CAN devices. This is a POC that prove that we could extend
the CANbus with plugin by a more complex way than only add CAN signals.
* **gps** GPS signals
* **j1939** and **j1939-openxc** 2 configurations used describing the Truck & Bus FMS standard signals. The **j1939-openxc** is the complete standard (could
be long to build) and the **j1939** only a subset of the first.
* **joystick** A plugin to support a Marine joystick device of the ZF Padova
company
* **n2k-basic** Add support for some NMEA2000 signals
* **simrad-draft** Another plugin that add experimental support for SIMRAD
devices supports.

The instruction to install, build and use are the same than for [community plugins](../community-plugins/).
If you want to install from the package of Redpesk repositories then just
replace the package name with the correct plugin's name.
