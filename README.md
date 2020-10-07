# Redpesk CAN Low level binding's plugins

You will find more advanced plugins in the repository using the CAN low level
binding. This concern mainly ARS408 maritim Radar or handle the NMEA2000
protocol for example.

## How to build

### Pre-requisites

To build these plugins you need the following development packages installed on
your system:

- rp-app-framework-binder
- rp-can-low-level
- rp-libappcontroller
- rp-libafb-helpers

And the build tools:

- cmake
- gcc or clang
- systemd-devel
- oniguruma
- json-c
- jq

#### From the sources

Get the Redpesk Application Framework Binder and install it:

```bash
$ git clone https://github.com/redpesk-core/app-framework-binder.git
Cloning into 'app-framework-binder'...
remote: Enumerating objects: 10708, done.
remote: Counting objects: 100% (10708/10708), done.
remote: Compressing objects: 100% (2908/2908), done.
remote: Total 10708 (delta 8036), reused 10389 (delta 7717), pack-reused 0
Receiving objects: 100% (10708/10708), 4.25 MiB | 681.00 KiB/s, done.
Resolving deltas: 100% (8036/8036), done.
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=DEBUG -DUSE_SIMULATION=YES ..
-- The C compiler identification is GNU 10.2.1
-- The CXX compiler identification is GNU 10.2.1
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc - works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ - works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Found PkgConfig: /usr/bin/pkg-config (found version "1.6.3")
-- Looking for pthread.h
-- Looking for pthread.h - found
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Failed
-- Looking for pthread_create in pthreads
-- Looking for pthread_create in pthreads - not found
-- Looking for pthread_create in pthread
-- Looking for pthread_create in pthread - found
-- Found Threads: TRUE
-- Checking for module 'json-c'
--   Found json-c, version 0.13.1
-- Looking for include file magic.h
-- Looking for include file magic.h - found
-- Looking for magic_load in magic
-- Looking for magic_load in magic - found
-- Checking for module 'libsystemd>=222'
--   Found libsystemd, version 245
-- Checking for module 'libmicrohttpd>=0.9.60'
--   Found libmicrohttpd, version 0.9.71
-- Checking for module 'openssl'
--   Found openssl, version 1.1.1g
-- Checking for module 'uuid'
--   Found uuid, version 2.35.2
-- Checking for module 'cynara-client'
--   Package 'cynara-client', required by 'virtual:world', not found
-- Checking for module 'check'
--   Found check, version 0.14.0
-- Configuring done
-- Generating done
-- Build files have been written to: /home/claneys/Workspace/Sources/IOTbzh/gitlab/redpesk-core/app-framework-binder/build
$ make -j
Scanning dependencies of target afb-json2c
Scanning dependencies of target afb-genskel
Scanning dependencies of target afb-dbus-binding
Scanning dependencies of target hello3
Scanning dependencies of target hello2
Scanning dependencies of target demoContext
Scanning dependencies of target afb-lib
Scanning dependencies of target tic-tac-toe
Scanning dependencies of target afb-exprefs
Scanning dependencies of target afbwsc
Scanning dependencies of target authLogin
Scanning dependencies of target demoPost
Scanning dependencies of target tuto-app1
[  1%] Building C object src/devtools/CMakeFiles/afb-genskel.dir/genskel.c.o
[  2%] Building C object src/devtools/CMakeFiles/afb-json2c.dir/json2c.c.o
[  3%] Building C object src/devtools/CMakeFiles/afb-exprefs.dir/exprefs.c.o
[  4%] Building C object bindings/intrinsics/CMakeFiles/afb-dbus-binding.dir/afb-dbus-binding.c.o
[  5%] Building C object bindings/samples/CMakeFiles/hello3.dir/hello3.c.o
[  6%] Building C object bindings/samples/CMakeFiles/hello2.dir/hello2.c.o
[  7%] Building C object src/CMakeFiles/afbwsc.dir/afb-ws.c.o
[  9%] Building C object bindings/samples/CMakeFiles/tic-tac-toe.dir/tic-tac-toe.c.o
[  9%] Building C object src/CMakeFiles/afbwsc.dir/afb-ws-client.c.o
[ 10%] Building C object bindings/samples/CMakeFiles/demoPost.dir/DemoPost.c.o
[...]
[ 94%] Linking C executable test-globset
[ 95%] Linking C executable test-session
[ 96%] Linking C executable test-apiv3
[ 96%] Built target test-globset
[ 97%] Linking C executable test-u16id
[ 97%] Built target test-u16id
[ 97%] Built target test-session
[ 98%] Linking C executable test-wrap-json
[ 98%] Built target test-apiv3
[ 99%] Linking C executable afb-daemon
[ 99%] Built target test-wrap-json
[100%] Linking C executable test-apiset
[100%] Built target test-apiset
[100%] Built target afb-daemon
$ sudo make install
```

