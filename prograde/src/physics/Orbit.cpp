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

#include "physics/Orbit.hpp"

Orbit::Orbit(QJsonObject const& json)
    : massiveBodyMass(json["massiveBodyMass"].toDouble())
{
	QJsonObject paramsJSON(json["parameters"].toObject());
	parameters.inclination = paramsJSON["inclination"].toDouble();
	parameters.ascendingNodeLongitude
	    = paramsJSON["ascendingNodeLongitude"].toDouble();
	parameters.periapsisArgument  = paramsJSON["periapsisArgument"].toDouble();
	parameters.eccentricity       = paramsJSON["eccentricity"].toDouble();
	parameters.semiMajorAxis      = paramsJSON["semiMajorAxis"].toDouble();
	parameters.meanAnomalyAtEpoch = paramsJSON["meanAnomalyAtEpoch"].toDouble();

	updatePeriod();
}

Orbit::Orbit(MassiveBodyMass const& massiveBodyMass,
             Orbit::Parameters parameters)
    : parameters(parameters)
    , massiveBodyMass(massiveBodyMass.value)
{
	updatePeriod();
}

// TODO(florian) compute correctly
Orbit::Orbit(Period const& period, Orbit::Parameters parameters)
    : parameters(parameters)
    , massiveBodyMass(period.value)
{
	double smaCubed = parameters.semiMajorAxis * parameters.semiMajorAxis
	                  * parameters.semiMajorAxis;
	double mu = constant::G * massiveBodyMass;
	if(parameters.eccentricity < 1)
	{
		this->period = 2 * constant::pi * sqrt(smaCubed / mu);
	}
	else
	{
		this->period = constant::NaN;
	}
}

double Orbit::getMassiveBodyMass() const
{
	return massiveBodyMass;
}

Orbit::Parameters Orbit::getParameters() const
{
	return parameters;
}

double Orbit::getPeriod() const
{
	return period;
}

double Orbit::getMeanAnomalyAtUT(UniversalTime uT)
{
	// TODO(florian) : test assertion
	// to return a sensible meanAnomaly (we don't care about returning > 2*M_PI
	// values as big as we want mathematically but float precision Does care,
	// so we treat potentially huge uTs like this;
	// if uT < period, then the result will be between 0 and 2*M_PI)
	// several loops are done for performance (we don't want one loop which
	// loops millions of times)
	UniversalTime uT2 = fmod(uT, period);
	auto equivUT      = uT2.convert_to<double>();

	double smaCubed = parameters.semiMajorAxis * parameters.semiMajorAxis
	                  * parameters.semiMajorAxis;
	if(parameters.eccentricity > 1)
	{
		smaCubed *= -1;
	}
	double n(sqrt(constant::G * massiveBodyMass / smaCubed));
	return (n * equivUT) + parameters.meanAnomalyAtEpoch;
}

double Orbit::getEccentricAnomalyAtUT(UniversalTime uT)
{
	if(parameters.eccentricity < 1)
	{
		return EccentricAnomalySolver::solveForEllipticOrbit(
		    getMeanAnomalyAtUT(uT), parameters.eccentricity);
	}
	if(parameters.eccentricity == 1)
	{
		return EccentricAnomalySolver::solveForParabolicOrbit(
		    getMeanAnomalyAtUT(uT));
	}

	return EccentricAnomalySolver::solveForHyperbolicOrbit(
	    getMeanAnomalyAtUT(uT), parameters.eccentricity);
}

double Orbit::getTrueAnomalyAtUT(UniversalTime uT)
{
	if(parameters.eccentricity < 1)
	{
		double coeff(sqrt((1 + parameters.eccentricity)
		                  / (1 - parameters.eccentricity)));
		double eccentricAnomaly(getEccentricAnomalyAtUT(uT));

		return 2.0 * atan(coeff * tan(eccentricAnomaly / 2.0));
	}

	if(parameters.eccentricity == 1)
	{
		return 2.0 * atan(getEccentricAnomalyAtUT(uT));
	}

	double coeff(
	    sqrt((parameters.eccentricity + 1) / (parameters.eccentricity - 1)));
	double eccentricAnomaly(getEccentricAnomalyAtUT(uT));

	return 2.0 * atan(coeff * tan(eccentricAnomaly) / 2.0);
}

