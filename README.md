# Low level CAN signaling binder

Low level CAN bus binder. Based upon OpenXC vi-firmware project. Purpose of this project is to offer a low level binding to an AGL platform, idea remains the same than the vi-firmware project. It's meant to generate from a JSON file describing CAN messages and diagnostic message (OBD2 for now) present in a car, a cpp file to integrate with the project and compile all together. Result will be a widget file to install on an AGL target system.

# Prerequirements

- Make sure you already have set up the AGL SDK before using the following [guide][SDK_instructions].

- This repo make use of git submodule, make sure to execute the following commands from the repository once cloned :

```bash
$ git submodule init
$ git submodule update
```

- An [USB CAN adapter][USB_CAN] connected to OBD2 connector through the [right cable][OBD2_cable].

- Make sure you have installed the AGL generator else you aren't able to generate custom low-level CAN binding. Generator can be found [here][generator] with the attached instruction to install and run it. It will produce a *configuration-generated.cpp* file to paste in the source, *src/*, directory.

# Getting started

## Compile and install the binding

With an AGL SDK environment correctly set, I encourage you to set the TARGET variable in the root CMakeLists.txt file if you have an AGL target already running in your network. Then you can directly build and install the binding and source directory on your target system.

Execute commands to get your binding compile :

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
```

And if you have set TARGET variable, you can install it on your AGL system :

```bash
$ make install
[ 16%] Built target bitfield
[ 27%] Built target isotp
[ 40%] Built target openxc
[ 48%] Built target uds
[ 97%] Built target low-can-binding
[100%] Built target widget
Install the project...
-- Install configuration: ""
true
{ "added": "low-can-binding@0.1" }
```

It's possible that you'll see the following message :

```bash
Error org.freedesktop.DBus.Error.Failed: "system error"
```
It's because installation remove the binding before installing it. If it is the first time that you make the installation then you'll have this message in place of ***true***.

To install it manually, you need to copy the *low-can-binding.wgt* file on your target, then from it execute the following commands :

On your host, to copy over the network :
```bash
$ scp low-can-binding.wgt root@<target_IP>:~
```

On the target, assuming ***wgt*** file is in the root home directory :

```bash
~# afm-util install low-can-binding.wgt
{ "added": "low-can-binding@0.1" } 
```
## Install AFB Websocket CLI client to test the binding.

You can test it using afb-client-demo CLI tool provided by the RPM package *libafbwsc-dev*. You can find this package in your build environment, using docker SDK recommended setup the file is */xdt/build/tmp/deploy/rpm/<your-target-arch>/*. Using Renesas RCar Gen2, porter board, you have to copy the file like this if your board is connected to your network and you know its IP address:

```bash
$ scp /xdt/build/tmp/deploy/rpm/cortex15hf_neon/libafbwsc-dev-1.0-r0.cortexa15hf_neon.rpm root@<target_IP>:~
```

Else, you have to copy into the SDcard with the AGL image installed on it.

From the docker image copy RPM to the shared directory between docker image and your host:

```bash
$ cp /xdt/build/tmp/deploy/rpm/cortex15hf_neon/libafbwsc-dev-1.0-r0.cortexa15hf_neon.rpm ~/share
```

Then plugin you SDcard in your Linux host (Windows can't read ext4 filesystem AGL runs on) and copy RPM file on it.

From you host, identify SDcard block device node here it is **sdc** with the correct capacity automounted by the desktop manager:

```bash
$ lsblk
loop1                                                                                     7:1    0     2G  0 loop 
└─docker-253:0-3146365-pool                                                             253:3    0   100G  0 dm   
  └─docker-253:0-3146365-e9f80849a2681e18549d3a4238cbf031e44052e36cd88a0abf041804b799b61c
    253:4    0    10G  0 dm   /var/lib/docker/devicemapper/mnt/e9f80849a2681e18549d3a4238cbf031e44052e36cd88a0abf041804b799b61c
sdb                                                                                       8:16   0 238.5G  0 disk 
├─sdb2                                                                                    8:18   0   238G  0 part 
│ └─Shamash-agl                                                                         253:1    0   238G  0 lvm  /home/claneys/Workspace/agl-docker
└─sdb1                                                                                    8:17   0   500M  0 part /boot
sr0                                                                                      11:0    1  1024M  0 rom  
loop0                                                                                     7:0    0   100G  0 loop 
└─docker-253:0-3146365-pool                                                             253:3    0   100G  0 dm   
  └─docker-253:0-3146365-e9f80849a2681e18549d3a4238cbf031e44052e36cd88a0abf041804b799b61c
    253:4    0    10G  0 dm   /var/lib/docker/devicemapper/mnt/e9f80849a2681e18549d3a4238cbf031e44052e36cd88a0abf041804b799b61c
