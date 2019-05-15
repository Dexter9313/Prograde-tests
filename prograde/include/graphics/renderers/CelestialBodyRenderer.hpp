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

#ifndef CELESTIALBODYRENDERER_HPP
#define CELESTIALBODYRENDERER_HPP

#include <QFileInfo>

#include "Camera.hpp"
#include "GLHandler.hpp"
#include "Planet.hpp"
#include "Primitives.hpp"

#include "graphics/Utils.hpp"
#include "physics/CelestialBody.hpp"
//#include "../shaders/FarCelestialBodyShader.hpp"

class CelestialBodyRenderer //: public Renderer
{
  public:
	CelestialBodyRenderer(CelestialBody* drawnBody, std::string const& name);
	void updateMesh(UniversalTime uT, Camera const& camera);
	void render();
	CelestialBody const* getDrawnBody() const { return drawnBody; };
	void setCenterPosition(float centerPosition)
	{
		this->centerPosition = centerPosition;
	};
	~CelestialBodyRenderer();

  private:
	CelestialBody* drawnBody;
	float centerPosition;
	int shaderParametersId;

	/*
	GLHandler::Mesh mesh;
	GLHandler::ShaderProgram shader;*/
	QMatrix4x4 model;

	Planet planet;
	QVector3D lightpos;
	QMatrix4x4 baseRotation;   // only align axis, no sideral time
	QMatrix4x4 properRotation; // full rotation, sideral time included

	// TEMP
	float declinationTilt = 23.4392811 * constant::pi / 180.0;
};

#endif // CELESTIALBODYDRAWER_HPP
