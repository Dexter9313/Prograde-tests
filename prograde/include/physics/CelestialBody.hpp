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

#ifndef CELESTIALBODY_HPP
#define CELESTIALBODY_HPP

#include <cmath>
#include <vector>

#include "CSVOrbit.hpp"
#include "Color.hpp"
#include "Orbit.hpp"

class CelestialBody
{
  public:
	enum class Type
	{
		GENERIC,
		TERRESTRIAL,
		GAZGIANT,
	};

	struct Parameters
	{
		Type type = Type::GENERIC;
		double mass;
		double radius;
		Vector3 oblateness = Vector3(1.0, 1.0, 1.0);
		Color color;
		float atmosphere = 0.f;
		float innerRing  = 0.f;
		float outerRing  = 0.f;

		/* ROTATION */
		float siderealTimeAtEpoch = 0.f; // angle between FP of Aries and Prime
		                                 // Meridian in radians
		float siderealRotationPeriod = FLT_MAX; // in seconds

		float northPoleRightAsc    = 0.f;                // in rad
		float northPoleDeclination = constant::pi / 2.0; // in rad
	};

	CelestialBody(double influentBodyMass, Orbit::Parameters orbitalParams,
	              Parameters physicalParams);
	CelestialBody(CelestialBody const& parent,
	              Orbit::Parameters const& orbitalParams,
	              Parameters physicalParams);
	CelestialBody(double influentBodyMass, std::string const& ownName,
	              Parameters physicalParams);
	CelestialBody(CelestialBody const* parent, std::string const& ownName,
	              Parameters physicalParams);
	CelestialBody(CelestialBody const& copiedBody) = default;
	CelestialBody const* getParent() const;
	CelestialBody* createChild(Orbit::Parameters const& orbitalParams,
	                           Parameters const& physicalParams);
	CelestialBody* createChild(std::string const& childName,
	                           Parameters const& physicalParams);
	Orbit const* getOrbit() const;
	Orbit* getOrbit();
	Parameters getParameters() const;
	Vector3 getRelativePositionAtUT(UniversalTime uT) const;
	Vector3 getAbsolutePositionAtUT(UniversalTime uT) const;
	Vector3 getAbsoluteVelocityAtUT(UniversalTime uT) const;
	CoordinateSystem getAttachedCoordinateSystemAtUT(UniversalTime uT) const;
	float getPrimeMeridianSiderealTimeAtUT(UniversalTime uT) const;
	virtual ~CelestialBody();

	// Will try to get more significant digits than the awful
	// to.absolute - from.absolute
	static Vector3 getRelativePositionAtUt(CelestialBody const* from,
	                                       CelestialBody const* to,
	                                       UniversalTime uT);

  private:
	CelestialBody const* parent;
	std::vector<CelestialBody*> children;
	Orbit* orbit;
	Parameters parameters;
};

#endif // CELESTIALBODY_HPP