sdc                                                                                       8:32   1  14.9G  0 disk 
└─sdc1                                                                                    8:33   1     2G  0 part /run/media/claneys/97f418a5-612f-44e9-b968-a19505695151
sda                                                                                       8:0    0 931.5G  0 disk 
├─sda2                                                                                    8:2    0   500G  0 part 
│ ├─Shamash-home                                                                        253:2    0   150G  0 lvm  /home
│ └─Shamash-root                                                                        253:0    0    50G  0 lvm  /
└─sda1                                                                                    8:1    0    16G  0 part [SWAP]
```

Copy, still from your host:

**Careful : Make sure to sync IO with sync command before unplug your SDcard. It could be corrupted if removed before all pending IO aren't done.**

```bash
$ sudo cp ~/devel/docker/share/libafbwsc-dev-1.0-r0.cortexa15hf_neon.rpm /run/media/claneys/97f418a5-612f-44e9-b968-a19505695151/home/root
$ sync
```

Insert the modified SDcard in your Porter board and boot from it. Your are ready to go.

## Configure the AGL system

### Virtual CAN device

 Connected to the target, here is how to load the virtual CAN device driver and set up a new vcan device :

```bash
 ~# modprobe vcan
 ~# ip link add vcan0 type vcan
 ~# ip link set vcan0 up
 ```

### CAN device using the USB CAN adapter

Using real connection to CAN bus of your car using the USB CAN adapter connected to the OBD2 connector. (this instruction assuming a speed of 500000kbps for your device, you can try supported bitrate like 125000, 250000 if 500000 doesn't work) :

```bash
~# modprobe can
~# ip link set can0 type can bitrate 500000
~# ip link set can0 up
~# ip link show can0
  can0: <NOARP,UP,LOWER_UP,ECHO> mtu 16 qdisc pfifo_fast state UNKNOWN qlen 10
    link/can 
    can state ERROR-ACTIVE (berr-counter tx 0 rx 0) restart-ms 0 
    bitrate 500000 sample-point 0.875 
    tq 125 prop-seg 6 phase-seg1 7 phase-seg2 2 sjw 1
    sja1000: tseg1 1..16 tseg2 1..8 sjw 1..4 brp 1..64 brp-inc 1
    clock 16000000
