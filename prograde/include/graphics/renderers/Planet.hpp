/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@hotmail.fr>

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

#ifndef PLANET_H
#define PLANET_H

#include <QtConcurrent>

#include "AssetLoader.hpp"
#include "BasicCamera.hpp"
#include "GLHandler.hpp"
#include "Primitives.hpp"

#include "math/constants.hpp"

class Planet
{
  public:
	Planet(float radius                = 1.f,
	       QVector3D const& oblateness = QVector3D(1.0, 1.0, 1.0));

	void initTerrestrial(QColor const& color = QColor(255, 255, 255),
	                     float polarLatitude = 90.f, float atmosphere = 0.f);
	void initGazGiant(QColor const& color  = QColor(255, 255, 255),
	                  float bandsIntensity = 1.f, float stormsIntensity = 1.f);
	void initFromTex(QString const& diffusePath);
	void initFromTex(QString const& diffusePath, QString const& normalPath,
	                 float atmosphere = 0.f);
	bool updateModel(QString const& modelName);
	void updateTextureLoading();

	void initRing(float innerRing, float outerRing,
	              QString const& texturePath = "");
	void updateRing();
	void renderPlanet(QVector3D const& pos, QVector3D const& lightpos,
	                  QMatrix4x4 const& properRotation,
	                  bool flipCoords = false);
	void renderPlanet(QMatrix4x4 const& model, QVector3D const& lightpos,
	                  QMatrix4x4 const& properRotation,
	                  bool flipCoords = false);
	void renderRings(QVector3D const& pos, QVector3D const& lightpos,
	                 QMatrix4x4 const& properRotation);
	void renderRings(QMatrix4x4 const& model, QVector3D const& lightpos,
	                 QMatrix4x4 const& properRotation);
	bool isValid() const { return valid; };
	~Planet();

  private:
	void loadParallel(QString const& path);
	static void envMap(GLHandler::ShaderProgram& shader, GLHandler::Mesh& mesh,
	                   GLHandler::RenderTarget& renderTarget);

	bool valid = false;
	float radius;
	float atmosphere = 0.f;
	QVector3D oblateness;

	bool normal = false;

	GLHandler::ShaderProgram shader = {};
	GLHandler::Mesh mesh            = {};
	GLHandler::RenderTarget cubemapDiffuse;
	GLHandler::RenderTarget cubemapNormal;

	bool rings                 = false;
	bool ringTextured          = false;
	GLHandler::Texture ringtex = {};

	GLHandler::ShaderProgram ringShader   = {};
	GLHandler::Mesh ringMesh              = {};
	GLHandler::RenderTarget ringTexTarget = {};

	static unsigned int& cubemapsSize();

	// parallel QImage loading
	std::vector<QFuture<QImage>> futures;
};

#endif // PLANET_H
