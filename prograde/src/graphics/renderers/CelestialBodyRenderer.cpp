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
#include "../../../include/graphics/renderers/CelestialBodyRenderer.hpp"

CelestialBodyRenderer::CelestialBodyRenderer(CelestialBody* drawnBody,
                                             std::string const& name)
    : drawnBody(drawnBody)
    , boundingSphere(drawnBody->getParameters().radius)
    , pointShader(GLHandler::newShader("colored"))
    , pointMesh(GLHandler::newMesh())
    , unloadedShader(
          GLHandler::newShader("planet/planet", "planet/uniformplanet"))
    , unloadedMesh(Primitives::newUnitSphere(unloadedShader, 50, 50))
    , name(name.c_str())
{
	/*shader = GLHandler::newShader("farcelestialbody");
	GLHandler::setShaderParam(shader, "color",
	                          Utils::toQt(drawnBody->getParameters().color));

	mesh = Primitives::newUnitSphere(shader, 100, 100);*/

	// node = scene->addSphereSceneNode(this->drawnBody->getParameters().radius,
	// 64, 0, -1);
	// node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	// node->setMaterialType(FarCelestialBodyShader::getInstance()->getMaterialType());
	// this->scnMgr->getMeshManipulator()->setVertexColors(mesh->getMesh(),
	// toIrrlicht(this->drawnBody->getParameters().color));
	model = QMatrix4x4();
	model.translate(Utils::toQt(this->drawnBody->getAbsolutePositionAtUT(0)));

	// ROTATION

	Vector3 x(1.0, 0.0, 0.0), y(0.0, 1.0, 0.0);
	x.rotateAlongZ(drawnBody->getParameters().northPoleRightAsc);
	y.rotateAlongZ(drawnBody->getParameters().northPoleRightAsc);
	Vector3 planetLocalZ // north pole
	    = Matrix4x4(drawnBody->getParameters().northPoleDeclination, -1.0 * y)
	      * x;

	planetLocalZ.rotateAlongX(-1.f * declinationTilt);
	planetLocalZ = planetLocalZ.getUnitForm();

	Vector3 planetLocalY(
	    crossProduct(planetLocalZ, Vector3(1.0, 0.0, 0.0)).getUnitForm());
	Vector3 planetLocalX(
	    crossProduct(planetLocalY, planetLocalZ).getUnitForm());

	baseRotation = QMatrix4x4(
	    planetLocalX[0], planetLocalY[0], planetLocalZ[0], 0.f, planetLocalX[1],
	    planetLocalY[1], planetLocalZ[1], 0.f, planetLocalX[2], planetLocalY[2],
	    planetLocalZ[2], 0.f, 0.f, 0.f, 0.f, 1.f);

	// POINT
	Color color(drawnBody->getParameters().color);
	GLHandler::setVertices(
	    pointMesh,
	    {0.f, 0.f, 0.f, color.r / 255.f, color.g / 255.f, color.b / 255.f, 1.f},
	    pointShader, {{"position", 3}, {"color", 4}});

	// UNLOADED
	GLHandler::setShaderParam(unloadedShader, "color",
	                          Utils::toQt(drawnBody->getParameters().color));
	GLHandler::setShaderParam(
	    unloadedShader, "oblateness",
	    Utils::toQt(drawnBody->getParameters().oblateness));
}

void CelestialBodyRenderer::updateMesh(UniversalTime uT, Camera const& camera)
{
	Vector3 camRelPos = camera.getRelativePositionTo(drawnBody, uT);

	double camDist(camRelPos.length());
	double scale(centerPosition / camDist);

	QVector3D position(Utils::toQt(scale * camRelPos));

	double radiusScale(drawnBody->getParameters().radius * scale);
	/*if(radiusScale / centerPosition < 0.002)
	{
	    radiusScale = 0.002 * centerPosition;
	}*/

	// custom models have km units, not radius units
	if(customModel)
	{
		radiusScale *= 1000.0 / drawnBody->getParameters().radius;
	}

	if(drawnBody->getParameters().outerRing == 0.f)
	{
		culled = camera.shouldBeCulled(position, boundingSphere * scale);
	}
	else
	{
		// take rings into account !
		double cullingRadius(drawnBody->getParameters().outerRing * scale);
		culled = camera.shouldBeCulled(position, cullingRadius);
	}
	if(culled)
	{
		return;
	}

	model = QMatrix4x4();
	model.translate(Utils::toQt(scale * camRelPos));
	model.scale(radiusScale);

	apparentAngle = 2.0 * atan(drawnBody->getParameters().radius / camDist);

	if(apparentAngle < 0.005)
	{
		unloadPlanet();
		if(apparentAngle < 0.0026)
		{
			return;
		}
	}
	else
	{
		loadPlanet();
	}

	Vector3 bodyCenter(scale * camRelPos);
	Vector3 centralBodyCenter(-1 * scale * camera.getAbsolutePosition());

	lightpos = Utils::toQt(centralBodyCenter - bodyCenter);
	/*
	    GLHandler::setShaderParam(shader, "bodyCenter",
	   Utils::toQt(bodyCenter)); GLHandler::setShaderParam(shader,
	   "centralBodyCenter", Utils::toQt(centralBodyCenter));
	*/

	// custom models have (1, 0, 0) at planetographic origin
	// non custom have (-1, 0, 0) at planetographic origin
	float siderealTime = drawnBody->getPrimeMeridianSiderealTimeAtUT(uT)
	                     + (customModel ? 0.0 : constant::pi);

	QMatrix4x4 sideralRotation;
	sideralRotation.rotate(siderealTime * 180.f / constant::pi,
	                       QVector3D(0.f, 0.f, 1.f));

	properRotation = baseRotation * sideralRotation;

	if(planet != nullptr)
	{
		planet->updateTextureLoading();
	}
}