Do the same for the lib afb-helpers and appcontroller:

```bash
git clone https://github.com/redpesk-common/libafb-helpers.git
git clone https://github.com/redpesk-common/libappcontroller.git
# install afb-helpers
cd libafb-helpers
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=DEBUG ..
make -j
sudo make install
# install appcontroller
cd ../libappcontroller
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=DEBUG ..
make -j
sudo make install
```

Finally, get the CAN low level binding and install it:

```bash
J1939_PRESENT=$([ "$(find /lib/modules/$(uname -r) -name "can-j1939.ko*")" ] && echo ON || echo OFF)
git clone https://github.com/redpesk-common/rp-can-low-level.git
cd rp-can-low-level
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=DEBUG -DWITH_FEATURE_J1939=${J1939_PRESENT} -DCMAKE_INSTALL_PREFIX=/usr/local ..
make -j
sudo make install
```

#### From the distribution packages

***INSTRUCTIONS WHEN THE PUBLICS OBS PROJECTS AND REPOS WILL BE CREATED***
***INSTRUCTIONS WHEN THE PUBLICS OBS PROJECTS AND REPOS WILL BE CREATED***
***INSTRUCTIONS WHEN THE PUBLICS OBS PROJECTS AND REPOS WILL BE CREATED***

Install the can-low-level binding from official packages repositories if you
want to test your **CAN low level**'s plugin natively on your host:

^- Fedora

```bash
export DISTRO="Fedora_32"
export REVISION=28
source /etc/os-release ; export DISTRO="${NAME}_${VERSION_ID}"
sudo wget -O /etc/yum.repos.d/redpesk_devel_${REVISION}.repo http://download.opensuse.org/repositories/IotBzh/redpesk_devel_${REVISION}/${DISTRO}/redpesk_devel_${REVISION}.repo
sudo dnf install rp-can-low-level
```

^- OpenSUSE

```bash
export DISTRO="openSUSE_Leap_15.2"
export REVISION=28
source /etc/os-release; export DISTRO=$(echo $PRETTY_NAME | sed "s/ /_/g")
sudo zypper ar http://download.opensuse.org/repositories/IotBzh/redpesk_devel_${REVISION}/${DISTRO}/redpesk_devel_${REVISION}.repo
sudo zypper --gpg-auto-import-keys ref
sudo zypper install rp-can-low-level
```

^- Ubuntu

```bash
export REVISION="28"
export DISTRO="xUbuntu_20.04"
wget -O - http://download.opensuse.org/repositories/IotBzh/redpesk_devel_${REVISION}/${DISTRO}/Release.key | sudo apt-key add -
sudo bash -c "cat >> /etc/apt/sources.list.d/redpesk.list <<EOF
deb http://download.opensuse.org/repositories/IotBzh/redpesk_devel_${REVISION}/${DISTRO}/ ./
EOF"
sudo apt-get update
sudo apt-get install rp-can-low-level
```

^- Debian

```bash
export REVISION="28"
export DISTRO="Debian_10"
wget -O - http://download.opensuse.org/repositories/IotBzh/redpesk_devel_${REVISION}/${DISTRO}/Release.key | sudo apt-key add -
sudo bash -c "cat >> /etc/apt/sources.list.d/redpesk.list <<EOF
deb http://download.opensuse.org/repositories/IotBzh/redpesk_devel_${REVISION}/${DISTRO}/ ./
EOF"
sudo apt-get update
sudo apt-get install rp-can-low-level
```

***INSTRUCTIONS WHEN THE PUBLICS OBS PROJECTS AND REPOS WILL BE CREATED***
***INSTRUCTIONS WHEN THE PUBLICS OBS PROJECTS AND REPOS WILL BE CREATED***
***INSTRUCTIONS WHEN THE PUBLICS OBS PROJECTS AND REPOS WILL BE CREATED***