double Orbit::getMassiveBodyDistanceAtUT(UniversalTime uT)
{
	// TODO(florian) : solve situation for parabola
	double e(parameters.eccentricity);
	double a(parameters.semiMajorAxis);
	double t(getTrueAnomalyAtUT(uT));

	if(e != 1.0)
	{
		return a * (1.0 - (e * e)) / (1.0 + (e * cos(t)));
	}

	return 2.0 * a / (1.0 + cos(t));
}

Vector3 Orbit::getPositionAtUT(UniversalTime uT)
{
	if(uT != cacheUT)
	{
		double distance(getMassiveBodyDistanceAtUT(uT)),
		    trueAnomaly(getTrueAnomalyAtUT(uT));

		position.setXYZ(distance, 0.0, 0.0);
		position.rotateAlongZ(trueAnomaly + parameters.periapsisArgument);
		position.rotateAlongX(parameters.inclination);
		position.rotateAlongZ(parameters.ascendingNodeLongitude);

		cacheUT = uT;
	}

	return position;
}

// TODO(florian) compute correctly
/*Vector3 Orbit::getVelocityAtUT(UniversalTime uT) const
{
    return Vector;
}*/

CoordinateSystem Orbit::getRelativeCoordinateSystemAtUT(UniversalTime uT)
{
	CoordinateSystem result;
	result.setOrigin(getPositionAtUT(uT));
	result.rotateAlongX(parameters.inclination);
	result.rotateAlongZ(parameters.ascendingNodeLongitude);

	return result;
}

std::ostream& Orbit::displayAsText(std::ostream& stream) const
{
	stream << "[" << std::endl;
	stream << "\tMassiveBodyMass : " << massiveBodyMass << std::endl;
	stream << "\tInclination : " << parameters.inclination << std::endl;
	stream << "\tAscending Node Longitude : "
	       << parameters.ascendingNodeLongitude << std::endl;
	stream << "\tPeriapsis Argument : " << parameters.periapsisArgument
	       << std::endl;
	stream << "\tEccentricity : " << parameters.eccentricity << std::endl;
	stream << "\tSemi Major Axis : " << parameters.semiMajorAxis << std::endl;
	stream << "\tMean Anomaly At Epoch : " << parameters.meanAnomalyAtEpoch
	       << std::endl;
	stream << "]" << std::endl;
	stream << period << std::endl;

	return stream;
}

QJsonObject Orbit::getJSONRepresentation() const
{
	QJsonObject result;
	result["massiveBodyMass"] = massiveBodyMass;

	QJsonObject parametersResult;
	parametersResult["inclination"] = parameters.inclination;
	parametersResult["ascendingNodeLongitude"]
	    = parameters.ascendingNodeLongitude;
	parametersResult["periapsisArgument"]  = parameters.periapsisArgument;
	parametersResult["eccentricity"]       = parameters.eccentricity;
	parametersResult["semiMajorAxis"]      = parameters.semiMajorAxis;
	parametersResult["meanAnomalyAtEpoch"] = parameters.meanAnomalyAtEpoch;

	result["parameters"] = parametersResult;

	return result;
}

void Orbit::updatePeriod()
{
	double smaCubed = parameters.semiMajorAxis * parameters.semiMajorAxis
	                  * parameters.semiMajorAxis;
	double mu = constant::G * massiveBodyMass;
	if(parameters.eccentricity < 1)
	{
		period = 2 * constant::pi * sqrt(smaCubed / mu);
	}
	else
	{
		period = constant::NaN;
	}
}

std::ostream& operator<<(std::ostream& stream, Orbit const& orbit)
{
	return orbit.displayAsText(stream);
}