void CelestialBodyRenderer::render()
{
	if(culled)
	{
		return;
	}

	if(apparentAngle < 0.0026)
	{
		GLHandler::setUpRender(pointShader, model);
		GLHandler::render(pointMesh);
		return;
	}

	if(apparentAngle < 0.005 || !planet->isValid())
	{
		GLHandler::setShaderParam(unloadedShader, "lightpos", lightpos);
		GLHandler::setShaderParam(unloadedShader, "properRotation",
		                          properRotation);
		GLHandler::setUpRender(unloadedShader, model);
		GLHandler::render(unloadedMesh);
		return;
	}

	/*GLHandler::setUpRender(shader, model);
	GLHandler::render(mesh);*/
	planet->renderPlanet(model, lightpos, properRotation, customModel);
	planet->renderRings(model, lightpos, properRotation);
}

CelestialBodyRenderer::~CelestialBodyRenderer()
{
	/*GLHandler::deleteMesh(mesh);
	GLHandler::deleteShader(shader);*/
	GLHandler::deleteMesh(pointMesh);
	GLHandler::deleteShader(pointShader);
	GLHandler::deleteMesh(unloadedMesh);
	GLHandler::deleteShader(unloadedShader);
	unloadPlanet();
}

void CelestialBodyRenderer::loadPlanet()
{
	if(planet != nullptr)
	{
		return;
	}

	planet
	    = new Planet(1.f, Utils::toQt(drawnBody->getParameters().oblateness));

	QString diffuse(""), normal("");
	QString str(QString("data/prograde/images/") + name + "/diffuse.jpg");
	if(QFileInfo(str).exists())
	{
		diffuse = str;
	}
	str = QString("data/prograde/images/") + name + "/diffuse.png";
	if(QFileInfo(str).exists())
	{
		diffuse = str;
	}
	if(diffuse != "")
	{
		str = QString("data/prograde/images/") + name + "/normal.jpg";
		if(QFileInfo(str).exists())
		{
			normal = str;
		}
		str = QString("data/prograde/images/") + name + "/normal.png";
		if(QFileInfo(str).exists())
		{
			normal = str;
		}
	}

	if(normal != "")
	{
		planet->initFromTex(diffuse, normal,
		                    drawnBody->getParameters().atmosphere);
	}
	else if(diffuse != "")
	{
		planet->initFromTex(diffuse);
	}
	else if(drawnBody->getParameters().type == CelestialBody::Type::GAZGIANT)
	{
		planet->initGazGiant(Utils::toQt(drawnBody->getParameters().color));
	}
	else
	{
		planet->initTerrestrial(Utils::toQt(drawnBody->getParameters().color));
	}

	if(QFileInfo(QString("data/prograde/models/") + name + ".ply").exists())
	{
		float modelRadius(planet->updateModel(name + ".ply"));
		customModel = (modelRadius > 0.f);
		if(customModel)
		{
			boundingSphere = modelRadius * 1000.f;
		}
	}

	// RINGS

	float outerRing(drawnBody->getParameters().outerRing);
	if(drawnBody->getParameters().outerRing != 0.f)
	{
		float innerRing(drawnBody->getParameters().innerRing);
		float radius(drawnBody->getParameters().radius);
		QString rings;
		str = QString("data/prograde/images/") + name + "/rings.jpg";
		if(QFileInfo(str).exists())
		{
			rings = str;
		}
		str = QString("data/prograde/images/") + name + "/rings.png";
		if(QFileInfo(str).exists())
		{
			rings = str;
		}

		if(rings != "")
		{
			planet->initRing(innerRing / radius, outerRing / radius, rings);
		}
		else
		{
			planet->initRing(drawnBody->getParameters().innerRing / radius,
			                 drawnBody->getParameters().outerRing / radius);
		}
	}
}

void CelestialBodyRenderer::unloadPlanet()
{
	if(planet == nullptr)
	{
		return;
	}
	delete planet;
	planet = nullptr;
}
