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

#include "../../include/physics/OrbitalSystem.hpp"

OrbitalSystem::OrbitalSystem(QJsonObject const& json)
{
	centralMass     = json["centralMass"].toDouble();
	centralRadius   = json["centralRadius"].toDouble();
	declinationTilt = json["declinationTilt"].toDouble();

	QJsonObject jb(json["bodies"].toObject());

	current  = 0;
	progress = new QProgressDialog(QObject::tr("Loading System..."), QString(),
	                               0, jb.size());
	while(!jb.empty())
	{
		loadChildFromJSON(jb.begin().key(), jb);
	}
	delete progress;
}

OrbitalSystem::OrbitalSystem(double centralMass, double centralRadius,
                             double declinationTilt)
    : centralMass(centralMass)
    , centralRadius(centralRadius)
    , declinationTilt(declinationTilt)
{
}

void OrbitalSystem::createChild(std::string const& name,
                                QJsonObject const& json,
                                std::string const& parent)
{
	if(!parent.empty())
	{
		bodies[name] = bodies[parent]->createChild(json, name);
	}
	else
	{
		bodies[name] = new CelestialBody(json, name, centralMass);
	}
}

void OrbitalSystem::createChild(
    std::string const& name, Orbit::Parameters const& orbitalParameters,
    CelestialBody::Parameters const& physicalParameters,
    std::string const& parent)
{
	if(!parent.empty())
	{
		bodies[name] = bodies[parent]->createChild(orbitalParameters,
		                                           physicalParameters);
	}
	else
	{
		bodies[name] = new CelestialBody(centralMass, orbitalParameters,
		                                 physicalParameters);
	}
}

void OrbitalSystem::createChild(
    std::string const& name,
    CelestialBody::Parameters const& physicalParameters,
    std::string const& parent)
{
	if(!parent.empty())
	{
		bodies[name] = bodies[parent]->createChild(name, physicalParameters);
	}
	else
	{
		bodies[name] = new CelestialBody(centralMass, name, physicalParameters);
	}
}

CelestialBody* OrbitalSystem::operator[](std::string const& name)
{
	return bodies[name];
}

CelestialBody const* OrbitalSystem::operator[](std::string const& name) const
{
	return bodies.at(name);
}

std::vector<std::string> OrbitalSystem::getAllCelestialBodiesNames() const
{
	std::vector<std::string> result;

	for(auto pair : bodies)
	{
		result.push_back(pair.first);
	}

	return result;
}

std::vector<CelestialBody*> OrbitalSystem::getAllCelestialBodiesPointers() const
{
	std::vector<CelestialBody*> result;

	for(auto pair : bodies)
	{
		result.push_back(pair.second);
	}

	return result;
}

std::vector<std::string> OrbitalSystem::getParentCelestialBodiesNames() const
{
	std::vector<std::string> result;

	for(auto pair : bodies)
	{
		if(pair.second->getParent() == nullptr)
		{
			result.push_back(pair.first);
		}
	}

	return result;
}

std::vector<CelestialBody*>
    OrbitalSystem::getParentCelestialBodiesPointers() const
{
	std::vector<CelestialBody*> result;

	for(auto pair : bodies)
	{
		if(pair.second->getParent() == nullptr)
		{
			result.push_back(pair.second);
		}
	}

	return result;
}

QJsonObject OrbitalSystem::getJSONRepresentation() const
{
	QJsonObject result;
	result["centralMass"]     = centralMass;
	result["centralRadius"]   = centralRadius;
	result["declinationTilt"] = declinationTilt;

	QJsonObject bodiesJSON;
	for(auto& pair : bodies)
	{
		QJsonObject bodyObj = pair.second->getJSONRepresentation();
		if(pair.second->getParent() != nullptr)
		{
			std::vector<CelestialBody*> ptrs(getAllCelestialBodiesPointers());
			for(unsigned int i(0); i < ptrs.size(); ++i)
			{
				if(pair.second->getParent() == ptrs[i])
				{
					bodyObj["parent"] = getAllCelestialBodiesNames()[i].c_str();
					break;
				}
			}
		}
		bodiesJSON[pair.first.c_str()] = bodyObj;
	}
	result["bodies"] = bodiesJSON;

	return result;
}

void OrbitalSystem::loadChildFromJSON(QString const& name, QJsonObject& json)
{
	QJsonObject bodyJSON(json[name].toObject());

	if(bodyJSON.contains("parent"))
	{
		std::string parentName(bodyJSON["parent"].toString().toStdString());
		if(bodies.count(parentName) == 0)
		{
			loadChildFromJSON(parentName.c_str(), json);
		}
		createChild(name.toStdString(), bodyJSON,
		            bodyJSON["parent"].toString().toStdString());
	}
	else
	{
		createChild(name.toStdString(), bodyJSON);
	}

	json.erase(json.find(name));
	++current;
	progress->setValue(current);
	QCoreApplication::processEvents();
}

OrbitalSystem::~OrbitalSystem()
{
	for(CelestialBody* parentBody : getParentCelestialBodiesPointers())
	{
		delete parentBody;
	}
}
