/*
 * Copyright (C) 2015, 2016 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 *  A representation of an OBD-II PID.
 *
 * pid - The 1 byte PID.
 * name - A human readable name to use for this PID when published.
 * frequency - The frequency to request this PID if supported by the vehicle
 *      when automatic, recurring OBD-II requests are enabled.
 */

enum UNIT {
	POURCENT,
	DEGREES_CELSIUS,
	KPA,
	RPM,
	GRAMS_SEC,
	SECONDS,
	KM,
	KM_H,
	PA,
	NM
};

const char *UNIT_NAMES[10] = {
	"POURCENT",
	"DEGREES_CELSIUS",
	"KPA",
	"RPM",
	"GRAMS_SEC",
	"SECONDS",
	"KM",
	"KM_H",
	"PA",
	"NM"
};

typedef struct _Obd2Pid {
    uint8_t pid;
    const char* name;
    const int min;
    const int max;
    enum UNIT unit;
    int frequency;
} Obd2Pid;

/*
 * Pre-defined OBD-II PIDs to query for if supported by the vehicle.
 */
const Obd2Pid OBD2_PIDS[] = {
    { pid: 0x4, name: "engine.load", min:0, max: 100, unit: POURCENT, frequency: 5 },
    { pid: 0x5, name: "engine.coolant.temperature", min: -40, max: 215, unit: DEGREES_CELSIUS, frequency: 1 },
    { pid: 0xa, name: "fuel.pressure", min: 0, max: 765, unit: KPA, frequency: 1 },
    { pid: 0xb, name: "intake.manifold.pressure", min: 0, max: 255, unit: KPA, frequency: 1 },
    { pid: 0xc, name: "engine.speed", min: 0, max: 16383, unit: RPM, frequency: 5 },
    { pid: 0xd, name: "vehicle.speed", min: 0, max: 255, unit: KM_H, frequency: 5 },
    { pid: 0xf, name: "intake.air.temperature", min: -40, max:215, unit: DEGREES_CELSIUS, frequency: 1 },
    { pid: 0x10, name: "mass.airflow", min: 0, max: 655, unit: GRAMS_SEC, frequency: 5 },
    { pid: 0x11, name: "throttle.position", min: 0, max: 100, unit: POURCENT, frequency: 5 },
    { pid: 0x1f, name: "running.time", min: 0, max: 65535, unit: SECONDS, frequency: 1 },
    { pid: 0x2d, name: "EGR.error", min: -100, max: 99, unit: POURCENT, frequency: 0 },
    { pid: 0x2f, name: "fuel.level", min: 0, max: 100, unit: POURCENT, frequency: 1 },
    { pid: 0x33, name: "barometric.pressure", min: 0, max: 255, unit: KPA, frequency: 1 },
    { pid: 0x4c, name: "commanded.throttle.position", min: 0, max: 100, unit: POURCENT, frequency: 1 },
    { pid: 0x52, name: "ethanol.fuel.percentage", min: 0, max: 100, unit: POURCENT, frequency: 1 },
    { pid: 0x5a, name: "accelerator.pedal.position", min: 0, max: 100, unit: POURCENT, frequency: 5 },
    { pid: 0x5b, name: "hybrid.battery-pack.remaining.life", min: 0, max: 100, unit: POURCENT, frequency: 5 },
    { pid: 0x5c, name: "engine.oil.temperature",min: -40, max: 210, unit: DEGREES_CELSIUS, frequency: 1 },
    { pid: 0x63, name: "engine.torque", min: 0, max: 65535, unit: NM, frequency: 1 },
};

/*
    { pid: 0x4, name: "engine.load", frequency: 5 },
    { pid: 0x5, name: "engine.coolant.temperature", frequency: 1 },
    { pid: 0xa, name: "fuel.pressure", frequency: 1 },
    { pid: 0xb, name: "intake.manifold.pressure", frequency: 1 },
    { pid: 0xc, name: "engine.speed", frequency: 5 },
    { pid: 0xd, name: "vehicle.speed", frequency: 5 },
    { pid: 0xf, name: "intake.air.temperature", frequency: 1 },
    { pid: 0x10, name: "mass.airflow", frequency: 5 },
    { pid: 0x11, name: "throttle.position", frequency: 5 },
    { pid: 0x1f, name: "running.time", frequency: 1 },
    { pid: 0x27, name: "fuel.level", frequency: 1 },
    { pid: 0x33, name: "barometric.pressure", frequency: 1 },
    { pid: 0x4c, name: "commanded.throttle.position", frequency: 1 },
    { pid: 0x52, name: "ethanol.fuel.percentage", frequency: 1 },
    { pid: 0x5a, name: "accelerator.pedal.position", frequency: 5 },
    { pid: 0x5c, name: "engine.oil.temperature", frequency: 1 },
    { pid: 0x63, name: "engine.torque", frequency: 1 },
};
 */