```

## Configure the binding

Configure the binding specifying in the JSON configuration file the CAN device(s) that it will to connect to. Edit file */var/lib/afm/applications/low-can-binding/0.1/can_buses.json* and change the CAN device name to the one you have :

```json
{
	"canbus":  "can0"
}
```

If you have several specify CAN bus devices using an array:

```json
{
	"canbus": [ "vcan0", "can0" ]
}
```

## Run it, test it, use it !

You can run the binding using **afm-util** tool, here is the classic way to go :

```bash
~# afm-util run low-can-binding@0.1
1
```

You can find instructions to use afm-util tool [here][afm-util], as well as documentation about Application Framework.

But you can't control nor interact with it because you don't know security token that **Application Framework** gave it at launch. So, to test it, it is better to launch the binding manually. In the following example, we will use port **1234** and left empty security token for testing purpose :

```bash
~#  afb-daemon --ldpaths=/usr/lib/afb:/var/lib/afm/applications/low-can-binding/0.1/libs/ --rootdir=/var/lib/afm/applications/low-can-binding/0.1/ --port=1234 --token=
NOTICE: binding [/usr/lib/afb/afb-dbus-binding.so] calling registering function afbBindingV1Register
NOTICE: binding /usr/lib/afb/afb-dbus-binding.so loaded with API prefix dbus
NOTICE: binding [/usr/lib/afb/authLogin.so] calling registering function afbBindingV1Register
NOTICE: binding /usr/lib/afb/authLogin.so loaded with API prefix auth
NOTICE: binding [/var/lib/afm/applications/low-can-binding/0.1/libs//low-can-binding.so] calling registering function afbBindingV1Register
NOTICE: binding /var/lib/afm/applications/low-can-binding/0.1/libs//low-can-binding.so loaded with API prefix low-can
NOTICE: Waiting port=1234 rootdir=/var/lib/afm/applications/low-can-binding/0.1/
NOTICE: Browser URL= http:/*localhost:1234
NOTICE: vcan0 device opened and reading {binding low-can}
NOTICE: Initialized 1/1 can bus device(s) {binding low-can}
```

Then connect to the binding using previously installed ***AFB Websocket CLI*** tool :

```bash
~# afb-client-demo ws://localhost:1234/api?token=
```

You will be on an interactive session where you can pass ask directly to the binding API. Binding provide for the moment 2 verbs, subscribe and unsubscribe that can take argument by a JSON **event** object taking a CAN message name as value. To use the ***AFB Websocket CLI*** tool, a command line will be like the following :

```
<api> <verb> <arguments>
```

Where API will be : ***low-can***.
Verb : ***subscribe*** or ***unsubscribe***
Arguments : ***{ "event": "driver.doors.open" }***

### Subscription and unsubscription

You can ask to subscribe to chosen CAN event with a call to *subscribe* API verb with the CAN messages name as JSON argument. Example from a websocket session:

```bash
low-can subscribe { "event": "doors.driver.open" }
ON-REPLY 1:low-can/subscribe: {"jtype":"afb-reply","request":{"status":"success","uuid":"a18fd375-b6fa-4c0e-a1d4-9d3955975ae8"}}
```

Subscription and unsubscription can take wildcard in their *event* value. To reveive all doors events :

```bash
low-can subscribe { "event" : "doors*" }
ON-REPLY 1:low-can/subscribe: {"jtype":"afb-reply","request":{"status":"success","uuid":"511c872e-d7f3-4f3b-89c2-aa9a3e9fbbdb"}}
```

Then you will receive an event each time a CAN message is decoded for the event named *doors.driver.open*

```
ON-EVENT low-can/messages.doors.driver.open({"event":"low-can\/messages.doors.driver.open","data":{"name":"messages.doors.driver.open","value":true},"jtype":"afb-event"})
```

Notice that event shows you that the CAN event is named *messages.doors.driver.open* but you ask for event about *doors.driver.open*. This is because all CAN messages or diagnostic messages are prefixed by the JSON parent node name, **messages** for CAN messages and **diagnostic_messages** for diagnostic messages like OBD2. This will let you subscribe or unsubcribe to all signals at once, not recommended, and better make filter on subscribe operation based upon their type. Examples:

```
low-can subscribe { "event" : "*speed*" } --> will subscribe to all messages with speed in their name. Search will be make without prefix for it.
low-can subscribe { "event" : "speed*" } --> will subscribe to all messages begin by speed in their name. Search will be make without prefix for it.
low-can subscribe { "event" : "messages*speed*" } --> will subscribe to all CAN messages with speed in their name. Search will be on prefixed messages here.
low-can subscribe { "event" : "messages*speed" } --> will subscribe to all CAN messages ending with speed in their name. Search will be on prefixed messages here.
low-can subscribe { "event" : "diagnostic*speed*" } --> will subscribe to all diagnostic messages with speed in their name. Search will be on prefixed messages here.
low-can subscribe { "event" : "diagnostic*speed" } --> will subscribe to all diagnostic messages ending with speed in their name. Search will be on prefixed messages here.
```

You can stop receiving event from it by unsubscribe the signal the same way you did for subscribe

```bash
low-can unsubscribe { "event": "doors.driver.open" }
ON-REPLY 2:low-can/unsubscribe: {"jtype":"afb-reply","request":{"status":"success"}}
low-can unsubscribe { "event" : "doors*" }
ON-REPLY 3:low-can/unsubscribe: {"jtype":"afb-reply","request":{"status":"success"}}
```

[USB_CAN]: http://reference.com/ "USB CAN adapter recommended"
[OBD2_cable]: http://foo.bar/ "OBD2<->DB9 recommended cable"
[SDK_instructions]: http://docs.iot.bzh/docs/getting_started/en/dev/reference/setup-sdk-environment.html "Setup SDK environment"
[generator]: http://github.com/iotbzh/can-config-generator "AGL low level CAN binding Generator"
[afm-util]: http://docs.iot.bzh/docs/apis_services/en/dev/reference/af-main/afm-daemons.html#using-afm-util "afm-util usage"