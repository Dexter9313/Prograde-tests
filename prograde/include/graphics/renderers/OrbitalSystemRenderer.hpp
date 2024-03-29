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

#ifndef ORBITALSYSTEMRENDERER_HPP
#define ORBITALSYSTEMRENDERER_HPP

#include "Billboard.hpp"

#include "Camera.hpp"
#include "CelestialBodyRenderer.hpp"
#include "graphics/Utils.hpp"
#include "physics/OrbitalSystem.hpp"

// TODO rethink Renderer (mesh attribute)
class OrbitalSystemRenderer //: public Renderer
{
  public:
	OrbitalSystemRenderer(OrbitalSystem* drawnSystem);
	void updateMesh(UniversalTime uT, Camera const& camera);
	void render(BasicCamera const& camera);
	~OrbitalSystemRenderer();

  private:
	OrbitalSystem* drawnSystem;
	std::vector<CelestialBodyRenderer*> bodyRenderers;

	std::map<double, CelestialBodyRenderer*> sortedRenderers;
	double camDist;

	double billboardOriginalEdgeSize;

	// TEMP
	/*GLHandler::Mesh mesh;
	GLHandler::ShaderProgram shader;
	QMatrix4x4 model;*/
	Billboard billboard;
};

#endif // ORBITALSYSTEMDRAWER_HPP