The low-level-can-generator generates c++ plugins from json mapping files
for the rp-can-low-level binding.

### Install the generator <p>

To generate your plugins you will need the low-level-can-generator.

```bash
dnf install rp-low-level-can-generator
```

or from the sources:

```bash 
git clone https://github.com/redpesk-common/low-level-can-generator
cd low-level-can-generator
mkdir build
cd build
cmake ..
make
sudo make install
```

### Write your json mapping file <p>

In this json will be defined all the messages you will recieve from your
connected device.

The generator uses OpenXC.[Here is the original documentation.](https://github.com/openxc/vi-firmware/blob/master/docs/config/reference.rst)

#### General Options <p>

* *`name`* <p> Set plugin's name. It will be your plugin's id in the binding.
It has to be unique.

* *`version`* <p>Set plugin's version. It will define which version of the
decoders will be used. Available versions are v1 and v2, the latest being v2.

* *`extra_sources`* <p> Set sources to be added to the top of the generated
plugin. It can be your own c++ signal decoders for example.

#### Message<p>

The *`messages`* key is a object with fields mapping from CAN message IDs 
to signal definitions. The fields must be hex IDs of CAN messages
as strings (e.g. 0x90).

* *`name`* <p> The name of the CAN message. Can be used to easily find a 
certain message among the others.

* *`bus`* <p> The name of the CAN bus where the messages can be found.

* *`comment`* <p> To give a piece of information about the message. Useful 
when reading the code.

* *`length`* <p> Message's length in bytes.

* *`is_fd`* <p> Either True or False. Determines if your device uses CAN FD
or not.

* *`is_extended`* <p> Either True or False. Determines if the CAN ID is 11 bits
or 29 bits long.

* *`is_j1939`* <p> Either True or False. Determines if you devices uses the
j1939 protocol.

* *`byte_frame_is_big_endian`* <p> Either True or False.

* *`bit_position_reversed`* <p> Either True or False.

* *`signals`* <p> A list of CAN signal objects (described in the 
[signal](#signal) section) that are in this message, with the name of the
signal as the key.

* *`max_frequency`* <p>
If sending raw CAN messages to the output interfaces, this controls the maximum
frequency (in Hz) that the message will be process and let through. The default
value (0) means that all messages will be processed, and there is no limit
imposed by the firmware. If you want to make sure you don't miss a change in
value even when rate limiting, see the force_send_changed attribute. Defaults
to 0 (no limit).

* *`max_signal_frequency`* <p>
Setting the max signal frequency at the message level will cascade down to all
of the signals within the message (unless overridden). The default value (0)
means that all signals will be processed, and there is no limit imposed by the
firmware. See the max_frequency flag documentation for the signal mapping for
more information. If you want to make sure you don't miss a change in value
even when rate limiting, see the force_send_changed_signals attribute. Defaults
to 0 (no limit).

* *`force_send_changed`* <p>
Meant to be used in conjunction with max_frequency, if this is true a raw CAN
message will be sent regardless of the given frequency if the value has changed
(when using raw CAN passthrough). Defaults to true.

* *`force_send_changed_signals`* <p>
Setting this value on a message will cascade down to all of the signals within
the message (unless overridden). See the force_send_changed flag documentation
for the signal mapping for more information. Defaults to false.

#### Signal <a name="#signal"></a> <p>

The attributes of a signal object within a message are:

* *`generic_name`* <p>
The name of the associated generic signal name (from the OpenXC specification)
that this should be translated to. Optional - if not specified, the signal is
read and stored in memory, but not sent to the output bus. This is handy for
combining the value of multiple signals into a composite measurement such as
steering wheel angle with its sign.

* *`bit_position`* <p>
The starting bit position of this signal within the message.

* *`bit_size`* <p>
The width in bits of the signal.

* *`factor (optional)`* <p>
The signal value is multiplied by this if set.

* *`offset (optional)`* <p>
This is added to the signal value if set.

* *`min_value (optional)`* <p>
The minimum value for the processed signal.

* *`max_value (optional)`* <p>
The maximum value for the processed signal.

* *`send_same (optional)`* <p>
If true, will re-send even if the value hasn't changed.

* *`force_send_changed (optional)`* <p>
If true, regardless of the frequency, it will send the value if it has changed.

* *`sign (optional)`* <p>
If the data is signed it indicates the encode.

* *`bit_sign_position (optional)`* <p>
The bit that indicates the sign of the signal in its CAN message.

* *`unit (optional)`* <p>
The unit of the data.

* *`decoder (optional)`* <p>
The name of a function that will be compiled with the firmware and should be
applied to the signal's value after the normal translation. See the
[Signal Decoder](#SignalDecoder) section for details.

* *`ignore (optional)`* <p>
Setting this to true on a signal will silence output of the signal. The VI
will not monitor the signal nor store any of its values. This is useful if
you are using a custom decoder for an entire message, want to silence the
normal output of the signals it handles. If you need to use the previously
stored values of any of the signals, you can use the ignoreDecoder as the
decoder for the signal. Defaults to false.

* *`enabled (optional)`* <p>
Enable or disable all processing of a CAN signal. By default, a signal is
enabled; if this flag is false, the signal will be left out of the generated
source code. Defaults to true.

>The difference between ignore, enabled and using an ignoreDecoder can be
confusing. To summarize the difference:
> * The enabled flag is the master control switch for a signal - when this
is false, the signal (or message, or mapping) will not be included in the
firmware at all. A common time to use this is if you want to have one
configuration file with many options, only a few of which are enabled in
any particular build.
> * The ignore flag will not exclude a signal from the firmware, but it will
not include it in the normal message processing pipeline. The most common use
case is when you need to reference the bit field information for the signal
from a custom decoder.
> * Finally, use the ignoreDecoder for your signal's decoder to both include
it in the firmware and handle it during the normal message processing pipeline,
but just silence its output. This is useful if you need to track the last known
value for this signal for a calculation in a custom decoder.

* *`states`* <p>
This is a mapping between the desired descriptive states (e.g. off) and the
corresponding numerical values from the CAN message (usually an integer). The
raw values are specified as a list to accommodate multiple raw states being
coalesced into a single final state (e.g. key off and key removed both mapping
to just "off").

* *`max_frequency (optional)`* <p>
Some CAN signals are sent at a very high frequency, likely more often than will
ever be useful to an application. This attribute sets the maximum frequency (Hz)
that the signal will be processed and let through. The default value (0) means
that all values will be processed, and there is no limit imposed by the
firmware. If you want to make sure you don't miss a change in value even when
dropping messages, see the force_send_changed attribute. You probably don't
want to combine this attribute with send_same or else you risk missing a status
change message.
Defauls to 0 (no limit).

* *`send_same (optional)`* <p>
By default, all signals are translated every time they are received from the
CAN bus. By setting this to false, you can force a signal to be sent only if
the value has actually changed. This works best with boolean and state based
signals. Defaults to true.

* *`force_send_changed (optional)`* <p>
Meant to be used in conjunction with max_frequency, if this is true a signal
will be sent regardless of the given frequency if the value has changed. This
is useful for state-based and boolean states, where the state change is the
most important thing and you don't want that message to be dropped. Defaults
to false.

* *`writable (optional)`* <p>
Set this attribute to true to allow this signal to be written back to the CAN
bus by an application. By default, the value will be interpreted as a floating
point number. Defaults to false.

* *`encoder (optional)`* <p>
You can specify a custom function here to encode the value for a CAN messages.

#### Diagnostic Messages

The *`diagnostic_messages`* key is an array of objects describing a recurring diagnostic message request.

The attributes of each diagnostic message object are:

* *`bus`* <p>
The name of one of the previously defined CAN buses where this message should be requested.
* *`id`* <p>
the arbitration ID for the request.
* *`mode`* <p>
The diagnostic request mode, e.g. Mode 1 for powertrain diagnostic requests.
* *`frequency`* <p>
The frequency in Hz to request this diagnostic message. The maximum allowed frequency is 10Hz.
* *`pid (optional)`* <p>
If the mode uses PIDs, the pid to request.
* *`name (optional)`* <p>
A human readable, string name for this request. If provided, the response will have a name field (much like a normal translated message) with this value in place of bus, id, mode and pid.
* *`decoder (optional)`* <p>
When using a name, you can also specify a custom decoder function to parse the payload. This field is the name of a function (that matches the DiagnosticResponseDecoder function prototype). When a decoder is specified, the decoded value will be returned in the value field in place of payload.
* *`callback (optional)`* <p>
This field is the name of a function (that matches the DiagnosticResponseCallback function prototype) that should be called every time a response is received to this request.

#### Signal Decoder <a name="#SignalDecoder"></a><p>

The default decoder for each signal is a simple passthrough, translating the
signal's value from engineering units to something more usable (using the
defined factor and offset). Some signals require additional processing that you
may wish to do within the binding and not on the host device. Other signals may need
to be combined to make a composite signal that's more meaningful to developers.

There is however a list of ready to use decoders provided by the low-can binding: 

* decode_state
* decode_booleanl
* decode_ignore
* decode_noop
* decode_bytes
* decode_ascii
* decode_date
* decode_time
* decode_signal
* decode_obd2_response

You can also define your own decoder. To do so, create a **signal-header.cpp**
file that will be added to the top of the generated c++ plugin file by adding
its filename to the *`extra_sources`* field. Then write you own decoder 
in this file. Here is an example:

```c++
openxc_DynamicField decoder_t::decode_date(signal_t& signal, std::shared_ptr<message_t> message, bool* send)
{
	float value = decoder_t::parse_signal_bitfield(signal, message);
	AFB_DEBUG("Decoded message from parse_signal_bitfield: %f", value);
	openxc_DynamicField decoded_value = build_DynamicField(value);

	// Don't send if they is no changes
	if ((signal.get_last_value() == value && !signal.get_send_same()) || !*send )
		*send = false;
	signal.set_last_value(value);

	return decoded_value;
}
```

#### Json Example <p>

```json
{	"name": "NMEA2000",
	"version" : "2.0",
	"extra_sources": [],
	"messages": {
		"60160": {
			"name": "Iso.Transport.Protocol.Data.Transfer",
			"bus":"j1939",
			"comment":"ISO Transport Protocol, Data Transfer",
			"length": 8,
			"is_fd": false,
			"is_extended": false,
			"is_j1939": true,
			"byte_frame_is_big_endian": true,
			"bit_position_reversed": true,
			"signals": {
				"Sid": {
					"bit_position": 0,
					"bit_size": 8,
					"sign": 0,
					"generic_name": "Iso.Transport.Protocol.Data.Transfer.Sid"
				},
				"Data": {
					"bit_position": 8,
					"bit_size": 56,
					"decoder": "decoder_t::decode_bytes",
					"sign": 0,
					"generic_name": "Iso.Transport.Protocol.Data.Transfer.Data"
				}
			}
		},
		"60160#1": {
			"name": "60160",
			"bus":"j1939",
			"comment":"Abstract signals for 60160",
			"length": 8,
			"is_fd": false,
			"is_extended": false,
			"is_j1939": true,
			"byte_frame_is_big_endian": true,
			"bit_position_reversed": true,
			"signals": {
				"signal.60160": {
					"bit_position": 0,
					"bit_size": 64,
					"comment": "One signals",
					"decoder": "decoder_t::decode_60160",
					"sign": 0,
					"generic_name": "signal.60160"
				}
			}
		}
	}
}
```

### Generate your plugin 

To generate your plugin you'll have to use the low-level-can-generator.
Once you have installed the generator and wrote your json mapping file, execute
this command: 

 ```bash
can-config-generator -m you-mapping-file.json -o your-plugin-name.cpp
 ```

### Add your plugin to the "builder"

To build your plugin you'll have to use the plugin installer provided by the
redpesk-can-low-level-plugins project. 

To do so add your plugin to 

### Build

This project does not build anything by default. You have to select the plugin
you want to build:

```bash
$ mkdir build
$ cd build
$ cmake ..
-- The C compiler identification is GNU 10.2.1
-- The CXX compiler identification is GNU 10.2.1
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc - works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ - works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
=== Including plugin 'ARS408_signals'
=== Including plugin 'can-ethernet'
=== Including plugin 'gps-signals'
=== Including plugin 'j1939-signals'
=== Including plugin 'joystick-signals'
=== Including plugin 'simrad-draft-signals'
=== Including plugin 'n2k-basic-signals'
-- Configuring done
-- Configuring done
-- Generating done
-- Build files have been written to: /home/claneys/Workspace/Sources/IOTbzh/github/redpesk/redpesk-can-low-level-plugins/build
$ make gps-signals
Scanning dependencies of target gps-signals
[ 50%] Building CXX object src/gps/CMakeFiles/gps-signals.dir/gps-signals.cpp.o
[100%] Linking CXX shared module gps-signals.ctlso
[100%] Built target gps-signals
```

## How to use


### Adapt the CAN low level configuration file

Change the `control-rp-can-low-level.json` file by selecting the plugin(s) to
load. Here is an example selecting the `gps-signals` plugin:

```json
{
        "$schema": "",
        "metadata": {
                "uid": "Low Can",
                "version": "1.0",
                "api": "low-can",
                "info": "Low can Configuration"
        },
        "config": {
                "active_message_set": 0,
                "dev-mapping": {
                        "hs": "can0"
                },
                "diagnostic_bus": "hs"
        },
        "plugins": [
                {
                        "uid": "gps-signals",
                        "info": "gps signals",
                        "libs": "gps-signals.ctlso"
                }
        ]
}
```

You also have to change the CAN *device* if you do not use the *can0* default
one. Also, make it correspond to your needs against the *json* configuration you
used. Here for the gps-signals json definitions, we only need the **hs** bus.

### Setup CAN bus

#### Virtual CAN device

Connected to the target, here is how to load the virtual CAN device driver and set up a new vcan device :

```bash
modprobe vcan
ip link add vcan0 type vcan
ip link set vcan0 up
```
You can also call your linux CAN device as you like, for example if you need to name it can0 :

```bash
modprobe vcan
ip link add can0 type vcan
ip link set can0 up
```

#### CAN device using the USB CAN adapter

Use the real connection to CAN bus of your device using an USB CAN adapter.

Once connected, launch dmesg command and search which device to use:

```bash
$ dmesg
[...]
[  131.871441] usb 1-3: new full-speed USB device number 4 using ohci-pci
[  161.860504] can: controller area network core (rev 20120528 abi 9)
[  161.860522] NET: Registered protocol family 29
[  177.561620] usb 1-3: USB disconnect, device number 4
[  191.061423] usb 1-2: USB disconnect, device number 3
[  196.095325] usb 1-2: new full-speed USB device number 5 using ohci-pci
[  327.568882] usb 1-2: USB disconnect, device number 5
[  428.594177] CAN device driver interface
[ 1872.551543] usb 1-2: new full-speed USB device number 6 using ohci-pci
[ 1872.809302] usb_8dev 1-2:1.0 can0: firmware: 1.7, hardware: 1.0
[ 1872.809356] usbcore: registered new interface driver usb_8dev
```

Here device is named `can0`.

This instruction assuming a speed of 500000kbps for your CAN bus, you can try
others supported bitrate like 125000, 250000 if 500000 doesn’t work:

```bash
$ ip link set can0 type can bitrate 500000
$ ip link set can0 up
$ ip link show can0
  can0: <NOARP, UP, LOWER_UP, ECHO> mtu 16 qdisc pfifo_fast state UNKNOWN qlen 10
    link/can
    can state ERROR-ACTIVE (berr-counter tx 0 rx 0) restart-ms 0
    bitrate 500000 sample-point 0.875
    tq 125 prop-seg 6 phase-seg1 7 phase-seg2 2 sjw 1
    sja1000: tseg1 1..16 tseg2 1..8 sjw 1..4 brp 1..64 brp-inc 1
    clock 16000000
```

On a Rcar Gen3 board for example, you’ll have your CAN device as can1 because `can0` already
exists as an embedded device.

The instructions will be the same:

```bash
$ ip link set can1 type can bitrate 500000
$ ip link set can1 up
$ ip link show can1
  can0: <NOARP, UP, LOWER_UP, ECHO> mtu 16 qdisc pfifo_fast state UNKNOWN qlen 10
    link/can
    can state ERROR-ACTIVE (berr-counter tx 0 rx 0) restart-ms 0
    bitrate 500000 sample-point 0.875
    tq 125 prop-seg 6 phase-seg1 7 phase-seg2 2 sjw 1
    sja1000: tseg1 1..16 tseg2 1..8 sjw 1..4 brp 1..64 brp-inc 1
    clock 1600000
```

#### Rename an existing CAN device

You can rename an existing CAN device using following command and thus move
an existing `can0` device to anything else. You will then be able to use 
another device as `can0`. For example, using a Rcar Gen3 board,
do the following :

```bash
sudo ip link set can0 down
sudo ip link set can0 name bsp-can0
sudo ip link set bsp-can0 up
```

Then connect your USB CAN device which will be named `can0` by default.

### Install and launch the binding

Now, install the configuration and plugin(s) to be used by the binding:

```bash
$ cd build
$ sudo make install
Install the project...
-- Install configuration: ""
-- Up-to-date: /usr/local/rp-can-low-level/etc/control-rp-can-low-level.json
$ make gps-signal
Scanning dependencies of target gps-signals
[ 50%] Building CXX object src/gps/CMakeFiles/gps-signals.dir/gps-signals.cpp.o
[100%] Linking CXX shared module gps-signals.ctlso
[100%] Built target gps-signals
$ sudo make install_gps-signal
Scanning dependencies of target install_gps-signals
[100%] Built target install_gps-signals
```

Launch it using the `afb-daemon` (look out at the gps-signals.ctlso plugin load):

```bash
$ afb-daemon --name=afbd-rp-low-can-level --port=1234 --roothttp=. --tracereq=common --token= --workdir=/usr/local/rp-can-low-level --binding=lib/afb-low-can-binding.so --ws-server=unix:/tmp/low-can -vvv
------BEGIN OF CONFIG-----
-- {
--    "name": "afbd-rp-low-can-level",
--    "port": 1234,
--    "roothttp": ".",
--    "tracereq": "common",
--    "token": "",
--    "workdir": "/usr/local/rp-can-low-level",
--    "binding": [
--      "lib/afb-low-can-binding.so"
--    ],
--    "ws-server": [
--      "unix:/tmp/low-can"
--    ],
--    "apitimeout": 20,
--    "cache-eol": 100000,
--    "cntxtimeout": 32000000,
--    "session-max": 200,
--    "rootdir": ".",
--    "uploaddir": ".",
--    "rootbase": "/opa",
--    "rootapi": "/api",
--    "ldpaths": [
--      "/usr/local/lib64/afb"
--    ]
--  }
------END OF CONFIG-----
INFO: running with pid 34440
INFO: API monitor added
INFO: binding [lib/afb-low-can-binding.so] looks like an AFB binding V3
INFO: API low-can added
WARNING: [API low-can] CTL-INIT JSON file found but not used : /usr/local/rp-can-low-level/etc/control-rp-can-low-level.json [/home/claneys/Workspace/Sources/IOTbzh/gitlab/redpesk-common/libappcontroller/ctl-lib/ctl-config.c:93,ConfigSearch]
WARNING: [API low-can] CTL-INIT JSON file found but not used : /home/claneys/Workspace/Sources/IOTbzh/gitlab/redpesk-common/rp-can-low-level/build/package/etc/control-rp-can-low-level.json [/home/claneys/Workspace/Sources/IOTbzh/gitlab/redpesk-common/libappcontroller/ctl-lib/ctl-config.c:93,ConfigSearch]
INFO: [API low-can] CTL-LOAD-CONFIG: loading config filepath=/usr/local/rp-can-low-level/etc/control-rp-can-low-level.json
DEBUG: [API low-can] Config { "active_message_set": 0, "dev-mapping": { "hs": "can0" }, "diagnostic_bus": "hs" }
DEBUG: [API low-can] BCM socket ifr_name is : can0
DEBUG: [API low-can] Shims initialized
DEBUG: [API low-can] Clearing existing diagnostic requests
DEBUG: [API low-can] Diagnostic Manager initialized
DEBUG: [API low-can] Plugin search path : '/usr/local/rp-can-low-level:/usr/local/rp-can-low-level/lib/..'
NOTICE: [API low-can] CTL-PLUGIN-LOADONE gps successfully registered
WARNING: [API low-can] Plugin multiple instances in searchpath will use /usr/local/rp-can-low-level/lib/plugins/gps-signals.ctlso [/home/claneys/Workspace/Sources/IOTbzh/gitlab/redpesk-common/libappcontroller/ctl-lib/ctl-plugin.c:247,LoadFoundPlugins]
INFO: Scanning dir=[/usr/local/lib64/afb] for bindings
INFO: binding [/usr/local/lib64/afb/afb-dbus-binding.so] isn't an AFB binding
DEBUG: Init config done
NOTICE: API low-can starting...
DEBUG: [API low-can] Found 0 signal(s)
INFO: API low-can started
NOTICE: API monitor starting...
INFO: API monitor started
WARNING: unable to set the upload directory . [/home/claneys/Workspace/Sources/IOTbzh/github/redpesk-core/app-framework-binder/src/main-afb-daemon.c:373,start_http_server]
NOTICE: Serving rootdir=. uploaddir=/tmp
NOTICE: Listening interface *:1234
NOTICE: Browser URL= http://localhost:1234
```

### Simple usage examples

Plug your GPS to your hardware CAN bus or use `canplayer` to replay a set of CAN
messages to send, ie:

```bash
canplayer -I gps-record
```

Then connect to your binding using the CLI utility provided with the `binder`
and call directly API's verbs, ie:

```bash
$ afb-client-demo -H -d unix:/tmp/low-can
list
ON-REPLY 1:list: success
[
  "messages.gps.simulateur.bearing",
  "messages.gps.simulateur.latitude",
  "messages.gps.simulateur.longitude",
  "messages.gps.simulateur.speed",
  "messages.gps.attack.bearing",
  "messages.gps.attack.latitude",
  "messages.gps.attack.longitude",
  "messages.gps.attack.speed",
  "messages.gps.accelero.bearing",
  "messages.gps.accelero.latitude",
  "messages.gps.accelero.longitude",
  "messages.gps.accelero.speed"
]
subscribe {"event": "gps*"}
ON-EVENT-CREATE: [1:low-can/messages.gps.simulateur.bearing]
ON-EVENT-SUBSCRIBE 3:subscribe: [1]
ON-EVENT-CREATE: [2:low-can/messages.gps.simulateur.latitude]
ON-EVENT-SUBSCRIBE 3:subscribe: [2]
ON-EVENT-CREATE: [3:low-can/messages.gps.simulateur.longitude]
ON-EVENT-SUBSCRIBE 3:subscribe: [3]
ON-EVENT-CREATE: [4:low-can/messages.gps.simulateur.speed]
ON-EVENT-SUBSCRIBE 3:subscribe: [4]
ON-EVENT-CREATE: [5:low-can/messages.gps.attack.bearing]
ON-EVENT-SUBSCRIBE 3:subscribe: [5]
ON-EVENT-CREATE: [6:low-can/messages.gps.attack.latitude]
ON-EVENT-SUBSCRIBE 3:subscribe: [6]
ON-EVENT-CREATE: [7:low-can/messages.gps.attack.longitude]
ON-EVENT-SUBSCRIBE 3:subscribe: [7]
ON-EVENT-CREATE: [8:low-can/messages.gps.attack.speed]
ON-EVENT-SUBSCRIBE 3:subscribe: [8]
ON-EVENT-CREATE: [9:low-can/messages.gps.accelero.bearing]
ON-EVENT-SUBSCRIBE 3:subscribe: [9]
ON-EVENT-CREATE: [10:low-can/messages.gps.accelero.latitude]
ON-EVENT-SUBSCRIBE 3:subscribe: [10]
ON-EVENT-CREATE: [11:low-can/messages.gps.accelero.longitude]
ON-EVENT-SUBSCRIBE 3:subscribe: [11]
ON-EVENT-CREATE: [12:low-can/messages.gps.accelero.speed]
ON-EVENT-SUBSCRIBE 3:subscribe: [12]
ON-REPLY 3:subscribe: success
null
ON-EVENT-PUSH: [8]
{
  "id":292,
  "name":"messages.gps.attack.speed",
  "value":0.0,
  "timestamp":1601302590185122
}
ON-EVENT-PUSH: [5]
{
  "id":292,
  "name":"messages.gps.attack.bearing",
  "value":0.0,
  "timestamp":1601302590185226
}
ON-EVENT-PUSH: [6]
{
  "id":292,
  "name":"messages.gps.attack.latitude",
  "value":329.42880249023438,
  "timestamp":1601302590185337
}
ON-EVENT-PUSH: [7]
{
  "id":292,
  "name":"messages.gps.attack.longitude",
  "value":239.43028259277344,
  "timestamp":1601302590185392
}
get { "event": "gps.attack.longitude" }
ON-REPLY 4:get: success
[
  {
    "event":"messages.gps.attack.longitude",
    "value":239.43028259277344
  }
]
```

Get more details from the binding complete [documentation](https://docs.iot.bzh/docs/en/master/apis_services/reference/signaling/5-Usage.html)
