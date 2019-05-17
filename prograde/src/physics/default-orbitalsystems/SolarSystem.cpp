/*
    Copyright (C) 2018 Florian Cabot <florian.cabot@hotmail.fr>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include "../../../include/physics/default-orbitalsystems/SolarSystem.hpp"

float sunMass   = 1.9891f * 1e30;
float sunRadius = 696342.f * 1000;

SolarSystem::SolarSystem()
    : OrbitalSystem(sunMass, sunRadius)
{
	createPlanets();
	createEarthSubSystem();
	createMarsSubSystem();
	createAsteroidBeltSubSystem();
	createJupiterSubSystem();
	createSaturnSubSystem();
	createUranusSubSystem();
	createNeptuneSubSystem();
	createPlutoSubSystem();
	createTransNeptunianSubSystem();
}

void SolarSystem::createPlanets()
{
	CelestialBody::Parameters physicalParams;

	// for siderealTimeAtEpoch see data/prograde/physics/siderealTimesAtEpoch.py

	// mercury
	physicalParams.radius                 = 2469.7 * km;
	physicalParams.color                  = Color(100, 100, 100);
	physicalParams.siderealTimeAtEpoch    = 6.069321130640849;
	physicalParams.siderealRotationPeriod = 5067014.4;
	physicalParams.northPoleRightAsc      = constant::pi * 281.01 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 61.45 / 180.f;
	createChild("Mercury", physicalParams);

	// venus
	physicalParams.radius              = 6051.8 * km;
	physicalParams.color               = Color(255, 255, 204);
	physicalParams.siderealTimeAtEpoch = 3.3587532198310948;
	// physicalParams.siderealRotationPeriod = 20997360.0;
	// ~4.4d ; we actually want the Atmosphere rotation period for visualization
	physicalParams.siderealRotationPeriod = 380234.594;
	physicalParams.northPoleRightAsc      = constant::pi * 92.76 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * -67.16 / 180.f;
	createChild("Venus", physicalParams);

	// earth
	physicalParams.radius                 = 6371.0 * km;
	physicalParams.color                  = Color(0, 204, 255);
	physicalParams.mass                   = 5.97219 * 1e24;
	physicalParams.atmosphere             = 0.3f;
	physicalParams.siderealTimeAtEpoch    = 1.7593363924107448;
	physicalParams.siderealRotationPeriod = 86164.0905;
	physicalParams.northPoleRightAsc      = constant::pi * 0.0 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 90.0 / 180.f;
	createChild("Earth", physicalParams);

	// mars
	physicalParams.radius                 = 3389.5 * km;
	physicalParams.color                  = Color(204, 102, 51);
	physicalParams.mass                   = 6.4171 * 1e23;
	physicalParams.atmosphere             = 0.075f;
	physicalParams.siderealTimeAtEpoch    = 0.7040061418814729;
	physicalParams.siderealRotationPeriod = 88642.6848;
	physicalParams.northPoleRightAsc      = constant::pi * 317.68143 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 52.88650 / 180.f;
	createChild("Mars", physicalParams);

	// jupiter
	physicalParams.radius     = 69911 * km;
	physicalParams.color      = Color(255, 204, 102);
	physicalParams.mass       = 1.89813 * 1e27;
	physicalParams.atmosphere = 0.f;
	// physicalParams.siderealTimeAtEpoch   = 4.667796671159006;
	// physicalParams.siderealRotationPeriod = 35730.0;
	// we actually want the Red Spot rotation period for visualization
	// extrapolated from https://www.calsky.com/cs.cgi?cha=7&sec=6&sub=1
	// predictions over a year
	// a lot of eyeballing also !
	physicalParams.siderealTimeAtEpoch    = constant::pi * 140.0 / 180.0;
	physicalParams.siderealRotationPeriod = 35744.20;
	physicalParams.northPoleRightAsc      = constant::pi * 268.057 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 64.495 / 180.f;
	createChild("Jupiter", physicalParams);

	// saturn
	physicalParams.radius                 = 58232 * km;
	physicalParams.color                  = Color(255, 204, 80);
	physicalParams.mass                   = 5.68319 * 1e26;
	physicalParams.innerRing              = 73788.72 * km;
	physicalParams.outerRing              = 139595.64 * km;
	physicalParams.siderealTimeAtEpoch    = 1.4399218441265502;
	physicalParams.siderealRotationPeriod = 37980.0;
	physicalParams.northPoleRightAsc      = constant::pi * 40.589 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 83.537 / 180.f;
	createChild("Saturn", physicalParams);

	// uranus
	physicalParams.radius                 = 25362 * km;
	physicalParams.color                  = Color(180, 180, 255);
	physicalParams.mass                   = 8.681 * 1e25;
	physicalParams.innerRing              = 73788.72 * km;
	physicalParams.outerRing              = 139595.64 * km;
	physicalParams.siderealTimeAtEpoch    = 0.22880273383759028;
	physicalParams.siderealRotationPeriod = 62063.712;
	physicalParams.northPoleRightAsc      = constant::pi * 77.311 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 15.175 / 180.f;
	createChild("Uranus", physicalParams);

	// neptune
	physicalParams.radius                 = 24622 * km;
	physicalParams.color                  = Color(80, 51, 255);
	physicalParams.mass                   = 1.0241 * 1e26;
	physicalParams.innerRing              = 0.0;
	physicalParams.outerRing              = 0.0;
	physicalParams.siderealTimeAtEpoch    = 0.29515412512381;
	physicalParams.siderealRotationPeriod = 58000.32;
	physicalParams.northPoleRightAsc      = constant::pi * 299.3 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 42.950 / 180.f;
	createChild("Neptune", physicalParams);

	// pluto
	physicalParams.radius                 = 1187 * km;
	physicalParams.color                  = Color(178, 145, 67);
	physicalParams.mass                   = 1.30900 * 1e22;
	physicalParams.siderealTimeAtEpoch    = 1.3928349482951936;
	physicalParams.siderealRotationPeriod = 551856.672;
	physicalParams.northPoleRightAsc      = constant::pi * 132.993 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * -6.163 / 180.f;
	createChild("Pluto", physicalParams);
}

void SolarSystem::createEarthSubSystem()
{
	Orbit::Parameters orbitalParams;
	(void) orbitalParams;
	CelestialBody::Parameters physicalParams;

	// moon
	orbitalParams.inclination             = constant::pi * 5.145 / 180.f;
	orbitalParams.ascendingNodeLongitude  = constant::pi * -11.3 / 180.f;
	orbitalParams.periapsisArgument       = constant::pi * 114.2 / 180.f;
	orbitalParams.eccentricity            = 0.0549;
	orbitalParams.semiMajorAxis           = 0.00257 * au;
	orbitalParams.meanAnomalyAtEpoch      = constant::pi * 358.6 / 180.f;
	physicalParams.radius                 = 1737.1 * km;
	physicalParams.color                  = Color(100, 100, 100);
	physicalParams.siderealTimeAtEpoch    = 0.6009311785663192;
	physicalParams.siderealRotationPeriod = 2360591.5104;
	physicalParams.northPoleRightAsc      = constant::pi * 266.86 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 65.64 / 180.f;
	createChild("Moon", physicalParams, "Earth");
}

void SolarSystem::createMarsSubSystem()
{
	Orbit::Parameters orbitalParams;
	(void) orbitalParams;
	CelestialBody::Parameters physicalParams;

	// phobos
	orbitalParams.inclination             = constant::pi * 26.04 / 180.f;
	orbitalParams.ascendingNodeLongitude  = constant::pi * 0 / 180.f;
	orbitalParams.periapsisArgument       = constant::pi * 0 / 180.f;
	orbitalParams.eccentricity            = 0.0151;
	orbitalParams.semiMajorAxis           = 0.00006267469 * au;
	orbitalParams.meanAnomalyAtEpoch      = constant::pi * 358.6 / 180.f;
	physicalParams.radius                 = 11.2667 * km;
	physicalParams.color                  = Color(100, 100, 100);
	physicalParams.siderealTimeAtEpoch    = 4.023685663761321;
	physicalParams.siderealRotationPeriod = 27553.843872;
	physicalParams.northPoleRightAsc      = constant::pi * 270.0 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 66.5 / 180.f;
	createChild("Phobos", physicalParams, "Mars");

	// deimos
	orbitalParams.inclination             = constant::pi * 27.58 / 180.f;
	orbitalParams.ascendingNodeLongitude  = constant::pi * 0 / 180.f;
	orbitalParams.periapsisArgument       = constant::pi * 0 / 180.f;
	orbitalParams.eccentricity            = 0.00033;
	orbitalParams.semiMajorAxis           = 0.0001568404676 * au;
	orbitalParams.meanAnomalyAtEpoch      = constant::pi * 358.6 / 180.f;
	physicalParams.radius                 = 6.2 * km;
	physicalParams.color                  = Color(170, 150, 150);
	physicalParams.siderealTimeAtEpoch    = 5.852159600397493;
	physicalParams.siderealRotationPeriod = 109123.2;
	physicalParams.northPoleRightAsc      = constant::pi * 270.0 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 66.5 / 180.f;
	createChild("Deimos", physicalParams, "Mars");
}

void SolarSystem::createAsteroidBeltSubSystem()
{
	CelestialBody::Parameters physicalParams;

	// ceres
	physicalParams.radius                 = 473.0 * km;
	physicalParams.color                  = Color(100, 100, 100);
	physicalParams.siderealTimeAtEpoch    = 3.422892798391968;
	physicalParams.siderealRotationPeriod = 32667.84;
	physicalParams.northPoleRightAsc      = constant::pi * 291.42744 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 66.764 / 180.f;
	createChild("Ceres", physicalParams);
}

void SolarSystem::createJupiterSubSystem()
{
	CelestialBody::Parameters physicalParams;

	// io
	physicalParams.radius                 = 1821.6 * km;
	physicalParams.color                  = Color(224, 213, 92);
	physicalParams.siderealTimeAtEpoch    = 1.7118221447865978;
	physicalParams.siderealRotationPeriod = 152853.50471;
	physicalParams.northPoleRightAsc      = constant::pi * 268.057 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 64.495 / 180.f;
	createChild("Io", physicalParams, "Jupiter");

	// europa
	physicalParams.radius                 = 1560.8 * km;
	physicalParams.color                  = Color(178, 145, 67);
	physicalParams.siderealTimeAtEpoch    = 6.008339408633722;
	physicalParams.siderealRotationPeriod = 306822.0384;
	physicalParams.northPoleRightAsc      = constant::pi * 268.057 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 64.495 / 180.f;
	createChild("Europa", physicalParams, "Jupiter");

	// ganymede
	physicalParams.radius                 = 2634.1 * km;
	physicalParams.color                  = Color(100, 100, 100);
	physicalParams.siderealTimeAtEpoch    = 0.3161974813725083;
	physicalParams.siderealRotationPeriod = 618153.375744;
	physicalParams.northPoleRightAsc      = constant::pi * 268.057 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 64.495 / 180.f;
	createChild("Ganymede", physicalParams, "Jupiter");

	// callisto
	physicalParams.radius                 = 2410.3 * km;
	physicalParams.color                  = Color(100, 100, 100);
	physicalParams.siderealTimeAtEpoch    = 4.322661995256511;
	physicalParams.siderealRotationPeriod = 1441931.18976;
	physicalParams.northPoleRightAsc      = constant::pi * 268.057 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 64.495 / 180.f;
	createChild("Callisto", physicalParams, "Jupiter");
}

void SolarSystem::createSaturnSubSystem()
{
	CelestialBody::Parameters physicalParams;

	// mimas
	physicalParams.radius                 = 198.2 * km;
	physicalParams.color                  = Color(100, 100, 100);
	physicalParams.siderealTimeAtEpoch    = 1.2155833485761178;
	physicalParams.siderealRotationPeriod = 81388.8;
	physicalParams.northPoleRightAsc      = constant::pi * 40.589 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 83.537 / 180.f;
	createChild("Mimas", physicalParams, "Saturn");

	// enceladus
	physicalParams.radius                 = 252.1 * km;
	physicalParams.color                  = Color(255, 255, 255);
	physicalParams.siderealTimeAtEpoch    = 0.13550134480931364;
	physicalParams.siderealRotationPeriod = 118386.8352;
	physicalParams.northPoleRightAsc      = constant::pi * 40.589 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 83.537 / 180.f;
	createChild("Enceladus", physicalParams, "Saturn");

	// tethys
	physicalParams.radius                 = 531.1 * km;
	physicalParams.color                  = Color(235, 235, 235);
	physicalParams.siderealTimeAtEpoch    = 0.8407241612918419;
	physicalParams.siderealRotationPeriod = 163106.0928;
	physicalParams.northPoleRightAsc      = constant::pi * 40.589 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 83.537 / 180.f;
	createChild("Tethys", physicalParams, "Saturn");

	// dione
	physicalParams.radius                 = 561.9 * km;
	physicalParams.color                  = Color(235, 235, 235);
	physicalParams.siderealTimeAtEpoch    = 1.1236765986768573;
	physicalParams.siderealRotationPeriod = 236469.456;
	physicalParams.northPoleRightAsc      = constant::pi * 40.589 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 83.537 / 180.f;
	createChild("Dione", physicalParams, "Saturn");

	// rhea
	physicalParams.radius                 = 763.8 * km;
	physicalParams.color                  = Color(235, 235, 235);
	physicalParams.siderealTimeAtEpoch    = 1.4800620020294997;
	physicalParams.siderealRotationPeriod = 390373.5168;
	physicalParams.northPoleRightAsc      = constant::pi * 40.589 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 83.537 / 180.f;
	createChild("Rhea", physicalParams, "Saturn");

	// titan
	physicalParams.radius                 = 2574.73 * km;
	physicalParams.color                  = Color(255, 204, 80);
	physicalParams.siderealTimeAtEpoch    = 5.251245365815998;
	physicalParams.siderealRotationPeriod = 1377648.0;
	physicalParams.northPoleRightAsc      = constant::pi * 40.589 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 83.537 / 180.f;
	createChild("Titan", physicalParams, "Saturn");

	// iapetus
	physicalParams.radius                 = 734.5 * km;
	physicalParams.color                  = Color(100, 100, 100);
	physicalParams.siderealTimeAtEpoch    = 0.6189284346518459;
	physicalParams.siderealRotationPeriod = 6853377.6;
	physicalParams.northPoleRightAsc      = constant::pi * 40.589 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 83.537 / 180.f;
	createChild("Iapetus", physicalParams, "Saturn");
}

void SolarSystem::createUranusSubSystem()
{
	CelestialBody::Parameters physicalParams;

	// miranda
	physicalParams.radius                 = 235.8 * km;
	physicalParams.color                  = Color(98, 85, 87);
	physicalParams.siderealTimeAtEpoch    = 3.4079336130672515;
	physicalParams.siderealRotationPeriod = 122124.5856;
	physicalParams.northPoleRightAsc      = constant::pi * 77.311 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 15.175 / 180.f;
	createChild("Miranda", physicalParams, "Uranus");

	// ariel
	physicalParams.radius                 = 578.9 * km;
	physicalParams.color                  = Color(210, 180, 140);
	physicalParams.siderealTimeAtEpoch    = 0.7353285244469319;
	physicalParams.siderealRotationPeriod = 217728.0;
	physicalParams.northPoleRightAsc      = constant::pi * 77.311 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 15.175 / 180.f;
	createChild("Ariel", physicalParams, "Uranus");

	// umbriel
	physicalParams.radius                 = 584.7 * km;
	physicalParams.color                  = Color(50, 50, 50);
	physicalParams.siderealTimeAtEpoch    = 3.313362003400425;
	physicalParams.siderealRotationPeriod = 358041.6;
	physicalParams.northPoleRightAsc      = constant::pi * 77.311 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 15.175 / 180.f;
	createChild("Umbriel", physicalParams, "Uranus");

	// titania
	physicalParams.radius                 = 788.4 * km;
	physicalParams.color                  = Color(210, 180, 140);
	physicalParams.siderealTimeAtEpoch    = 4.618360842157154;
	physicalParams.siderealRotationPeriod = 752218.6176;
	physicalParams.northPoleRightAsc      = constant::pi * 77.311 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 15.175 / 180.f;
	createChild("Titania", physicalParams, "Uranus");

	// oberon
	physicalParams.radius                 = 761.4 * km;
	physicalParams.color                  = Color(210, 180, 140);
	physicalParams.siderealTimeAtEpoch    = 5.862611035753226;
	physicalParams.siderealRotationPeriod = 1163223.4176;
	physicalParams.northPoleRightAsc      = constant::pi * 77.311 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 15.175 / 180.f;
	createChild("Oberon", physicalParams, "Uranus");
}

void SolarSystem::createNeptuneSubSystem()
{
	CelestialBody::Parameters physicalParams;

	// triton
	physicalParams.radius                 = 1353.4 * km;
	physicalParams.color                  = Color(98, 85, 87);
	physicalParams.siderealTimeAtEpoch    = 0.5654432608671653;
	physicalParams.siderealRotationPeriod = 507772.8;
	physicalParams.northPoleRightAsc      = constant::pi * 119.3 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * -42.950 / 180.f;
	createChild("Triton", physicalParams, "Neptune");
}

void SolarSystem::createPlutoSubSystem()
{
	CelestialBody::Parameters physicalParams;

	// charon
	physicalParams.radius                 = 606.0 * km;
	physicalParams.color                  = Color(100, 100, 100);
	physicalParams.siderealTimeAtEpoch    = 4.536447817866964;
	physicalParams.siderealRotationPeriod = 551856.70656;
	physicalParams.northPoleRightAsc      = constant::pi * 132.993 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * -6.163 / 180.f;
	createChild("Charon", physicalParams, "Pluto");
}

void SolarSystem::createTransNeptunianSubSystem()
{
	CelestialBody::Parameters physicalParams;

	// Actual orientations are unknown

	// eris
	physicalParams.radius                 = 1163.0 * km;
	physicalParams.color                  = Color(200, 200, 200);
	physicalParams.siderealTimeAtEpoch    = 0.0;
	physicalParams.siderealRotationPeriod = 93240.0;
	physicalParams.northPoleRightAsc      = constant::pi * 0.0 / 180.f;
	physicalParams.northPoleDeclination   = constant::pi * 90.0 / 180.f;
	createChild("Eris", physicalParams);

	// makemake
	physicalParams.radius = 725.0 * km;
	physicalParams.color  = Color(210, 180, 140);
	// physicalParams.siderealTimeAtEpoch    = ;
	physicalParams.siderealRotationPeriod = 82175.76;
	// physicalParams.northPoleRightAsc      = constant::pi * / 180.f;
	// physicalParams.northPoleDeclination   = constant::pi * / 180.f;
	createChild("Makemake", physicalParams);

	// haumea
	physicalParams.radius = 960.0 * km;
	physicalParams.oblateness.setXYZ(960.0 / 960.0, 770.0 / 960.0,
	                                 495.0 / 960.0);
	physicalParams.color     = Color(255, 200, 200);
	physicalParams.innerRing = 2252.0 * km;
	physicalParams.outerRing = 2322.0 * km;
	// physicalParams.siderealTimeAtEpoch    = ;
	physicalParams.siderealRotationPeriod = 14095.8144;
	// physicalParams.northPoleRightAsc      = constant::pi * / 180.f;
	// physicalParams.northPoleDeclination   = constant::pi * / 180.f;
	createChild("Haumea", physicalParams);

	physicalParams.oblateness.setXYZ(1.0, 1.0, 1.0);
	physicalParams.innerRing = 0.0;
	physicalParams.outerRing = 0.0;
}
